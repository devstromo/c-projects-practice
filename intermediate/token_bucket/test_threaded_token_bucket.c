#include "threaded_token_bucket.h"

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

enum { THREAD_COUNT = 8, ATTEMPTS_PER_THREAD = 128 };

typedef struct {
    double now;
    int error;
    size_t calls;
} FakeClock;

static int fake_clock(void *context, double *seconds) {
    FakeClock *clock = context;
    clock->calls++;
    if (clock->error != 0) {
        return clock->error;
    }
    *seconds = clock->now;
    return 0;
}

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    size_t ready;
    bool go;
} StartGate;

typedef struct {
    ThreadedTokenBucket *bucket;
    StartGate *gate;
    double cost;
    size_t allowed;
    size_t denied;
} TestWorker;

static void *consume_requests(void *argument) {
    TestWorker *worker = argument;
    StartGate *gate = worker->gate;
    assert(pthread_mutex_lock(&gate->mutex) == 0);
    gate->ready++;
    assert(pthread_cond_broadcast(&gate->condition) == 0);
    while (!gate->go) {
        assert(pthread_cond_wait(&gate->condition, &gate->mutex) == 0);
    }
    assert(pthread_mutex_unlock(&gate->mutex) == 0);

    for (int i = 0; i < ATTEMPTS_PER_THREAD; i++) {
        TokenBucketResult result;
        assert(threaded_token_bucket_try_consume(worker->bucket, worker->cost, &result) == 0);
        assert(result == TOKEN_BUCKET_ALLOWED || result == TOKEN_BUCKET_DENIED);
        if (result == TOKEN_BUCKET_ALLOWED) {
            worker->allowed++;
        } else {
            worker->denied++;
        }
    }
    return NULL;
}

/* Release all workers together. No worker or decision depends on a real sleep. */
static void run_contention(ThreadedTokenBucket *bucket, double cost, size_t expected) {
    StartGate gate = {0};
    assert(pthread_mutex_init(&gate.mutex, NULL) == 0);
    assert(pthread_cond_init(&gate.condition, NULL) == 0);
    pthread_t threads[THREAD_COUNT];
    TestWorker workers[THREAD_COUNT] = {0};
    for (size_t i = 0; i < THREAD_COUNT; i++) {
        workers[i].bucket = bucket;
        workers[i].gate = &gate;
        workers[i].cost = cost;
        assert(pthread_create(&threads[i], NULL, consume_requests, &workers[i]) == 0);
    }

    assert(pthread_mutex_lock(&gate.mutex) == 0);
    while (gate.ready < THREAD_COUNT) {
        assert(pthread_cond_wait(&gate.condition, &gate.mutex) == 0);
    }
    gate.go = true;
    assert(pthread_cond_broadcast(&gate.condition) == 0);
    assert(pthread_mutex_unlock(&gate.mutex) == 0);

    size_t allowed = 0;
    size_t denied = 0;
    for (size_t i = 0; i < THREAD_COUNT; i++) {
        assert(pthread_join(threads[i], NULL) == 0);
        allowed += workers[i].allowed;
        denied += workers[i].denied;
    }
    assert(allowed == expected);
    assert(allowed + denied == THREAD_COUNT * ATTEMPTS_PER_THREAD);
    assert(pthread_cond_destroy(&gate.condition) == 0);
    assert(pthread_mutex_destroy(&gate.mutex) == 0);
}

static void test_shared_burst(void) {
    const struct {
        double capacity;
        double cost;
        size_t expected;
    } cases[] = {{1.0, 1.0, 1}, {31.0, 1.0, 31}, {10.0, 2.0, 5}};
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        ThreadedTokenBucket bucket;
        FakeClock clock = {0};
        assert(threaded_token_bucket_init_with_clock(&bucket, cases[i].capacity, 2.0,
                                                     fake_clock, &clock) == 0);
        run_contention(&bucket, cases[i].cost, cases[i].expected);
        assert(clock.calls == 1 + THREAD_COUNT * ATTEMPTS_PER_THREAD);
        assert(threaded_token_bucket_destroy(&bucket) == 0);
    }
}

static void test_shared_refill(void) {
    ThreadedTokenBucket bucket;
    FakeClock clock = {0};
    assert(threaded_token_bucket_init_with_clock(&bucket, 3.0, 2.0, fake_clock, &clock) == 0);
    run_contention(&bucket, 1.0, 3);
    /* Change fake time only after all previous workers have been joined. */
    clock.now = 0.25;
    run_contention(&bucket, 1.0, 0);
    clock.now = 0.5;
    run_contention(&bucket, 1.0, 1);
    clock.now = 100.0;
    run_contention(&bucket, 1.0, 3);
    assert(threaded_token_bucket_destroy(&bucket) == 0);
}

static void test_validation_and_clock_errors(void) {
    ThreadedTokenBucket bucket;
    FakeClock clock = {10.0, 0, 0};
    assert(threaded_token_bucket_init(NULL, 3.0, 2.0) == EINVAL);
    assert(threaded_token_bucket_init_with_clock(&bucket, 3.0, 2.0, NULL, NULL) == EINVAL);
    const double invalid[] = {0.0, -1.0, NAN, INFINITY};
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); i++) {
        assert(threaded_token_bucket_init_with_clock(&bucket, invalid[i], 2.0,
                                                     fake_clock, &clock) == EINVAL);
        assert(threaded_token_bucket_init_with_clock(&bucket, 3.0, invalid[i],
                                                     fake_clock, &clock) == EINVAL);
    }
    clock.error = EIO;
    assert(threaded_token_bucket_init_with_clock(&bucket, 3.0, 2.0, fake_clock, &clock) == EIO);
    clock.error = 0;
    clock.now = NAN;
    assert(threaded_token_bucket_init_with_clock(&bucket, 3.0, 2.0, fake_clock, &clock) == EINVAL);
    clock.now = 10.0;
    assert(threaded_token_bucket_init_with_clock(&bucket, 3.0, 2.0, fake_clock, &clock) == 0);

    TokenBucketResult result = TOKEN_BUCKET_DENIED;
    assert(threaded_token_bucket_try_consume(NULL, 1.0, &result) == EINVAL);
    assert(result == TOKEN_BUCKET_DENIED);
    assert(threaded_token_bucket_try_consume(&bucket, 1.0, NULL) == EINVAL);
    const double invalid_costs[] = {0.0, -1.0, 4.0, NAN, INFINITY};
    for (size_t i = 0; i < sizeof(invalid_costs) / sizeof(invalid_costs[0]); i++) {
        assert(threaded_token_bucket_try_consume(&bucket, invalid_costs[i], &result) == 0);
        assert(result == TOKEN_BUCKET_INVALID);
    }
    clock.error = EIO;
    result = TOKEN_BUCKET_DENIED;
    assert(threaded_token_bucket_try_consume(&bucket, 1.0, &result) == EIO);
    assert(result == TOKEN_BUCKET_DENIED);
    clock.error = 0;
    clock.now = 9.0;
    assert(threaded_token_bucket_try_consume(&bucket, 1.0, &result) == 0);
    assert(result == TOKEN_BUCKET_INVALID);
    clock.now = NAN;
    assert(threaded_token_bucket_try_consume(&bucket, 1.0, &result) == 0);
    assert(result == TOKEN_BUCKET_INVALID);
    clock.now = 10.0;
    assert(threaded_token_bucket_try_consume(&bucket, 3.0, &result) == 0);
    assert(result == TOKEN_BUCKET_ALLOWED);
    assert(threaded_token_bucket_try_consume(&bucket, 1.0, &result) == 0);
    assert(result == TOKEN_BUCKET_DENIED);
    assert(threaded_token_bucket_destroy(&bucket) == 0);
    assert(threaded_token_bucket_destroy(NULL) == EINVAL);
}

static void test_default_clock(void) {
    ThreadedTokenBucket bucket;
    assert(threaded_token_bucket_init(&bucket, 1.0, 2.0) == 0);
    TokenBucketResult result;
    assert(threaded_token_bucket_try_consume(&bucket, 1.0, &result) == 0);
    assert(result == TOKEN_BUCKET_ALLOWED);
    assert(threaded_token_bucket_destroy(&bucket) == 0);
}

int main(void) {
    test_shared_burst();
    test_shared_refill();
    test_validation_and_clock_errors();
    test_default_clock();
    puts("All threaded token bucket tests passed (4 groups, 8 workers per contention round).");
    return 0;
}
