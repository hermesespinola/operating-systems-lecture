#include "scheduler.h"
#include <stdio.h>

#define N_QUEUES 4

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

QUEUE ready[N_QUEUES];
QUEUE waitinginevent[MAXTHREAD];
int current_queue;
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
			changethread = 1;
			threads[callingthread].status=READY;
			if (q == N_QUEUES) {
				_enqueue(&ready[q], callingthread);
			} else {
				_enqueue(&ready[q + 1], callingthread);
			}
	}

	if(changethread)
	{
		q = 0;
		old=currthread;
		next = _dequeue(&ready[q]);
		while (next != old && next == 0) {
			next = _dequeue(&ready[++q % N_QUEUES]);
		}
		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}
