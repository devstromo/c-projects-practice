#define _POSIX_C_SOURCE 200809L

#include "threaded_token_bucket.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { WORKER_COUNT = 4, REQUESTS_PER_WORKER = 6 };

typedef struct {
    ThreadedTokenBucket *bucket;
    unsigned int allowed;
    unsigned int denied;
    int error;
} Worker;

static void *run_worker(void *argument) {
    Worker *worker = argument;
    for (int i = 0; i < REQUESTS_PER_WORKER; i++) {
        TokenBucketResult result;
        worker->error = threaded_token_bucket_try_consume(worker->bucket, 1.0, &result);
        if (worker->error != 0) {
            break;
        }
        if (result == TOKEN_BUCKET_ALLOWED) {
            /* Perform real work here, after the bucket mutex has been released. */
            worker->allowed++;
        } else if (result == TOKEN_BUCKET_DENIED) {
            worker->denied++;
        } else {
            worker->error = EINVAL;
            break;
        }

        if (i + 1 < REQUESTS_PER_WORKER) {
            struct timespec remaining = {0, 250000000L};
            while (nanosleep(&remaining, &remaining) != 0) {
                if (errno != EINTR) {
                    worker->error = errno;
                    return NULL;
                }
            }
        }
    }
    return NULL;
}

int main(void) {
    ThreadedTokenBucket bucket;
    int error = threaded_token_bucket_init(&bucket, 3.0, 2.0);
    if (error != 0) {
        fprintf(stderr, "Bucket initialization: %s\n", strerror(error));
        return EXIT_FAILURE;
    }

    pthread_t threads[WORKER_COUNT];
    Worker workers[WORKER_COUNT] = {0};
    size_t started = 0;
    int status = EXIT_SUCCESS;
    puts("4 workers sharing one bucket: capacity = 3, refill = 2 tokens/second");
    puts("Each worker attempts 6 requests costing 1 token, about 250 ms apart.");
    for (size_t i = 0; i < WORKER_COUNT; i++) {
        workers[i].bucket = &bucket;
        error = pthread_create(&threads[i], NULL, run_worker, &workers[i]);
        if (error != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(error));
            status = EXIT_FAILURE;
            break;
        }
        started++;
    }

    unsigned int allowed = 0;
    unsigned int denied = 0;
    for (size_t i = 0; i < started; i++) {
        error = pthread_join(threads[i], NULL);
        if (error != 0) {
            /* A worker may still use the bucket; terminate without destroying it. */
            fprintf(stderr, "pthread_join: %s\n", strerror(error));
            exit(EXIT_FAILURE);
        }
        if (workers[i].error != 0) {
            fprintf(stderr, "Worker %zu: %s\n", i + 1, strerror(workers[i].error));
            status = EXIT_FAILURE;
        }
        printf("Worker %zu: %u allowed, %u denied\n", i + 1,
               workers[i].allowed, workers[i].denied);
        allowed += workers[i].allowed;
        denied += workers[i].denied;
    }
    printf("Total: %u allowed, %u denied (scheduling affects the distribution).\n",
           allowed, denied);

    error = threaded_token_bucket_destroy(&bucket);
    if (error != 0) {
        fprintf(stderr, "Bucket destruction: %s\n", strerror(error));
        status = EXIT_FAILURE;
    }
    return status;
}
