//
// Created by vithurshan on 10/7/26.
//

#include "racecond.h"
#include <pthread.h>
#include <stdio.h>
#define THREAD_COUNT 10

long shared_counter = 0;
//expected = 10000000


void* increment(void* arg) {
    int tid = (int)arg;
    for (int j=0;j<1000000;j++) {
        long temp = shared_counter;
        shared_counter = temp + 1;
    }
    printf("thread id : %d Counter: %ld\n",tid, shared_counter);
}

void thread_deploy() {
    pthread_t threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, increment, (void*)i);
    }
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
}



