#include "token_bucket.h"

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

static void assert_unchanged(const TokenBucket *bucket, const TokenBucket *before) {
    assert(bucket->capacity == before->capacity);
    assert(bucket->refill_rate == before->refill_rate);
    assert(bucket->tokens == before->tokens);
    assert(bucket->last_update == before->last_update);
}

static void test_initial_burst(void) {
    TokenBucket bucket;
    assert(token_bucket_init(&bucket, 3.0, 2.0, 10.0));
    for (int i = 0; i < 3; i++) {
        assert(token_bucket_try_consume(&bucket, 1.0, 10.0) == TOKEN_BUCKET_ALLOWED);
    }
    assert(token_bucket_try_consume(&bucket, 1.0, 10.0) == TOKEN_BUCKET_DENIED);
    assert(bucket.tokens == 0.0);
}

static void test_fractional_refill_and_cost(void) {
    TokenBucket bucket;
    assert(token_bucket_init(&bucket, 2.0, 0.5, 0.0));
    assert(token_bucket_try_consume(&bucket, 2.0, 0.0) == TOKEN_BUCKET_ALLOWED);
    assert(token_bucket_try_consume(&bucket, 0.5, 0.5) == TOKEN_BUCKET_DENIED);
    assert(bucket.tokens == 0.25);
    assert(bucket.last_update == 0.5);
    assert(token_bucket_try_consume(&bucket, 0.5, 1.0) == TOKEN_BUCKET_ALLOWED);
    assert(bucket.tokens == 0.0);
    assert(token_bucket_try_consume(&bucket, 0.5, 1.0) == TOKEN_BUCKET_DENIED);
}

static void test_capacity_cap(void) {
    TokenBucket bucket;
    assert(token_bucket_init(&bucket, 3.0, 2.0, 0.0));
    assert(token_bucket_try_consume(&bucket, 3.0, 0.0) == TOKEN_BUCKET_ALLOWED);
    assert(token_bucket_try_consume(&bucket, 1.0, 100.0) == TOKEN_BUCKET_ALLOWED);
    assert(bucket.tokens == 2.0);
    assert(token_bucket_try_consume(&bucket, 3.0, 100.0) == TOKEN_BUCKET_DENIED);
    /* Time spent full is discarded; it cannot pay for later requests. */
    assert(token_bucket_try_consume(&bucket, 1.0, 200.0) == TOKEN_BUCKET_ALLOWED);
    assert(token_bucket_try_consume(&bucket, 2.0, 200.0) == TOKEN_BUCKET_ALLOWED);
    assert(token_bucket_try_consume(&bucket, 1.0, 200.0) == TOKEN_BUCKET_DENIED);
}

static void test_invalid_initialization(void) {
    TokenBucket bucket;
    assert(token_bucket_init(&bucket, 3.0, 2.0, 0.0));
    TokenBucket before = bucket;
    const double invalid[] = {0.0, -1.0, NAN, INFINITY, -INFINITY};
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); i++) {
        assert(!token_bucket_init(&bucket, invalid[i], 2.0, 0.0));
        assert_unchanged(&bucket, &before);
        assert(!token_bucket_init(&bucket, 3.0, invalid[i], 0.0));
        assert_unchanged(&bucket, &before);
    }
    const double invalid_times[] = {-1.0, NAN, INFINITY, -INFINITY};
    for (size_t i = 0; i < sizeof(invalid_times) / sizeof(invalid_times[0]); i++) {
        assert(!token_bucket_init(&bucket, 3.0, 2.0, invalid_times[i]));
        assert_unchanged(&bucket, &before);
    }
    assert(!token_bucket_init(NULL, 3.0, 2.0, 0.0));
}

static void test_invalid_requests(void) {
    TokenBucket bucket;
    assert(token_bucket_init(&bucket, 3.0, 2.0, 10.0));
    assert(token_bucket_try_consume(&bucket, 3.0, 10.0) == TOKEN_BUCKET_ALLOWED);
    TokenBucket before = bucket;
    const double invalid_costs[] = {0.0, -1.0, 4.0, NAN, INFINITY, -INFINITY};
    for (size_t i = 0; i < sizeof(invalid_costs) / sizeof(invalid_costs[0]); i++) {
        assert(token_bucket_try_consume(&bucket, invalid_costs[i], 20.0) == TOKEN_BUCKET_INVALID);
        assert_unchanged(&bucket, &before);
    }
    const double invalid_times[] = {9.0, -1.0, NAN, INFINITY, -INFINITY};
    for (size_t i = 0; i < sizeof(invalid_times) / sizeof(invalid_times[0]); i++) {
        assert(token_bucket_try_consume(&bucket, 1.0, invalid_times[i]) == TOKEN_BUCKET_INVALID);
        assert_unchanged(&bucket, &before);
    }
    assert(token_bucket_try_consume(NULL, 1.0, 10.0) == TOKEN_BUCKET_INVALID);
    assert(token_bucket_try_consume(&bucket, 1.0, 10.5) == TOKEN_BUCKET_ALLOWED);
    assert(bucket.tokens == 0.0);
}

static void test_large_refill(void) {
    TokenBucket bucket;
    assert(token_bucket_init(&bucket, 3.0, DBL_MAX, 0.0));
    assert(token_bucket_try_consume(&bucket, 3.0, 0.0) == TOKEN_BUCKET_ALLOWED);
    assert(token_bucket_try_consume(&bucket, 1.0, DBL_MAX) == TOKEN_BUCKET_ALLOWED);
    assert(bucket.tokens == 2.0);
    assert(isfinite(bucket.tokens));
}

int main(void) {
    test_initial_burst();
    test_fractional_refill_and_cost();
    test_capacity_cap();
    test_invalid_initialization();
    test_invalid_requests();
    test_large_refill();
    puts("All token bucket tests passed (6 groups).");
    return 0;
}
