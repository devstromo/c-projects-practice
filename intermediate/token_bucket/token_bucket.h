#ifndef TOKEN_BUCKET_H
#define TOKEN_BUCKET_H

#include <stdbool.h>

/* Caller-owned state; initialize it before use and do not edit its fields. */
typedef struct {
    double capacity;
    double refill_rate;
    double tokens;
    double last_update;
} TokenBucket;

typedef enum {
    TOKEN_BUCKET_ALLOWED,
    TOKEN_BUCKET_DENIED,
    TOKEN_BUCKET_INVALID
} TokenBucketResult;

/*
 * Start full. Capacity and refill_rate (tokens/second) must be finite and > 0.
 * now is a finite, nonnegative timestamp in seconds. Failure leaves state alone.
 * No memory allocation is performed; the caller owns the TokenBucket object.
 */
bool token_bucket_init(TokenBucket *bucket, double capacity,
                       double refill_rate, double now);

/*
 * Refill, then try to spend cost tokens. Use the same clock as initialization;
 * now must be finite and >= last_update. Cost must be finite and in (0, capacity].
 * DENIED still applies the refill, but spends no tokens. INVALID changes nothing.
 * Calls sharing a bucket require external synchronization when used in threads.
 */
TokenBucketResult token_bucket_try_consume(TokenBucket *bucket, double cost,
                                          double now);

#endif
