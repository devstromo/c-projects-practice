#define _POSIX_C_SOURCE 200809L

#include "token_bucket.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static bool monotonic_seconds(double *seconds) {
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
        perror("clock_gettime");
        return false;
    }
    *seconds = (double)now.tv_sec + (double)now.tv_nsec / 1000000000.0;
    return true;
}

static bool pause_seconds(double seconds) {
    struct timespec remaining;
    remaining.tv_sec = (time_t)seconds;
    remaining.tv_nsec = (long)((seconds - (double)remaining.tv_sec) * 1000000000.0);
    while (nanosleep(&remaining, &remaining) != 0) {
        if (errno != EINTR) {
            perror("nanosleep");
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[]) {
    bool live = argc == 2 && strcmp(argv[1], "--live") == 0;
    if (argc > 1 && !live) {
        fprintf(stderr, "Usage: %s [--live]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const struct {
        double seconds;
        double cost;
    } requests[] = {
        {0.0, 1.0}, {0.0, 1.0}, {0.0, 1.0}, {0.0, 1.0},
        {0.25, 1.0}, {0.5, 1.0}, {1.0, 2.0}, {1.5, 2.0}, {3.0, 1.0}
    };

    double start = 0.0;
    if (live && !monotonic_seconds(&start)) {
        return EXIT_FAILURE;
    }
    TokenBucket bucket;
    if (!token_bucket_init(&bucket, 3.0, 2.0, start)) {
        fputs("Could not initialize token bucket.\n", stderr);
        return EXIT_FAILURE;
    }

    printf("Token bucket: capacity = 3, refill = 2 tokens/second (%s)\n",
           live ? "live clock" : "simulated time");
    puts("Seconds  Cost  Result   Tokens left");
    for (size_t i = 0; i < sizeof(requests) / sizeof(requests[0]); i++) {
        double now = requests[i].seconds;
        if (live) {
            if (i > 0 && !pause_seconds(requests[i].seconds - requests[i - 1].seconds)) {
                return EXIT_FAILURE;
            }
            if (!monotonic_seconds(&now)) {
                return EXIT_FAILURE;
            }
        }
        TokenBucketResult result = token_bucket_try_consume(&bucket, requests[i].cost, now);
        if (result == TOKEN_BUCKET_INVALID) {
            fputs("Invalid request or timestamp.\n", stderr);
            return EXIT_FAILURE;
        }
        printf("%7.2f  %4.1f  %-7s  %.2f\n", now - start, requests[i].cost,
               result == TOKEN_BUCKET_ALLOWED ? "allowed" : "denied", bucket.tokens);
    }
    return EXIT_SUCCESS;
}
