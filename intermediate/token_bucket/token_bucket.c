#include "token_bucket.h"

#include <math.h>
#include <stddef.h>

bool token_bucket_init(TokenBucket *bucket, double capacity,
                       double refill_rate, double now) {
    if (bucket == NULL || !isfinite(capacity) || capacity <= 0.0 ||
        !isfinite(refill_rate) || refill_rate <= 0.0 ||
        !isfinite(now) || now < 0.0) {
        return false;
    }

    bucket->capacity = capacity;
    bucket->refill_rate = refill_rate;
    bucket->tokens = capacity;
    bucket->last_update = now;
    return true;
}

TokenBucketResult token_bucket_try_consume(TokenBucket *bucket, double cost,
                                          double now) {
    if (bucket == NULL || !isfinite(cost) || cost <= 0.0 ||
        cost > bucket->capacity || !isfinite(now) ||
        now < bucket->last_update) {
        return TOKEN_BUCKET_INVALID;
    }

    double elapsed = now - bucket->last_update;
    double refill = elapsed * bucket->refill_rate;
    /* A very large elapsed time or rate can overflow refill; it fills the bucket. */
    if (!isfinite(refill) || refill >= bucket->capacity - bucket->tokens) {
        bucket->tokens = bucket->capacity;
    } else {
        bucket->tokens += refill;
    }
    bucket->last_update = now;

    if (bucket->tokens < cost) {
        return TOKEN_BUCKET_DENIED;
    }

    bucket->tokens -= cost;
    return TOKEN_BUCKET_ALLOWED;
}
