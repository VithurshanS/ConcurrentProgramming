//
// Created by vithurshan on 10/7/26.
//

#include "critical_section_problem.h"
#include <pthread.h>
#include <stdio.h>
#include <threads.h>
#include <stdint.h>
#define THREAD_COUNT 10

long shared_counter = 0;
//expected = 10000000

//mutex usage
pthread_mutex_t mutex_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;


void* increment(void* arg) {
    int tid = (int)(intptr_t)arg;
    pthread_mutex_lock(&mutex_a);
    printf("INCREMENT thread id : %d ACQUIRED the lock for mutex_a\n", tid);
    for (int j=0;j<1'000'000;j++) {

        long temp = shared_counter;
        shared_counter = temp + 1;

    }
    pthread_mutex_unlock(&mutex_a);
    printf("INCREMENT thread id : %d RELEASED the lock for mutex_a\n", tid);
    printf("INCREMENT thread id : %d Counter: %ld\n",tid, shared_counter);
}
// for dead lock
void* increment_d(void* arg) {
    int tid = (int)(intptr_t)arg;
    pthread_mutex_lock(&mutex_a);
    printf("INCREMENT thread id : %d ACQUIRED the lock for mutex_a\n", tid);
    pthread_mutex_lock(&mutex_b);
    printf("INCREMENT thread id : %d ACQUIRED the lock for mutex_b\n", tid);
    for (int j=0;j<1'000'000;j++) {

        long temp = shared_counter;
        shared_counter = temp + 1;

    }
    pthread_mutex_unlock(&mutex_b);
    printf("INCREMENT thread id : %d RELEASED the lock for mutex_b\n", tid);
    pthread_mutex_unlock(&mutex_a);
    printf("INCREMENT thread id : %d RELEASED the lock for mutex_a\n", tid);
    printf("INCREMENT thread id : %d Counter: %ld\n",tid, shared_counter);
}


void* decrement_d(void* arg) {
    int tid = (int)(intptr_t)arg;
    pthread_mutex_lock(&mutex_b);
    printf("DECREMENT thread id : %d ACQUIRED the lock for mutex_b\n", tid);
    pthread_mutex_lock(&mutex_a);
    printf("DECREMENT thread id : %d ACQUIRED the lock for mutex_a\n", tid);
    for (int j=0;j<1'000'000;j++) {

        long temp = shared_counter;
        shared_counter = temp - 1;

    }
    pthread_mutex_unlock(&mutex_a);
    printf("DECREMENT thread id : %d RELEASED the lock for mutex_a\n", tid);
    pthread_mutex_unlock(&mutex_b);
    printf("DECREMENT thread id : %d RELEASED the lock for mutex_b\n", tid);
    printf("DECREMENT thread id : %d Counter: %ld\n",tid, shared_counter);
}

//increment 2 for check starvation
void* increment2(void* arg) {
    int tid = (int)(intptr_t)arg;
    for (int k = 0;k<20;k++) {
        pthread_mutex_lock(&mutex_a);
        printf("thread_id %d ACQUIRED the lock for %d th time\n",tid,k);
        for (int j=0;j<1'000'000;j++) {

            long temp = shared_counter;
            shared_counter = temp + 1;

        }
        pthread_mutex_unlock(&mutex_a);
        printf("thread_id %d RELEASED the lock for %d th time\n",tid,k);

    }
    printf("thread id : %d Counter: %ld\n",tid, shared_counter);

}

void thread_deploy() {
    pthread_t threads[THREAD_COUNT];
    pthread_t dec_threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, increment_d, (void*)(intptr_t)i);
        pthread_create(&dec_threads[i], NULL, decrement_d, (void*)(intptr_t)i);
    }
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
        pthread_join(dec_threads[i], NULL);
    }
}



