#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>

#include <private.h>
#include <uthread.h>

void thread3(void* arg)
{
    (void)arg;
    while (true) {
        printf("Thread 3\n");
        sleep(1);
    }
}

void thread2(void* arg)
{
    (void)arg;
    uthread_create(thread3, NULL);
    while (true) {
        printf("Thread 2\n");
        sleep(1);
    }
}

void thread1(void* arg)
{
    (void)arg;
    uthread_create(thread2, NULL);
    while (true) {
        printf("Thread 1\n");
        sleep(1);
    }
}

int main()
{
    // preempt_start(true);
    uthread_run(true, thread1, NULL);
    return 0;
}
