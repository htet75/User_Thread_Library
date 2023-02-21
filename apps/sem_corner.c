#include <stdio.h>
#include <stdlib.h>
#include "sem.h"
#include <uthread.h>

sem_t sem;

void threadC() {
    sem_down(sem);
    printf("Thread C got the semaphore!\n");

    /* Simulate some work that requires the semaphore */
    for (int i = 0; i < 5; i++) {
        printf("Thread C is doing some work...\n");
    }

    printf("Thread C is releasing the semaphore\n");
    sem_up(sem);
}

void threadB() {
    sem_up(sem);
    printf("Thread B released the semaphore\n");

    uthread_create(threadC, NULL);
}

void threadA() {
    sem_down(sem);
    printf("Thread A got the semaphore!\n");

    uthread_create(threadB, NULL);

    /* Simulate some work that requires the semaphore */
    for (int i = 0; i < 5; i++) {
        printf("Thread A is doing some work...\n");
    }

    printf("Thread A is releasing the semaphore\n");
    sem_up(sem);
}

int main() {
    sem = sem_create(0);

    uthread_run(false, threadA, NULL);

    sem_destroy(sem);
    return 0;
}
