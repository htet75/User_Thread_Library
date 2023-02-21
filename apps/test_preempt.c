#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <uthread.h>
#include <private.h>

#define max_count 5

void thread3()
{
    time_t start, end;
    time(&start);
    int count = 0;
    while (count < max_count) {
        preempt_disable();
        time(&end);
        if (difftime(end, start) >= 1.0) 
        {
            printf("Thread 3: %d\n", count);
            time(&start);
            count++;
        }
        // uthread_yield();
        // sleep(1);
    }
    preempt_enable();
    while(count < max_count*2)
    {
        time(&end);
        if(difftime(end,start) >= 1.0)
        {
            printf("Thread 3: %d\n", count);
            time(&start);
            count++;
        }
    }
}

void thread2()
{
    time_t start, end;
    uthread_create(thread3, NULL);
    time(&start);
    int count = 0;
    while (count < max_count) {
        time(&end);
        if (difftime(end, start) >= 1.0) 
        {
            printf("Thread 2: %d\n", count);
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
    while (count < max_count) {
        time(&end);
        if (difftime(end, start) >= 1.0) 
        {
            printf("Thread 1: %d\n", count);
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
    printf("Preempt enabled:\n");
    uthread_run(true, thread1, NULL);

    // preempt_start(false);
    printf("Prempt disabled:\n");
    uthread_run(false, thread1, NULL);
    return 0;
}
