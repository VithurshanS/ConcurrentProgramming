//
// Created by vithurshan on 10/7/26.
//

#include "dining_philosopher.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//resources are locks and philophers are threads

pthread_mutex_t forks[5];

void* philosopherAction(void* arg) {
    int pid = (int)arg;
    while (true) {
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = (rand() % 200 + 100) * 1000000L;
        printf("Philosopher %d is thinking\n", pid);
        nanosleep(&ts,NULL);
        //acquire left fork
        int left_fork_index = pid;
        pthread_mutex_lock(&forks[left_fork_index]);
        printf("Philosopher %d acquired left fork\n", pid);
        // ts.tv_nsec= 500 * 1000000L;
        // ts.tv_nsec = (rand() % 100 + 100) * 100000L; eating time > deadlock time -> didnt get the deadlock
        ts.tv_nsec = (rand() % 100 + 100) * 1000000L; //got the deadlock imediately
        nanosleep(&ts,NULL);

        //acquire right fork
        int right_fork_index = (pid+1)%5;
        pthread_mutex_lock(&forks[right_fork_index]);
        printf("Philosopher %d acquired right fork\n", pid);
        struct timespec ts1;
        ts1.tv_sec = 0;
        ts1.tv_nsec = (rand() % 100 + 100) * 1000000L;
        //eat
        printf("Philosopher %d is eating\n", pid);
        nanosleep(&ts1,NULL);
        //release forks
        pthread_mutex_unlock(&forks[left_fork_index]);
        printf("Philosopher %d released left fork\n", pid);
        nanosleep(&ts,NULL);
        pthread_mutex_unlock(&forks[right_fork_index]);
        printf("Philosopher %d released right fork\n", pid);

    }
}

void diningPhilosopher()
{
    pthread_t philosopher[5];
    for (int i = 0; i < 5; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }
    for (int i = 0; i < 5; i++) {
        pthread_create(&philosopher[i], NULL, philosopherAction, (void*)i);
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(philosopher[i], NULL);
    }
    for (int i = 0; i < 5; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
}
