//
// Created by vithurshan on 10/7/26.
//

#include "bounded_buffer_problem.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

#define N_PRODUCERS 20
#define N_CONSUMERS 20
#define BUFFER_SIZE 5
#define MAX_ITEMS 100000
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define WAIT_FOR 1 //1 seconds
#define RUN_FOR 100



pthread_mutex_t consumer_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t producer_lock = PTHREAD_MUTEX_INITIALIZER;
int item_counter = 0;
int buffer[BUFFER_SIZE]={0};
int consumed_items[MAX_ITEMS] = {0};
int head = 0;
int tail = 0;

//base
void produce(void *arg) {
    // Implementation for producing items
    struct timespec ts;
    ts.tv_sec = WAIT_FOR;
    ts.tv_nsec = 0;
    int tid = (int)(intptr_t)arg;
    while (true) {
        pthread_mutex_lock(&producer_lock);
        int item = ++item_counter;
        if (buffer[tail] != 0) {
            printf("Producer %d: Buffer is full, value over written \n", tid);
        }
        buffer[tail] = item;
        tail = (tail + 1) % BUFFER_SIZE;
        printf("Producer %d Produced item: %d\n",tid, item);
        pthread_mutex_unlock(&producer_lock);
        nanosleep(&ts, nullptr);
    }

}
void consume(void *arg) {
    struct timespec ts;
    ts.tv_sec = WAIT_FOR;
    ts.tv_nsec = 0;

    while (true) {
        pthread_mutex_lock(&consumer_lock);
        int item = buffer[head];
        buffer[head] = 0;
        head = (head + 1) % BUFFER_SIZE;
        printf("Consumer %d Consumed item: %d\n", (int)(intptr_t)arg, item);
        pthread_mutex_unlock(&consumer_lock);
        if (item>0&& item<MAX_ITEMS) {
            consumed_items[item]++;
            if (consumed_items[item]> 1) {
                printf(" BUG! Item %d consumed %d TIMES!\n", item, consumed_items[item]);
            }
        }else if (item == 0) {
            printf(" BUG! Consumer %d consumed an EMPTY slot (0)!\n", (int)(intptr_t)arg);
        }

        nanosleep(&ts, nullptr);
    }
    // Implementation for consuming items
}

// avoid over written and consumption of null items
void produce_v1(void *arg) {
    // Implementation for producing items
    struct timespec ts;
    ts.tv_sec = WAIT_FOR;
    ts.tv_nsec = 0;
    int tid = (int)(intptr_t)arg;
    int cont_wait = 0;
    int run_for = RUN_FOR;
    while (run_for>0) {
        pthread_mutex_lock(&producer_lock);

        if (buffer[tail] != 0) {
            printf("Producer %d: Buffer is full, waiting for consumption as %d th time \n", tid,++cont_wait);
            pthread_mutex_unlock(&producer_lock);
            nanosleep(&ts, nullptr);
            continue;
        }
        run_for--;
        cont_wait = 0;
        int item = ++item_counter;
        buffer[tail] = item;
        tail = (tail + 1) % BUFFER_SIZE;
        printf("Producer %d Produced item: %d\n",tid, item);
        pthread_mutex_unlock(&producer_lock);
        nanosleep(&ts, nullptr);
    }

}
void consume_v1(void *arg) {
    int tid = (int)(intptr_t)arg;
    struct timespec ts;
    ts.tv_sec = WAIT_FOR;
    ts.tv_nsec = 0;
    int cont_wait = 0;
    int run_for = RUN_FOR;
    while (run_for>0) {
        pthread_mutex_lock(&consumer_lock);
        int item = buffer[head];
        if (item == 0) {
            printf("Consumer %d: Buffer slot is empty, waiting for production %d th time \n", tid,++cont_wait);
            pthread_mutex_unlock(&consumer_lock);
            nanosleep(&ts, nullptr);
            continue;
        }
        run_for--;
        cont_wait=0;
        buffer[head] = 0;
        head = (head + 1) % BUFFER_SIZE;
        printf("Consumer %d Consumed item: %d\n", (int)(intptr_t)arg, item);
        pthread_mutex_unlock(&consumer_lock);
        if (item>0&& item<MAX_ITEMS) {
            consumed_items[item]++;
            if (consumed_items[item]> 1) {
                printf(" BUG! Item %d consumed %d TIMES!\n", item, consumed_items[item]);
            }
        }

        nanosleep(&ts, nullptr);
    }
    // Implementation for consuming items
}




void produce_consume() {
    // Implementation for producing and consuming items
    pthread_t producers[N_PRODUCERS];
    pthread_t consumers[N_CONSUMERS];
    int lim = MAX(N_PRODUCERS, N_CONSUMERS);
    for (int i=0;i<lim;i++) {
        if (i < N_PRODUCERS) {
            pthread_create(&producers[i], nullptr, (void *)produce_v1, (void*)(intptr_t)i);
        }
        if (i < N_CONSUMERS) {
            pthread_create(&consumers[i], nullptr, (void *)consume_v1, (void*)(intptr_t)i);
        }
    }
    for (int i=0;i<lim;i++) {
        if (i < N_PRODUCERS) {
            pthread_join(producers[i], nullptr);
        }
        if (i < N_CONSUMERS) {
            pthread_join(consumers[i], nullptr);
        }
    }
    printf("final counter number is (total produced elements  %d",item_counter);
    int non_zero_element_count = 0;
    for (int i=0;i<BUFFER_SIZE;i++) {
        if (buffer[i] != 0) {
            non_zero_element_count++;
        }
    }
    printf(", non-zero elements: %d)\n", non_zero_element_count);
}