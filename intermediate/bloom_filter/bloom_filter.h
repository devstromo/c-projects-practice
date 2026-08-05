#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t *bits;          // Bit array
    size_t size_bits;       // Number of bits in the filter
    size_t size_bytes;      // Number of bytes allocated
    size_t num_hash_funcs;  // Number of hash functions (k)
    size_t items_added;     // Count of items added
} BloomFilter;

// Create a new Bloom filter
// expected_items: expected number of items to insert
// false_positive_rate: desired false positive rate (0.0 to 1.0, e.g., 0.01 for 1%)
BloomFilter *bloom_create(size_t expected_items, double false_positive_rate);

// Add an item to the filter
void bloom_add(BloomFilter *filter, const void *data, size_t len);

// Add a null-terminated string to the filter
void bloom_add_string(BloomFilter *filter, const char *str);

// Check if an item might be in the filter
// Returns: true = possibly in set, false = definitely not in set
bool bloom_contains(BloomFilter *filter, const void *data, size_t len);

// Check if a null-terminated string might be in the filter
bool bloom_contains_string(BloomFilter *filter, const char *str);

// Get the current false positive probability based on items added
double bloom_false_positive_rate(BloomFilter *filter);

// Clear all bits (reset the filter)
void bloom_clear(BloomFilter *filter);

// Free the Bloom filter
void bloom_destroy(BloomFilter *filter);

// Print filter statistics
void bloom_print_stats(BloomFilter *filter);

#endif // BLOOM_FILTER_H
