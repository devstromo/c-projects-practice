#ifndef THREADED_TOKEN_BUCKET_H
#define THREADED_TOKEN_BUCKET_H

#include "token_bucket.h"

#include <pthread.h>

/* Return 0 and write seconds, or return a positive error code such as EIO. */
typedef int (*TokenBucketClock)(void *context, double *seconds);

/* Caller-owned. Do not copy, move, or directly access fields after initialization. */
typedef struct {
    TokenBucket bucket;
    pthread_mutex_t mutex;
    TokenBucketClock clock;
    void *clock_context;
} ThreadedTokenBucket;

/* Initialize before starting workers. The default clock is CLOCK_MONOTONIC. */
int threaded_token_bucket_init(ThreadedTokenBucket *bucket, double capacity,
                               double refill_rate);

/*
 * A custom clock supports deterministic tests. It runs during initialization and
 * under the bucket mutex during consumption; it must not reenter this bucket.
 * Keep its context alive until destruction and synchronize external changes.
 * A failed initialization requires no destruction. Do not initialize twice
 * without first successfully destroying the bucket.
 */
int threaded_token_bucket_init_with_clock(ThreadedTokenBucket *bucket,
                                          double capacity, double refill_rate,
                                          TokenBucketClock clock, void *context);

/*
 * Return 0 and write ALLOWED, DENIED, or INVALID to result; otherwise return an
 * error code and leave result unchanged. Read the clock after acquiring the lock.
 * Invalid costs and invalid timestamps follow the core API's no-change contract.
 * Initialize successfully before use; do not mix these calls with direct core
 * calls on the embedded bucket. Work must happen after this function returns.
 */
int threaded_token_bucket_try_consume(ThreadedTokenBucket *bucket, double cost,
                                      TokenBucketResult *result);

/* Join all workers first. Return 0 on success or a pthread/argument error code. */
int threaded_token_bucket_destroy(ThreadedTokenBucket *bucket);

#endif
