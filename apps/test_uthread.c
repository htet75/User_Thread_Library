#include "uthread.h"
#include <stdio.h>

void thread_func3(void *arg) {
    int thread_num = *(int *)arg;
    printf("Thread %d starting\n", thread_num);
    for (int i = 0; i < 3; i++) {
        printf("Thread %d running, iteration %d\n", thread_num, i);
        uthread_yield();
    }
    printf("Thread %d exiting\n", thread_num);
}

void thread_func2(void *arg) {
    int arg3 = 3;
    uthread_create(thread_func3, &arg3);
    int thread_num = *(int *)arg;
    printf("Thread %d starting\n", thread_num);
    for (int i = 0; i < 3; i++) {
        printf("Thread %d running, iteration %d\n", thread_num, i);
        uthread_yield();
    }
    printf("Thread %d exiting\n", thread_num);
}

void thread_func1(void *arg) {
    int arg2 = 2;
    uthread_create(thread_func2, &arg2);
    int thread_num = *(int *)arg;
    printf("Thread %d starting\n", thread_num);
    for (int i = 0; i < 3; i++) {
        printf("Thread %d running, iteration %d\n", thread_num, i);
        uthread_yield();
    }
    printf("Thread %d exiting\n", thread_num);
}

int main() {
    int arg1 = 1;
    uthread_run(false, thread_func1, &arg1);
    return 0;
}
