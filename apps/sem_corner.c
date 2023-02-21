#include <stdio.h>
#include <stdlib.h>
#include "sem.h"
#include <uthread.h>

sem_t sem;

void threadC() {
    printf("Thread C grabbed the semaphore!\n");
    sem_down(sem);

    /* Simulate some work that requires the semaphore */
    for (int i = 0; i < 5; i++) {
        printf("Thread C is doing some work...\n");
    }

    printf("Thread C is releasing the semaphore\n");
    sem_up(sem);
}

void threadB() {
    uthread_create(threadC, NULL);
    printf("Thread B released the semaphore\n");
    sem_up(sem);
}

void threadA() {
    uthread_create(threadB, NULL);

    printf("Thread A grabbed the semaphore!\n");
    sem_down(sem);


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
