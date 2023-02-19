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

/*Global variables to keep track of previous signal action and timer configuration*/
struct sigaction previous_action;
struct itimerval previous_timer;

/*Signal Handler for SIGVTALARM*/
void timer_interrupt_handler(int signal)
{
	uthread_yield();	//force the current thread to yield
}

void preempt_disable(void)
{
	sigset_t signal;
	/*Initialize the signal mask*/
    sigemptyset(&signal);
    sigaddset(&signal, SIGVTALRM);
	/*block SIGVTALRM */
    sigprocmask(SIG_BLOCK, &signal, NULL);
}

void preempt_enable(void)
{
	sigset_t signal;
	/*Initialize the signal mask*/
    sigemptyset(&signal);
    sigaddset(&signal, SIGVTALRM);
	/*unblock SIGNTALRM*/
    sigprocmask(SIG_UNBLOCK, &signal, NULL);
}

void preempt_start(bool preempt)
{
	/* TODO Phase 4 */
	struct sigaction action;
	struct itimerval timer; 

	/* Signal handler for SIGVTALRM */
	action.sa_handler = timer_interrupt_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGVTALRM, &action, &previous_action);

	/* Timer configuration to fire an alarm a hundred times per second */
	timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 1000000 / HZ;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 1000000 / HZ;
    setitimer(ITIMER_VIRTUAL, &timer, &previous_timer);
}

void preempt_stop(void)
{
	/*Restore the previous action for SIGVTALRM*/
	sigaction(SIGVTALRM, &previous_action, NULL);
	
	/*Restore the previous timer configuration for SIGVTALRM*/
    setitimer(ITIMER_VIRTUAL, &previous_timer, NULL);
}

