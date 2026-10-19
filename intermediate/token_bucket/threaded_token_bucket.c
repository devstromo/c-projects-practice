#define _POSIX_C_SOURCE 200809L

#include "threaded_token_bucket.h"

#include <errno.h>
#include <stddef.h>
#include <time.h>

static int monotonic_clock(void *context, double *seconds) {
    (void)context;
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        return errno;
    }
    *seconds = (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
    return 0;
}

int threaded_token_bucket_init(ThreadedTokenBucket *bucket, double capacity,
                               double refill_rate) {
    return threaded_token_bucket_init_with_clock(bucket, capacity, refill_rate,
                                                 monotonic_clock, NULL);
}

int threaded_token_bucket_init_with_clock(ThreadedTokenBucket *bucket,
                                          double capacity, double refill_rate,
                                          TokenBucketClock clock, void *context) {
    if (bucket == NULL || clock == NULL) {
        return EINVAL;
    }

    double now;
    int error = clock(context, &now);
    if (error != 0) {
        return error;
    }
    TokenBucket initial;
    if (!token_bucket_init(&initial, capacity, refill_rate, now)) {
        return EINVAL;
    }

    error = pthread_mutex_init(&bucket->mutex, NULL);
    if (error != 0) {
        return error;
    }
    bucket->bucket = initial;
    bucket->clock = clock;
    bucket->clock_context = context;
    return 0;
}

int threaded_token_bucket_try_consume(ThreadedTokenBucket *bucket, double cost,
                                      TokenBucketResult *result) {
    if (bucket == NULL || result == NULL) {
        return EINVAL;
    }

    int error = pthread_mutex_lock(&bucket->mutex);
    if (error != 0) {
        return error;
    }

    double now;
    error = bucket->clock(bucket->clock_context, &now);
    TokenBucketResult decision = TOKEN_BUCKET_INVALID;
    if (error == 0) {
        decision = token_bucket_try_consume(&bucket->bucket, cost, now);
    }
    /* Unlock even when the clock fails, so later callers can still proceed. */
    int unlock_error = pthread_mutex_unlock(&bucket->mutex);
    if (unlock_error != 0) {
        return unlock_error;
    }
    if (error != 0) {
        return error;
    }
    *result = decision;
    return 0;
}

int threaded_token_bucket_destroy(ThreadedTokenBucket *bucket) {
    if (bucket == NULL) {
        return EINVAL;
    }
    return pthread_mutex_destroy(&bucket->mutex);
}
