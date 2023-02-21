#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define USEC 1000000 // Number of microseconds in a second

/*Global variables to keep track of previous signal action and timer configuration*/
struct sigaction previous_action;
struct itimerval previous_timer;

/* Signal Handler for SIGVTALARM */
void timer_interrupt_handler(int signum)
{
    uthread_yield();	//force the current thread to yield
}

/* Disable preemption from occuring */
void preempt_disable(void)
{
    sigset_t signal;
    /* Initialize the signal mask */
    sigemptyset(&signal);
    sigaddset(&signal, SIGVTALRM);
    /* block SIGVTALRM */
    sigprocmask(SIG_BLOCK, &signal, NULL);
}

/* Reenables preemption to work again */
void preempt_enable(void)
{
    sigset_t signal;
    /*Initialize the signal mask*/
    sigemptyset(&signal);
    sigaddset(&signal, SIGVTALRM);
    /* unblock SIGNTALRM */
    sigprocmask(SIG_UNBLOCK, &signal, NULL);
}

void preempt_start(bool preempt)
{
    /* To prevent the timer from interrupting as soon as it goes off */
    preempt_disable();

    /* Enable preemption if asked */
    if(preempt)
    {
        /* Declare necessary signal/timer objects */
        struct sigaction action;
        struct itimerval timer; 

        /* Signal handler for SIGVTALRM */
        sigemptyset(&action.sa_mask);
        action.sa_handler = timer_interrupt_handler;
        action.sa_flags = SA_RESTART; // In case preemption happens on a read/write etc.
        sigaction(SIGVTALRM, &action, &previous_action);

        /* Timer configuration to fire an alarm at 100 Hz */
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = USEC / HZ;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = USEC / HZ;

        /* Create virtual timer and store original timer into previous timer */
        setitimer(ITIMER_VIRTUAL, &timer, &previous_timer);

        /* Enable preempt */
        preempt_enable();
    }

}

/* Restores the previously stored values for the sigaction and previous timer */
void preempt_stop(void)
{
    /*Restore the previous action for SIGVTALRM*/
    sigaction(SIGVTALRM, &previous_action, NULL);
    
    /*Restore the previous timer configuration for SIGVTALRM*/
    setitimer(ITIMER_VIRTUAL, &previous_timer, NULL);
}

