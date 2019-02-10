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


readyQ *rq;
int *availableThreadIds;
  
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

	if(rq->head == NULL){
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
		node *nextTemp = rq->head;

		while(nextTemp != NULL){
			if (nextTemp->thr->id == id && temp == NULL){
				rq->head = nextTemp->next;
				struct thread* thr = nextTemp->thr;
				free(nextTemp);
				return thr;

			} else if(nextTemp->thr->id == id){
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

struct thread* poll (){
	if(rq->head == NULL){
		return NULL;

	} else{
		node *temp = rq->head;
		node *nextTemp = rq->head->next;

		rq->head = nextTemp;
		struct thread* thre = temp->thr;
		free(temp);
	
		return thre;
	}
}


/* This is the thread control block */



struct thread *runningThread;



void thread_init(void) {
	/* your optional code here */

	initializeQ();

	ucontext_t savedContext;
	int err = 0;
	err = getcontext(&savedContext);
	runningThread = (struct thread *)malloc((sizeof(struct thread)));
	runningThread->id = 0;
	runningThread->mycontext = savedContext;

	availableThreadIds = (int *)malloc(THREAD_MAX_THREADS*sizeof(int));
	
	for(int i = 0; i < THREAD_MAX_THREADS; i++){
		availableThreadIds[i] = 0;
	}
}

Tid thread_id() {

	return THREAD_INVALID;
}

Tid thread_create(void (*fn) (void *), void *parg) {
	

}

Tid thread_yield(Tid want_tid) {
	int err = 0;
	if (want_tid == runningThread->id || want_tid == THREAD_SELF){
		err = setcontext(&(runningThread->mycontext));
		assert(!err);
		return want_tid;

	} else if(want_tid == THREAD_ANY){

		if (rq->head == NULL){
			return THREAD_NONE;
		}
		err = getcontext(&(runningThread->mycontext));
		runningThread->status = 1;
		assert(!err);
		enQ(runningThread);

		struct thread *threadFromQueue = poll();
		err = setcontext(&(runningThread->mycontext));
		assert(!err);
		return threadFromQueue->id;

	} else if (want_tid < -2 ){
		return THREAD_INVALID;
	} else{
		if (rq->head == NULL){
			return THREAD_INVALID;
		}
		struct thread *threadFromQueue = deQ(want_tid);
		if (threadFromQueue == NULL){
			return THREAD_INVALID;
		}
		err = getcontext(&(runningThread->mycontext));
		runningThread->status = 1;
		assert(!err);
		enQ(runningThread);

		err = setcontext(&(threadFromQueue->mycontext));
		assert(!err);
		return want_tid;
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
