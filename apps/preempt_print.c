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

void thread3()
{
    time_t start, end;
    time(&start);
    int count = 0;
    while (count < 10) {
        time(&end);
        if (difftime(end, start) >= 1.0) 
        {
            printf("Thread 3\n");
            time(&start);
            count++;
        }
        // uthread_yield();
        // sleep(1);
    }
}

void thread2()
{
    time_t start, end;
    uthread_create(thread3, NULL);
    time(&start);
    int count = 0;
    while (count < 10) {
        time(&end);
        if (difftime(end, start) >= 1.0) 
        {
            printf("Thread 2\n");
            time(&start);
            count++;
        }
        // uthread_yield();
        // sleep(1);
    }
}

void thread1()
{
    time_t start, end;
    uthread_create(thread2, NULL);
    time(&start);
    int count = 0;
    while (count < 10) {
        time(&end);
        if (difftime(end, start) >= 1.0) 
        {
            printf("Thread 1\n");
            time(&start);
            count++;
        }
        // uthread_yield();
        // sleep(1);
    }
}

int main()
{
    // preempt_start(true);
    uthread_run(true, thread1, NULL);
    return 0;
}
