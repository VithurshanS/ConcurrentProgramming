//
// Created by vithurshan on 10/7/26.
//

#include "reader_writer_problem.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#define WRITER_WAIT_FOR 1000000
#define NUM_READERS 40
#define NUM_WRITERS 4
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ITERATION 10

typedef struct {
    int account_A;
    int account_B;
} BankDB;

BankDB db = {500,500};

// data inconsistency problem - coonsistency target is total ammount of DB which is 1000
void* reader(void* arg) {
    // Read from the database
    int tid = (int)(intptr_t)arg;
    int iteration = ITERATION;
    while (iteration-- > 0) {
        int a = db.account_A;
        int b = db.account_B;
        if (a + b != 1000) {
            printf("Reader %d TORN READ BUG  detected: Account A = %d, Account B = %d\n",tid, a, b);
            continue;
        }
        printf("Reader %d: Account A = %d, Account B = %d\n",tid, a, b);
    }
}

void* writer(void* arg) {
    // Write to the database
    int tid = (int)(intptr_t)arg;
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 10*WRITER_WAIT_FOR;
    int iteration = ITERATION;
    while (iteration-- > 0) {
        // Simulate a write operation
        db.account_A += 100;
        nanosleep(&ts, NULL);
        db.account_B -= 100;
        printf("Writer %d: Account A = %d, Account B = %d\n", tid, db.account_A, db.account_B);
        db.account_B += 100;
        nanosleep(&ts, NULL);
        db.account_A -= 100;
        printf("Writer %d: Account A = %d, Account B = %d\n", tid, db.account_A, db.account_B);
    }
}

// mutex lock introduced to solve consistency issue write- read or write-write should not to be allowed so it destroyed the read parallelism
pthread_mutex_t writer_lock = PTHREAD_MUTEX_INITIALIZER;
void* reader_mutex_v1(void* arg) {
    // Read from the database
    int tid = (int)(intptr_t)arg;
    int iteration = ITERATION;
    while (iteration-- > 0) {
        pthread_mutex_lock(&writer_lock); //writer should not come when reader on its critical section but unfortunately it wiil apply to peer readers also
        int a = db.account_A;
        int b = db.account_B;
        pthread_mutex_unlock(&writer_lock);
        if (a + b != 1000) {
            printf("Reader %d TORN READ BUG  detected: Account A = %d, Account B = %d\n",tid, a, b);
            continue;
        }
        printf("Reader %d: Account A = %d, Account B = %d\n",tid, a, b);
    }
}

void* writer_mutex_v1(void* arg) {
    // Write to the database
    int tid = (int)(intptr_t)arg;
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 10*WRITER_WAIT_FOR;
    int iteration = ITERATION;
    while (iteration-- > 0) {
        // Simulate a write operation
        pthread_mutex_lock(&writer_lock);
        db.account_A += 100;
        nanosleep(&ts, NULL);
        db.account_B -= 100;
        printf("Writer %d: Account A = %d, Account B = %d\n", tid, db.account_A, db.account_B);
        pthread_mutex_unlock(&writer_lock); // we might expect that reader prints 600,400 but consistency maintainde
        pthread_mutex_lock(&writer_lock);
        db.account_B += 100;
        nanosleep(&ts, NULL);
        db.account_A -= 100;
        printf("Writer %d: Account A = %d, Account B = %d\n", tid, db.account_A, db.account_B);
        pthread_mutex_unlock(&writer_lock);
    }
}
// concurrent readers allowed - but introduces the writer starveness

pthread_mutex_t writer_lock_v1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reader_lock_v1 = PTHREAD_MUTEX_INITIALIZER;

int reader_count = 0;
void* reader_concurrency(void* arg) {
    // Read from the database
    int tid = (int)(intptr_t)arg;
    //int iteration = ITERATION;
    while (true) {
        pthread_mutex_lock(&reader_lock_v1);
        reader_count++;
        if (reader_count == 1) {
            pthread_mutex_lock(&writer_lock_v1);
        }
        pthread_mutex_unlock(&reader_lock_v1);
        printf("Reader count %d\n",reader_count);
        int a = db.account_A;
        int b = db.account_B;
        if (a + b != 1000) {
            printf("Reader %d TORN READ BUG  detected: Account A = %d, Account B = %d\n",tid, a, b);
            continue;
        }
        printf("Reader %d: Account A = %d, Account B = %d\n",tid, a, b);
        pthread_mutex_lock(&reader_lock_v1);
        reader_count--;
        if (reader_count == 0) {
            pthread_mutex_unlock(&writer_lock_v1);
        }
        pthread_mutex_unlock(&reader_lock_v1);
    }
}

void* writer_concurrency(void* arg) {
    // Write to the database
    int tid = (int)(intptr_t)arg;
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 10*WRITER_WAIT_FOR;
    //int iteration = ITERATION;
    while (true) {
        // Simulate a write operation
        pthread_mutex_lock(&writer_lock_v1);
        db.account_A += 100;
        nanosleep(&ts, NULL);
        db.account_B -= 100;
        printf("Writer %d: Account A = %d, Account B = %d\n", tid, db.account_A, db.account_B);
        pthread_mutex_unlock(&writer_lock_v1);
        nanosleep(&ts, NULL); // to ensure to watch writer starveness
        pthread_mutex_lock(&writer_lock_v1);
        db.account_B += 100;
        nanosleep(&ts, NULL);
        db.account_A -= 100;
        printf("Writer %d: Account A = %d, Account B = %d\n", tid, db.account_A, db.account_B);
        pthread_mutex_unlock(&writer_lock_v1);
        nanosleep(&ts, NULL); // to ensure to watch writer starveness

    }
}

void readerWriterProblem() {
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];
    for (int i=0;i<MAX(NUM_READERS, NUM_WRITERS);i++) {
        if (i < NUM_WRITERS) {
            pthread_create(&writers[i], NULL, writer_concurrency, (void*)(intptr_t)(i));
        }
        if (i < NUM_READERS) {
            pthread_create(&readers[i], NULL, reader_concurrency, (void*)(intptr_t)(i));
        }

    }
    for (int i=0;i<MAX(NUM_READERS, NUM_WRITERS);i++) {
        if (i < NUM_READERS) {
            pthread_join(readers[i], NULL);
        }
        if (i < NUM_WRITERS) {
            pthread_join(writers[i], NULL);
        }
    }
}