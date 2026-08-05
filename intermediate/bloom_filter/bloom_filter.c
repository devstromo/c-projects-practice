#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bloom_filter.h"

// ============================================================================
// Hash Functions
// ============================================================================

/**
 * MurmurHash3 scramble function (32-bit mixing)
 *
 * Applies bit-mixing transformations to a 32-bit block to maximize avalanche
 * effect (small input changes cause large output changes).
 *
 * Steps:
 *   1. Multiply by c1 (0xcc9e2d51) - spreads bits via multiplication
 *   2. Rotate left by 15 bits     - mixes high and low bits
 *   3. Multiply by c2 (0x1b873593) - further diffusion
 *
 * These constants are carefully chosen primes that provide good bit diffusion.
 * The rotation ensures bits from any position affect all other positions.
 *
 * @param k  The 32-bit block to scramble
 * @return   The scrambled 32-bit value
 */
static uint32_t murmur3_32_scramble(uint32_t k) {
    k *= 0xcc9e2d51;            // c1: multiplication spreads bits
    k = (k << 15) | (k >> 17);  // ROTL15: circular left rotation by 15
    k *= 0x1b873593;            // c2: second multiplication for more diffusion
    return k;
}

/**
 * MurmurHash3 - 32-bit implementation
 *
 * A fast, non-cryptographic hash function created by Austin Appleby.
 * Provides excellent distribution and avalanche properties for hash tables.
 *
 * Algorithm overview:
 *   1. Process input in 4-byte blocks, mixing each into the hash state
 *   2. Handle remaining 1-3 bytes separately
 *   3. Apply finalization mixing to ensure all bits affect the output
 *
 * Properties:
 *   - O(n) time complexity where n is input length
 *   - Excellent avalanche: each input bit affects ~50% of output bits
 *   - No cryptographic security (not collision-resistant against attacks)
 *   - Deterministic: same input + seed always produces same output
 *
 * @param key   Pointer to the input data
 * @param len   Length of input data in bytes
 * @param seed  Initial seed value (allows different hash "families")
 * @return      32-bit hash value
 */
static uint32_t murmur3_32(const uint8_t *key, size_t len, uint32_t seed) {
    uint32_t h = seed;
    uint32_t k;

    // Process 4-byte chunks (len >> 2 = len / 4)
    // Each block is scrambled and XORed into the running hash
    for (size_t i = len >> 2; i; i--) {
        memcpy(&k, key, sizeof(uint32_t));  // Safe unaligned read
        key += sizeof(uint32_t);
        h ^= murmur3_32_scramble(k);
        h = (h << 13) | (h >> 19);          // ROTL13: rotate left by 13
        h = h * 5 + 0xe6546b64;             // Mix with prime multiplier
    }

    // Process remaining 1-3 bytes (len & 3 = len % 4)
    // Pack remaining bytes into k, then scramble
    k = 0;
    for (size_t i = len & 3; i; i--) {
        k <<= 8;
        k |= key[i - 1];
    }
    h ^= murmur3_32_scramble(k);

    // Finalization: force all bits to avalanche
    // This ensures even small inputs produce well-distributed hashes
    h ^= (uint32_t)len;     // Mix in the length
    h ^= h >> 16;           // Mix high bits into low bits
    h *= 0x85ebca6b;        // Multiplication by prime
    h ^= h >> 13;           // More bit mixing
    h *= 0xc2b2ae35;        // Another prime multiplication
    h ^= h >> 16;           // Final mix

    return h;
}

/**
 * FNV-1a hash function (32-bit)
 *
 * Fowler-Noll-Vo hash, variant 1a. A simple, fast hash function that
 * processes input one byte at a time. The "1a" variant XORs before
 * multiplying, which provides slightly better avalanche than FNV-1.
 *
 * Algorithm:
 *   hash = FNV_offset_basis XOR seed
 *   for each byte:
 *       hash = hash XOR byte
 *       hash = hash * FNV_prime
 *
 * Constants:
 *   - FNV_offset_basis (32-bit): 2166136261 (0x811c9dc5)
 *   - FNV_prime (32-bit):        16777619   (0x01000193)
 *
 * Used here as a secondary hash for double-hashing in the Bloom filter.
 * Combined with MurmurHash3, it allows generating k hash values efficiently:
 *   h(i) = murmur3(data) + i * fnv1a(data)
 *
 * @param data  Pointer to input data
 * @param len   Length of input data in bytes
 * @param seed  Seed value XORed with offset basis for variation
 * @return      32-bit hash value
 */
static uint32_t fnv1a_32(const uint8_t *data, size_t len, uint32_t seed) {
    uint32_t hash = 2166136261u ^ seed;  // FNV offset basis XOR seed
    for (size_t i = 0; i < len; i++) {
        hash ^= data[i];                  // XOR with byte (1a variant)
        hash *= 16777619u;                // Multiply by FNV prime
    }
    return hash;
}

/**
 * Generate k hash indices using double hashing technique
 *
 * Instead of computing k independent hash functions (expensive), we use
 * the double hashing optimization from Kirsch & Mitzenmacher (2006):
 *
 *   g(i) = h1(x) + i * h2(x)  for i = 0, 1, ..., k-1
 *
 * This produces k hash values with only 2 hash function calls, while
 * maintaining the same false positive probability as k independent hashes.
 *
 * Paper: "Less Hashing, Same Performance: Building a Better Bloom Filter"
 * https://www.eecs.harvard.edu/~michaelm/postscripts/rsa2008.pdf
 *
 * @param data         Input data to hash
 * @param len          Length of input data
 * @param num_hashes   Number of hash indices to generate (k)
 * @param filter_size  Size of the Bloom filter in bits (for modulo)
 * @param indices      Output array to store k bit indices
 */
static void get_hash_indices(const void *data, size_t len, size_t num_hashes,
                             size_t filter_size, size_t *indices) {
    uint32_t h1 = murmur3_32((const uint8_t *)data, len, 0);
    uint32_t h2 = fnv1a_32((const uint8_t *)data, len, h1);

    for (size_t i = 0; i < num_hashes; i++) {
        indices[i] = (h1 + i * h2) % filter_size;
    }
}

// ============================================================================
// Bit Operations
// ============================================================================

static inline void set_bit(uint8_t *bits, size_t index) {
    bits[index / 8] |= (1 << (index % 8));
}

static inline bool get_bit(const uint8_t *bits, size_t index) {
    return (bits[index / 8] & (1 << (index % 8))) != 0;
}

// ============================================================================
// Bloom Filter Implementation
// ============================================================================

BloomFilter *bloom_create(size_t expected_items, double false_positive_rate) {
    if (expected_items == 0 || false_positive_rate <= 0.0 || false_positive_rate >= 1.0) {
        fprintf(stderr, "Error: Invalid parameters for Bloom filter creation\n");
        return NULL;
    }

    BloomFilter *filter = (BloomFilter *)malloc(sizeof(BloomFilter));
    if (filter == NULL) {
        perror("Failed to allocate Bloom filter");
        return NULL;
    }

    // Calculate optimal number of bits: m = -n * ln(p) / (ln(2)^2)
    double ln2 = log(2.0);
    double ln2_sq = ln2 * ln2;
    double m = -((double)expected_items * log(false_positive_rate)) / ln2_sq;
    filter->size_bits = (size_t)ceil(m);

    // Ensure minimum size
    if (filter->size_bits < 64) {
        filter->size_bits = 64;
    }

    // Calculate optimal number of hash functions: k = (m/n) * ln(2)
    double k = ((double)filter->size_bits / (double)expected_items) * ln2;
    filter->num_hash_funcs = (size_t)ceil(k);

    // Clamp hash functions to reasonable range
    if (filter->num_hash_funcs < 1) {
        filter->num_hash_funcs = 1;
    } else if (filter->num_hash_funcs > 20) {
        filter->num_hash_funcs = 20;
    }

    // Allocate bit array
    filter->size_bytes = (filter->size_bits + 7) / 8;
    filter->bits = (uint8_t *)calloc(filter->size_bytes, sizeof(uint8_t));
    if (filter->bits == NULL) {
        perror("Failed to allocate bit array");
        free(filter);
        return NULL;
    }

    filter->items_added = 0;

    return filter;
}

void bloom_add(BloomFilter *filter, const void *data, size_t len) {
    if (filter == NULL || data == NULL || len == 0) {
        return;
    }

    size_t *indices = (size_t *)malloc(filter->num_hash_funcs * sizeof(size_t));
    if (indices == NULL) {
        perror("Failed to allocate hash indices");
        return;
    }

    get_hash_indices(data, len, filter->num_hash_funcs, filter->size_bits, indices);

    for (size_t i = 0; i < filter->num_hash_funcs; i++) {
        set_bit(filter->bits, indices[i]);
    }

    filter->items_added++;
    free(indices);
}

void bloom_add_string(BloomFilter *filter, const char *str) {
    if (str != NULL) {
        bloom_add(filter, str, strlen(str));
    }
}

bool bloom_contains(BloomFilter *filter, const void *data, size_t len) {
    if (filter == NULL || data == NULL || len == 0) {
        return false;
    }

    size_t *indices = (size_t *)malloc(filter->num_hash_funcs * sizeof(size_t));
    if (indices == NULL) {
        perror("Failed to allocate hash indices");
        return false;
    }

    get_hash_indices(data, len, filter->num_hash_funcs, filter->size_bits, indices);

    bool result = true;
    for (size_t i = 0; i < filter->num_hash_funcs; i++) {
        if (!get_bit(filter->bits, indices[i])) {
            result = false;
            break;
        }
    }

    free(indices);
    return result;
}

bool bloom_contains_string(BloomFilter *filter, const char *str) {
    if (str == NULL) {
        return false;
    }
    return bloom_contains(filter, str, strlen(str));
}

double bloom_false_positive_rate(BloomFilter *filter) {
    if (filter == NULL || filter->size_bits == 0) {
        return 1.0;
    }

    // p = (1 - e^(-k*n/m))^k
    double exponent = -((double)filter->num_hash_funcs * (double)filter->items_added)
                      / (double)filter->size_bits;
    double base = 1.0 - exp(exponent);
    return pow(base, (double)filter->num_hash_funcs);
}

void bloom_clear(BloomFilter *filter) {
    if (filter != NULL && filter->bits != NULL) {
        memset(filter->bits, 0, filter->size_bytes);
        filter->items_added = 0;
    }
}

void bloom_destroy(BloomFilter *filter) {
    if (filter != NULL) {
        free(filter->bits);
        free(filter);
    }
}

void bloom_print_stats(BloomFilter *filter) {
    if (filter == NULL) {
        printf("Bloom filter: NULL\n");
        return;
    }

    printf("=== Bloom Filter Statistics ===\n");
    printf("Size: %zu bits (%.2f KB)\n", filter->size_bits,
           (double)filter->size_bytes / 1024.0);
    printf("Hash functions: %zu\n", filter->num_hash_funcs);
    printf("Items added: %zu\n", filter->items_added);
    printf("Bits per item: %.2f\n",
           filter->items_added > 0 ? (double)filter->size_bits / filter->items_added : 0.0);
    printf("Current false positive rate: %.6f%%\n",
           bloom_false_positive_rate(filter) * 100.0);
    printf("===============================\n");
}
