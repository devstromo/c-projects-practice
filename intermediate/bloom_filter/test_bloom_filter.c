#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "bloom_filter.h"

#define TEST_PASSED printf("  [PASS] %s\n", __func__)
#define TEST_FAILED(msg) do { printf("  [FAIL] %s: %s\n", __func__, msg); return 0; } while(0)

static int tests_run = 0;
static int tests_passed = 0;

// ============================================================================
// Test Functions
// ============================================================================

int test_create_and_destroy(void) {
    BloomFilter *filter = bloom_create(100, 0.01);
    if (filter == NULL) TEST_FAILED("Failed to create filter");

    if (filter->size_bits == 0) TEST_FAILED("Filter size is 0");
    if (filter->num_hash_funcs == 0) TEST_FAILED("No hash functions");
    if (filter->bits == NULL) TEST_FAILED("Bit array is NULL");
    if (filter->items_added != 0) TEST_FAILED("Items count should be 0");

    bloom_destroy(filter);
    TEST_PASSED;
    return 1;
}

int test_invalid_parameters(void) {
    // Expected items = 0
    BloomFilter *f1 = bloom_create(0, 0.01);
    if (f1 != NULL) {
        bloom_destroy(f1);
        TEST_FAILED("Should fail with 0 expected items");
    }

    // Invalid false positive rates
    BloomFilter *f2 = bloom_create(100, 0.0);
    if (f2 != NULL) {
        bloom_destroy(f2);
        TEST_FAILED("Should fail with 0.0 fp rate");
    }

    BloomFilter *f3 = bloom_create(100, 1.0);
    if (f3 != NULL) {
        bloom_destroy(f3);
        TEST_FAILED("Should fail with 1.0 fp rate");
    }

    BloomFilter *f4 = bloom_create(100, -0.1);
    if (f4 != NULL) {
        bloom_destroy(f4);
        TEST_FAILED("Should fail with negative fp rate");
    }

    TEST_PASSED;
    return 1;
}

int test_add_and_contains_string(void) {
    BloomFilter *filter = bloom_create(100, 0.01);
    if (filter == NULL) TEST_FAILED("Failed to create filter");

    // Add some strings
    bloom_add_string(filter, "hello");
    bloom_add_string(filter, "world");
    bloom_add_string(filter, "bloom");
    bloom_add_string(filter, "filter");

    // Check they exist
    if (!bloom_contains_string(filter, "hello")) TEST_FAILED("'hello' should exist");
    if (!bloom_contains_string(filter, "world")) TEST_FAILED("'world' should exist");
    if (!bloom_contains_string(filter, "bloom")) TEST_FAILED("'bloom' should exist");
    if (!bloom_contains_string(filter, "filter")) TEST_FAILED("'filter' should exist");

    // Items count
    if (filter->items_added != 4) TEST_FAILED("Should have 4 items");

    bloom_destroy(filter);
    TEST_PASSED;
    return 1;
}

int test_add_and_contains_binary(void) {
    BloomFilter *filter = bloom_create(100, 0.01);
    if (filter == NULL) TEST_FAILED("Failed to create filter");

    // Add binary data
    int values[] = {42, 100, 255, 1000};
    for (int i = 0; i < 4; i++) {
        bloom_add(filter, &values[i], sizeof(int));
    }

    // Check they exist
    for (int i = 0; i < 4; i++) {
        if (!bloom_contains(filter, &values[i], sizeof(int))) {
            TEST_FAILED("Binary value should exist");
        }
    }

    bloom_destroy(filter);
    TEST_PASSED;
    return 1;
}

int test_no_false_negatives(void) {
    BloomFilter *filter = bloom_create(1000, 0.01);
    if (filter == NULL) TEST_FAILED("Failed to create filter");

    // Add 500 items
    char buf[32];
    for (int i = 0; i < 500; i++) {
        snprintf(buf, sizeof(buf), "item_%d", i);
        bloom_add_string(filter, buf);
    }

    // Verify ALL added items are found (no false negatives)
    for (int i = 0; i < 500; i++) {
        snprintf(buf, sizeof(buf), "item_%d", i);
        if (!bloom_contains_string(filter, buf)) {
            bloom_destroy(filter);
            TEST_FAILED("False negative detected - this should never happen");
        }
    }

    bloom_destroy(filter);
    TEST_PASSED;
    return 1;
}

int test_false_positive_rate(void) {
    // Test with 1% expected false positive rate
    double target_fp_rate = 0.01;
    BloomFilter *filter = bloom_create(1000, target_fp_rate);
    if (filter == NULL) TEST_FAILED("Failed to create filter");

    // Add 1000 items
    char buf[32];
    for (int i = 0; i < 1000; i++) {
        snprintf(buf, sizeof(buf), "exists_%d", i);
        bloom_add_string(filter, buf);
    }

    // Test 10000 non-existing items and count false positives
    int false_positives = 0;
    for (int i = 0; i < 10000; i++) {
        snprintf(buf, sizeof(buf), "notexists_%d", i);
        if (bloom_contains_string(filter, buf)) {
            false_positives++;
        }
    }

    double actual_fp_rate = (double)false_positives / 10000.0;

    // Allow some variance - should be within 3x of target (statistical variance)
    if (actual_fp_rate > target_fp_rate * 3.0) {
        printf("    (actual FP rate: %.4f, target: %.4f)\n", actual_fp_rate, target_fp_rate);
        bloom_destroy(filter);
        TEST_FAILED("False positive rate too high");
    }

    printf("    (actual FP rate: %.4f%%, target: %.2f%%)\n",
           actual_fp_rate * 100, target_fp_rate * 100);

    bloom_destroy(filter);
    TEST_PASSED;
    return 1;
}

int test_clear(void) {
    BloomFilter *filter = bloom_create(100, 0.01);
    if (filter == NULL) TEST_FAILED("Failed to create filter");

    // Add items
    bloom_add_string(filter, "test1");
    bloom_add_string(filter, "test2");

    if (filter->items_added != 2) TEST_FAILED("Should have 2 items");
    if (!bloom_contains_string(filter, "test1")) TEST_FAILED("test1 should exist before clear");

    // Clear
    bloom_clear(filter);

    if (filter->items_added != 0) TEST_FAILED("Items should be 0 after clear");

    // After clear, items should not be found (with very high probability)
    // Note: Due to the nature of Bloom filters, technically a cleared filter
    // *could* still return true for some queries if we had a degenerate hash,
    // but with proper hashing this should be false.
    if (bloom_contains_string(filter, "test1")) {
        TEST_FAILED("test1 should not exist after clear");
    }

    bloom_destroy(filter);
    TEST_PASSED;
    return 1;
}

int test_different_sizes(void) {
    // Test various sizes
    size_t sizes[] = {10, 100, 1000, 10000};
    double fp_rates[] = {0.1, 0.01, 0.001};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            BloomFilter *filter = bloom_create(sizes[i], fp_rates[j]);
            if (filter == NULL) {
                printf("    Failed for size=%zu, fp_rate=%.3f\n", sizes[i], fp_rates[j]);
                TEST_FAILED("Failed to create filter");
            }

            // Verify reasonable parameters
            if (filter->size_bits < 64) {
                bloom_destroy(filter);
                TEST_FAILED("Filter too small");
            }

            bloom_destroy(filter);
        }
    }

    TEST_PASSED;
    return 1;
}

int test_null_handling(void) {
    BloomFilter *filter = bloom_create(100, 0.01);
    if (filter == NULL) TEST_FAILED("Failed to create filter");

    // These should not crash
    bloom_add(filter, NULL, 0);
    bloom_add(filter, NULL, 10);
    bloom_add_string(filter, NULL);

    if (bloom_contains(filter, NULL, 0)) TEST_FAILED("NULL should not be found");
    if (bloom_contains(filter, NULL, 10)) TEST_FAILED("NULL should not be found");
    if (bloom_contains_string(filter, NULL)) TEST_FAILED("NULL should not be found");

    // Operations on NULL filter should not crash
    bloom_add(NULL, "test", 4);
    bloom_add_string(NULL, "test");
    bloom_contains(NULL, "test", 4);
    bloom_contains_string(NULL, "test");
    bloom_clear(NULL);
    bloom_destroy(NULL);

    bloom_destroy(filter);
    TEST_PASSED;
    return 1;
}

int test_similar_strings(void) {
    BloomFilter *filter = bloom_create(100, 0.001);
    if (filter == NULL) TEST_FAILED("Failed to create filter");

    // Add specific strings
    bloom_add_string(filter, "test");

    // Check similar but different strings
    if (!bloom_contains_string(filter, "test")) TEST_FAILED("'test' should exist");

    // These should very likely not match (though false positives are possible)
    // With 0.1% FP rate, we shouldn't see many false positives
    int unexpected_matches = 0;
    const char *similar[] = {"test1", "test2", "Test", "TEST", "tes", "tset", "est"};

    for (int i = 0; i < 7; i++) {
        if (bloom_contains_string(filter, similar[i])) {
            unexpected_matches++;
        }
    }

    // Allow at most 1 false positive out of 7 tests (statistically reasonable)
    if (unexpected_matches > 1) {
        printf("    (unexpected matches: %d/7)\n", unexpected_matches);
        bloom_destroy(filter);
        TEST_FAILED("Too many false positives for similar strings");
    }

    bloom_destroy(filter);
    TEST_PASSED;
    return 1;
}

int test_fp_rate_calculation(void) {
    BloomFilter *filter = bloom_create(100, 0.01);
    if (filter == NULL) TEST_FAILED("Failed to create filter");

    // Initially should be 0
    double initial_rate = bloom_false_positive_rate(filter);
    if (initial_rate != 0.0) TEST_FAILED("Initial FP rate should be 0");

    // Add items and verify rate increases
    for (int i = 0; i < 50; i++) {
        char buf[32];
        snprintf(buf, sizeof(buf), "item_%d", i);
        bloom_add_string(filter, buf);
    }

    double after_rate = bloom_false_positive_rate(filter);
    if (after_rate <= 0.0) TEST_FAILED("FP rate should be > 0 after adding items");
    if (after_rate >= 1.0) TEST_FAILED("FP rate should be < 1");

    bloom_destroy(filter);
    TEST_PASSED;
    return 1;
}

// ============================================================================
// Test Runner
// ============================================================================

typedef int (*TestFunc)(void);

typedef struct {
    const char *name;
    TestFunc func;
} TestCase;

int main(void) {
    printf("\n========================================\n");
    printf("   Bloom Filter Unit Tests\n");
    printf("========================================\n\n");

    TestCase tests[] = {
        {"Create and Destroy", test_create_and_destroy},
        {"Invalid Parameters", test_invalid_parameters},
        {"Add and Contains (String)", test_add_and_contains_string},
        {"Add and Contains (Binary)", test_add_and_contains_binary},
        {"No False Negatives", test_no_false_negatives},
        {"False Positive Rate", test_false_positive_rate},
        {"Clear Filter", test_clear},
        {"Different Sizes", test_different_sizes},
        {"NULL Handling", test_null_handling},
        {"Similar Strings", test_similar_strings},
        {"FP Rate Calculation", test_fp_rate_calculation},
    };

    int num_tests = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < num_tests; i++) {
        printf("Running: %s\n", tests[i].name);
        tests_run++;
        if (tests[i].func()) {
            tests_passed++;
        }
    }

    printf("\n========================================\n");
    printf("   Results: %d/%d tests passed\n", tests_passed, tests_run);
    printf("========================================\n\n");

    return (tests_passed == tests_run) ? 0 : 1;
}
