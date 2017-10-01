#include "scheduler.h"
#include <stdio.h>

#define N_QUEUES 3

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

QUEUE ready[N_QUEUES];
QUEUE waitinginevent[MAXTHREAD];
int current_queue;
int q_timer = 0;
int q = 0;

void scheduler(int arguments)
{
	int old,next;
	int changethread=0;
	int waitingthread=0;

	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;

	if(event==NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status=READY;
		_enqueue(&ready[0], callingthread);
	}

	if(event==BLOCKTHREAD)
	{

		threads[callingthread].status=BLOCKED;
		_enqueue(&waitinginevent[blockevent],callingthread);

		changethread=1;
	}

	if(event==ENDTHREAD)
	{
		threads[callingthread].status=END;
		changethread=1;
	}

	if(event==UNBLOCKTHREAD)
	{
			threads[callingthread].status=READY;
			_enqueue(&ready[0],callingthread);
	}


	if(event==TIMER)
	{
			q_timer += 1;
			if (q_timer >= q) {
				changethread = 1;
				threads[callingthread].status=READY;
				if (current_queue < N_QUEUES) {
					_enqueue(&ready[current_queue+1], callingthread);
				} else {
					_enqueue(&ready[current_queue], callingthread);
				}
				q_timer = 0;
			}
	}

	if(changethread)
	{
		old=currthread;
		q = 0;
		next = _dequeue(&ready[q]);
		while (next != old && next == 0) {
			next = _dequeue(&ready[++q % N_QUEUES]);
		}
		printf("old thread: %d, next thread: %d, next queue: %d\n", old, next, current_queue);
		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}
