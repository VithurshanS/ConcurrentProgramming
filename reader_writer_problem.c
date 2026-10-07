//
// Created by vithurshan on 10/7/26.
//

#include "reader_writer_problem.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#define WRITER_WAIT_FOR 100
#define NUM_READERS 4
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
    int tid = (int)arg;
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
    int tid = (int)arg;
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


void readerWriterProblem() {
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];
    for (int i=0;i<MAX(NUM_READERS, NUM_WRITERS);i++) {
        if (i < NUM_WRITERS) {
            pthread_create(&writers[i], NULL, writer, (void*)i);
        }
        if (i < NUM_READERS) {
            pthread_create(&readers[i], NULL, reader, (void*)i);
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