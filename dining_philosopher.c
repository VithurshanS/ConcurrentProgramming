//
// Created by vithurshan on 10/7/26.
//

#include "dining_philosopher.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define CHANCE 100 //each philosopher will get this many chances so expectation is they should eat at this times
#define MS 100000

//resources are locks and philophers are threads

//livelockness (philosopherAction_livelock)
//for CHANCE 100 MS 100000 THINKING TIME (rand() % 200 + 100) * MS
//Philosopher 0 ate 70 times
// Philosopher 1 ate 71 times
// Philosopher 2 ate 76 times
// Philosopher 3 ate 72 times
// Philosopher 4 ate 65 times
//for CHANCE 100 MS 1000 THINKING TIME (rand() % 200 + 100) * MS
// Philosopher 0 ate 69 times
// Philosopher 1 ate 66 times
// Philosopher 2 ate 62 times
// Philosopher 3 ate 59 times
// Philosopher 4 ate 68 times
//for CHANCE 100 MS 100000 THINKING TIME (rand() % 20 + 10) * MS
// Philosopher 0 ate 32 times
// Philosopher 1 ate 23 times
// Philosopher 2 ate 20 times
// Philosopher 3 ate 26 times
// Philosopher 4 ate 28 times
//for CHANCE 100 MS 100000 THINKING TIME (rand() % 2 + 1) * MS
// Philosopher 0 ate 11 times
// Philosopher 1 ate 16 times
// Philosopher 2 ate 9 times
// Philosopher 3 ate 12 times
// Philosopher 4 ate 16 times
//for CHANCE 100 MS 100000 THINKING TIME (rand() % 2 + 1) * (int)(MS/10)
// Philosopher 0 ate 5 times
// Philosopher 1 ate 0 times
// Philosopher 2 ate 1 times
// Philosopher 3 ate 3 times
// Philosopher 4 ate 2 times
//for CHANCE 100 MS 100000 THINKING TIME (rand() % 2 + 1) * (int)(MS/100)
// Philosopher 0 ate 0 times
// Philosopher 1 ate 1 times
// Philosopher 2 ate 2 times
// Philosopher 3 ate 3 times
// Philosopher 4 ate 0 times

//deadlockness (philosopherAction)
//thinking time (rand() % 200 + 100) * MS CHANCE 100;
//deadlock imediately
//thinking time (rand() % 20 + 10) * MS CHANCE 100;
//deadlock imediately
//thinking time (rand() % 2000 + 1000) * MS CHANCE 100;
//deadlock after some time
//thinking time (rand() % 20000 + 10000) * MS CHANCE 100;
//deadlock imediatly
//thinking time (rand() % 200 + 100) * MS*10000 CHANCE 100;
// Philosopher 0 ate 100 times
// Philosopher 1 ate 100 times
// Philosopher 2 ate 100 times
// Philosopher 3 ate 100 times
// Philosopher 4 ate 100 times




pthread_mutex_t forks[5];

int eatcount[5] = {0};

void* philosopherAction(void* arg) {
    int pid = (int)arg;
    int chance =CHANCE;
    while (chance-->0) {
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = (rand() % 200 + 100) * MS*10000;
        printf("Philosopher %d is thinking\n", pid);
        nanosleep(&ts,NULL);
        //acquire left fork
        int left_fork_index = pid;
        pthread_mutex_lock(&forks[left_fork_index]);
        printf("Philosopher %d acquired left fork\n", pid);
        // ts.tv_nsec= 500 * 1000000L;
        // ts.tv_nsec = (rand() % 100 + 100) * 100000L; eating time > deadlock time -> didnt get the deadlock
        ts.tv_nsec = (rand() % 100 + 100) * MS; //got the deadlock imediately
        nanosleep(&ts,NULL);

        //acquire right fork
        int right_fork_index = (pid+1)%5;
        pthread_mutex_lock(&forks[right_fork_index]);
        printf("Philosopher %d acquired right fork\n", pid);
        struct timespec ts1;
        ts1.tv_sec = 0;
        ts1.tv_nsec = (rand() % 100 + 100) * MS;
        //eat
        printf("Philosopher %d is eating\n", pid);
        eatcount[pid]++;
        nanosleep(&ts1,NULL);
        //release forks
        pthread_mutex_unlock(&forks[left_fork_index]);
        printf("Philosopher %d released left fork\n", pid);
        nanosleep(&ts,NULL);
        pthread_mutex_unlock(&forks[right_fork_index]);
        printf("Philosopher %d released right fork\n", pid);

    }
}

void* philosopherAction_livelock(void* arg) {
    int pid = (int)arg;
    int chance = CHANCE;
    while (chance-->0) {
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = (rand() % 200 + 100) * MS;
        printf("Philosopher %d is thinking\n", pid);
        nanosleep(&ts,NULL);
        //acquire left fork
        int left_fork_index = pid;
        pthread_mutex_lock(&forks[left_fork_index]);
        printf("Philosopher %d acquired left fork\n", pid);
        // ts.tv_nsec= 500 * 1000000L;
        // ts.tv_nsec = (rand() % 100 + 100) * 100000L; eating time > deadlock time -> didnt get the deadlock
        ts.tv_nsec = (rand() % 100 + 100) * MS; //got the deadlock imediately
        //deadwait
        nanosleep(&ts,NULL);

        //acquire right fork
        int right_fork_index = (pid+1)%5;
        int is_acquired = pthread_mutex_trylock(&forks[right_fork_index]);
        if (is_acquired != 0) {
            printf("Philosopher %d could not acquire right fork\n", pid);
            pthread_mutex_unlock(&forks[left_fork_index]);
            printf("Philosopher %d released left fork\n", pid);
            continue;
        }
        printf("Philosopher %d acquired right fork\n", pid);
        struct timespec ts1;
        ts1.tv_sec = 0;
        ts1.tv_nsec = (rand() % 100 + 100) * MS;
        //eat
        printf("Philosopher %d is eating\n", pid);
        eatcount[pid]++;
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
    for (int i=0;i<5;i++) {
        printf("Philosopher %d ate %d times\n", i, eatcount[i]);
    }
}
