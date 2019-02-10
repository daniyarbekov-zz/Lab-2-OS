#include <assert.h>
#include <stdlib.h>
#include <ucontext.h>
#include "thread.h"
#include "interrupt.h"

/* This is the wait queue structure */




struct wait_queue {
	/* ... Fill this in Lab 3 ... */
};

struct thread {
	/* ... Fill this in ... */

	Tid id;
	//status = 0 (running),(ready) 
	int status;
	ucontext_t mycontext;
};



typedef struct node {
	struct thread *thr;
	struct node *next;
} node;


typedef struct readyQ{
	node *head;
	node *back;
} readyQ;

  
node *buildNode(struct thread *thr)  { 
    node *temp = (struct node*)malloc(sizeof(node));
	temp->thr = thr;
    temp->next = NULL; 
    
	return temp;  
} 
  
void initializeQ() { 
    rq = (struct readyQ*)malloc(sizeof(struct readyQ)); 
    rq->head = NULL;
	rq->back = NULL;
} 


void enQ (struct thread *thr){
	node *addedNode = buildNode(thr);

	if(rq->head == NULL && rq->back == NULL){
		rq->head = addedNode;
		rq->back = addedNode;
		return;
	}
	else if(rq->head == rq->back){
		rq->back = addedNode;
		rq->head->next = rq->back;
		return;
	} else{
		rq->back->next = addedNode;
		rq->back = rq->back->next;
		return;
	}
}

struct thread* deQ (Tid id){
	if(rq->head == NULL){
		return NULL;

	} else{
		node *temp = NULL;
		node *nextTemp = rq->head->next;

		while(nextTemp != NULL){
			if(nextTemp->thr->id == id){
				temp->next = nextTemp->next;
				struct thread* thr = nextTemp->thr;
				free(nextTemp);
				return thr;
			}
			temp = nextTemp;
			nextTemp = nextTemp->next;
		}
		return NULL;
	}
}


/* This is the thread control block */


Tid currentThreadId = -1000;
Tid currentSizeQ = 0;


readyQ *rq;

void thread_init(void) {
	/* your optional code here */

	initializeQ(rq);
	int err = 0;

	ucontext_t firstContext;
	err = getcontext(&firstContext);
	assert(!err);

	currentThreadId = 0;
}

Tid thread_id() {
	TBD();
	return THREAD_INVALID;
}

Tid thread_create(void (*fn) (void *), void *parg) {
	TBD();
	return THREAD_FAILED;
}

Tid thread_yield(Tid want_tid) {
	ucontext_t savedContext;
	int err = 0;
	err = getcontext(&savedContext);
	assert(!err);
	err = 0;

	if (want_tid == currentThreadId || want_tid == THREAD_SELF){
		err = setcontext(savedContext);
		assert(!err);
		return currentThreadId;

	} else if(want_tid == THREAD_ANY){
		//change it to the array implementation
		currentSizeQ += 1;

		if (rq->head == NULL){
			return THREAD_NONE;
		}
		struct thread *threadFromQueue = deQ(want_tid);;
		struct thread *runningThread = (struct thread *)malloc(sizeof(struct thread));

		//change implementation to the array
		runningThread->id = currentSizeQ + 1;
		runningThread->mycontext = savedContext;
		
		rq.enQ(runningThread);


	}

	



	return THREAD_SELF;
}

Tid thread_exit() {
	TBD();
	return THREAD_FAILED;
}

Tid thread_kill(Tid tid) {
	TBD();
	return THREAD_FAILED;
}




/*******************************************************************
 * Important: The rest of the code should be implemented in Lab 3. *
 *******************************************************************/

/* make sure to fill the wait_queue structure defined above */
struct wait_queue *
wait_queue_create()
{
	struct wait_queue *wq;

	wq = malloc(sizeof(struct wait_queue));
	assert(wq);

	TBD();

	return wq;
}

void
wait_queue_destroy(struct wait_queue *wq)
{
	TBD();
	free(wq);
}

Tid
thread_sleep(struct wait_queue *queue)
{
	TBD();
	return THREAD_FAILED;
}

/* when the 'all' parameter is 1, wakeup all threads waiting in the queue.
 * returns whether a thread was woken up on not. */
int
thread_wakeup(struct wait_queue *queue, int all)
{
	TBD();
	return 0;
}

/* suspend current thread until Thread tid exits */
Tid
thread_wait(Tid tid)
{
	TBD();
	return 0;
}

struct lock {
	/* ... Fill this in ... */
};

struct lock *
lock_create()
{
	struct lock *lock;

	lock = malloc(sizeof(struct lock));
	assert(lock);

	TBD();

	return lock;
}

void
lock_destroy(struct lock *lock)
{
	assert(lock != NULL);

	TBD();

	free(lock);
}

void
lock_acquire(struct lock *lock)
{
	assert(lock != NULL);

	TBD();
}

void
lock_release(struct lock *lock)
{
	assert(lock != NULL);

	TBD();
}

struct cv {
	/* ... Fill this in ... */
};

struct cv *
cv_create()
{
	struct cv *cv;

	cv = malloc(sizeof(struct cv));
	assert(cv);

	TBD();

	return cv;
}

void
cv_destroy(struct cv *cv)
{
	assert(cv != NULL);

	TBD();

	free(cv);
}

void
cv_wait(struct cv *cv, struct lock *lock)
{
	assert(cv != NULL);
	assert(lock != NULL);

	TBD();
}

void
cv_signal(struct cv *cv, struct lock *lock)
{
	assert(cv != NULL);
	assert(lock != NULL);

	TBD();
}

void
cv_broadcast(struct cv *cv, struct lock *lock)
{
	assert(cv != NULL);
	assert(lock != NULL);

	TBD();
}
