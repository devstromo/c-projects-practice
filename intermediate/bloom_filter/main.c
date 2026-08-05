#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bloom_filter.h"

#define MAX_INPUT_LEN 256

// ============================================================================
// Demo: Key-Value Store Simulation
// ============================================================================

typedef struct {
    char key[64];
    char value[128];
} KeyValuePair;

typedef struct {
    KeyValuePair *entries;
    size_t capacity;
    size_t count;
    BloomFilter *filter;
    size_t bloom_hits;      // Filter said "maybe exists"
    size_t bloom_misses;    // Filter said "definitely not"
    size_t actual_found;    // Actually found in store
} SimpleKVS;

SimpleKVS *kvs_create(size_t capacity, double fp_rate) {
    SimpleKVS *kvs = (SimpleKVS *)malloc(sizeof(SimpleKVS));
    if (kvs == NULL) {
        return NULL;
    }

    kvs->entries = (KeyValuePair *)malloc(capacity * sizeof(KeyValuePair));
    if (kvs->entries == NULL) {
        free(kvs);
        return NULL;
    }

    kvs->filter = bloom_create(capacity, fp_rate);
    if (kvs->filter == NULL) {
        free(kvs->entries);
        free(kvs);
        return NULL;
    }

    kvs->capacity = capacity;
    kvs->count = 0;
    kvs->bloom_hits = 0;
    kvs->bloom_misses = 0;
    kvs->actual_found = 0;

    return kvs;
}

void kvs_destroy(SimpleKVS *kvs) {
    if (kvs != NULL) {
        bloom_destroy(kvs->filter);
        free(kvs->entries);
        free(kvs);
    }
}

bool kvs_put(SimpleKVS *kvs, const char *key, const char *value) {
    if (kvs == NULL || kvs->count >= kvs->capacity) {
        return false;
    }

    // Check if key already exists
    for (size_t i = 0; i < kvs->count; i++) {
        if (strcmp(kvs->entries[i].key, key) == 0) {
            strncpy(kvs->entries[i].value, value, sizeof(kvs->entries[i].value) - 1);
            kvs->entries[i].value[sizeof(kvs->entries[i].value) - 1] = '\0';
            return true;
        }
    }

    // Add new entry
    strncpy(kvs->entries[kvs->count].key, key, sizeof(kvs->entries[kvs->count].key) - 1);
    kvs->entries[kvs->count].key[sizeof(kvs->entries[kvs->count].key) - 1] = '\0';
    strncpy(kvs->entries[kvs->count].value, value, sizeof(kvs->entries[kvs->count].value) - 1);
    kvs->entries[kvs->count].value[sizeof(kvs->entries[kvs->count].value) - 1] = '\0';

    // Add to Bloom filter
    bloom_add_string(kvs->filter, key);

    kvs->count++;
    return true;
}

// Returns NULL if not found
const char *kvs_get(SimpleKVS *kvs, const char *key) {
    if (kvs == NULL) {
        return NULL;
    }

    // First check Bloom filter - this is the optimization!
    if (!bloom_contains_string(kvs->filter, key)) {
        // Bloom filter says definitely not here - skip the expensive lookup
        kvs->bloom_misses++;
        return NULL;
    }

    // Bloom filter says maybe here - need to check actual data
    kvs->bloom_hits++;

    // Linear search (simulating disk read in a real KVS)
    for (size_t i = 0; i < kvs->count; i++) {
        if (strcmp(kvs->entries[i].key, key) == 0) {
            kvs->actual_found++;
            return kvs->entries[i].value;
        }
    }

    // False positive - Bloom said maybe, but wasn't actually there
    return NULL;
}

void kvs_print_stats(SimpleKVS *kvs) {
    printf("\n=== KVS Statistics ===\n");
    printf("Entries: %zu / %zu\n", kvs->count, kvs->capacity);
    printf("Bloom filter lookups:\n");
    printf("  - Definite misses (saved lookups): %zu\n", kvs->bloom_misses);
    printf("  - Possible hits (needed lookup): %zu\n", kvs->bloom_hits);
    printf("  - Actually found: %zu\n", kvs->actual_found);

    if (kvs->bloom_hits > 0) {
        size_t false_positives = kvs->bloom_hits - kvs->actual_found;
        printf("  - False positives: %zu (%.2f%%)\n", false_positives,
               (double)false_positives / kvs->bloom_hits * 100.0);
    }

    printf("\nBloom filter details:\n");
    bloom_print_stats(kvs->filter);
}

// ============================================================================
// Interactive Demo
// ============================================================================

void print_menu(void) {
    printf("\n========== Bloom Filter Demo ==========\n");
    printf("1. Add key-value pair\n");
    printf("2. Get value by key\n");
    printf("3. Check if key might exist (Bloom only)\n");
    printf("4. Show statistics\n");
    printf("5. Run automated demo\n");
    printf("6. Exit\n");
    printf("========================================\n");
    printf("Choose an option: ");
}

void run_automated_demo(void) {
    printf("\n--- Running Automated Demo ---\n\n");

    // Create a KVS with 1000 expected items and 1% false positive rate
    SimpleKVS *kvs = kvs_create(1000, 0.01);
    if (kvs == NULL) {
        printf("Failed to create KVS\n");
        return;
    }

    // Insert some keys
    printf("Inserting 100 keys (user_0 to user_99)...\n");
    for (int i = 0; i < 100; i++) {
        char key[32], value[64];
        snprintf(key, sizeof(key), "user_%d", i);
        snprintf(value, sizeof(value), "data_for_user_%d", i);
        kvs_put(kvs, key, value);
    }

    // Query existing keys
    printf("\nQuerying 50 existing keys...\n");
    for (int i = 0; i < 50; i++) {
        char key[32];
        snprintf(key, sizeof(key), "user_%d", i);
        kvs_get(kvs, key);
    }

    // Query non-existing keys (most should be filtered by Bloom)
    printf("Querying 200 non-existing keys...\n");
    for (int i = 1000; i < 1200; i++) {
        char key[32];
        snprintf(key, sizeof(key), "user_%d", i);
        kvs_get(kvs, key);
    }

    kvs_print_stats(kvs);
    kvs_destroy(kvs);

    printf("\n--- Demo Complete ---\n");
}

int main(int argc, char *argv[]) {
    // Check for command line mode
    if (argc > 1 && strcmp(argv[1], "--demo") == 0) {
        run_automated_demo();
        return 0;
    }

    printf("Bloom Filter for Key-Value Store\n");
    printf("=================================\n");

    // Create KVS with 100 expected items and 1% false positive rate
    SimpleKVS *kvs = kvs_create(100, 0.01);
    if (kvs == NULL) {
        fprintf(stderr, "Failed to create KVS\n");
        return 1;
    }

    char input[MAX_INPUT_LEN];
    char key[64];
    char value[128];
    int choice;

    while (1) {
        print_menu();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        if (sscanf(input, "%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1:
                printf("Enter key: ");
                if (fgets(key, sizeof(key), stdin) == NULL) break;
                key[strcspn(key, "\n")] = '\0';

                printf("Enter value: ");
                if (fgets(value, sizeof(value), stdin) == NULL) break;
                value[strcspn(value, "\n")] = '\0';

                if (kvs_put(kvs, key, value)) {
                    printf("Added: %s -> %s\n", key, value);
                } else {
                    printf("Failed to add key-value pair\n");
                }
                break;

            case 2:
                printf("Enter key to look up: ");
                if (fgets(key, sizeof(key), stdin) == NULL) break;
                key[strcspn(key, "\n")] = '\0';

                const char *result = kvs_get(kvs, key);
                if (result != NULL) {
                    printf("Found: %s -> %s\n", key, result);
                } else {
                    printf("Key '%s' not found\n", key);
                }
                break;

            case 3:
                printf("Enter key to check: ");
                if (fgets(key, sizeof(key), stdin) == NULL) break;
                key[strcspn(key, "\n")] = '\0';

                if (bloom_contains_string(kvs->filter, key)) {
                    printf("Key '%s' MIGHT exist (check actual store)\n", key);
                } else {
                    printf("Key '%s' DEFINITELY does not exist\n", key);
                }
                break;

            case 4:
                kvs_print_stats(kvs);
                break;

            case 5:
                run_automated_demo();
                break;

            case 6:
                printf("Goodbye!\n");
                kvs_destroy(kvs);
                return 0;

            default:
                printf("Invalid option. Please choose 1-6.\n");
        }
    }

    kvs_destroy(kvs);
    return 0;
}
