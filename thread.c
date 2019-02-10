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
	void *stack_ptr;
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


//remove all nodes in ready q whose status = 2
void remove_tobedeleted(){
	struct node *tempNext = rq->head;
	struct node *temp = NULL;

	while(tempNext == NULL || tempNext->next != NULL){
		if (tempNext-> thr->status == 2 && (rq->head == tempNext)){
			temp = tempNext;
			tempNext = tempNext->next;
			head = tempNext;

			struct node *thrPointer = temp->thr;
			availableThreadIds[thrPointer->id] = 0;
			//free the thread;
			free(temp->thr->stack_ptr);			
			free(temp->thr);
			//free node
			free(temp);
		} else if(tempNext->thr->status == 2){
			temp->next = tempNext->next;
			temp = tempNext;
			tempNext = tempNext->next;

			struct node *thrPointer = temp->thr;
			availableThreadIds[thrPointer->id] = 0;
			//free the thread;
			free(temp->thr->stack_ptr);			
			free(temp->thr);
			//free node
			free(temp);
		} else {
			temp = tempNext;
			tempNext = tempNext->next;
		}		
	 
	}
	
	return;
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
	runningThread->stack_ptr = NULL;
	availableThreadIds = (int *)malloc(THREAD_MAX_THREADS*sizeof(int));
	
	for(int i = 0; i < THREAD_MAX_THREADS; i++){
		availableThreadIds[i] = 0;
	}
}

void thread_stub(void (*thread_main)(void *), void *arg) {
	Tid ret;

	thread_main(arg); // call thread_main() function with arg
	ret = thread_exit();
	// we should only get here if we are the last thread. 
	assert(ret == THREAD_NONE);
	// all threads are done, so process should exit
	exit(0);
}

Tid thread_id() {

	return runningThread->id;
}

Tid thread_create(void (*fn) (void *), void *parg) {

	Tid assignedID;
	int i = 0;
	for (; i < THREAD_MAX_THREADS; i++){
		if(availableThreadIds[i] == 0){
			break;
		}
	}
	if(i == THREAD_MAX_THREADS){
		return THREAD_NOMORE;
	}
	assignedID = i;
	availableThreadIds[i] = 1;

	thread *createdThread = (struct thread *)malloc((sizeof(struct thread)));
	
	if (!assigningStack || createdThread) {
		return THREAD_NOMEMORY;
	}

	createdThread->stack_ptr = (void *)malloc(THREAD_MIN_STACK);	
	void *assigningStack = (createdThread->stack_ptr +THREAD_MIN_STACK) - (THREAD_MIN_STACK)%16;


	int err = 0;
	ucontext_t mycontext;
	err = getcontext(&mycontext);
	assert(!err);
	createdThread->mycontext = mycontext;

	createdThread->mycontext.gregs[REG_RSP] = (unsigned long) assigningStack;
	createdThread->mycontext.gregs[REG_RIP] = (unsigned long) &thread_stub;
	createdThread->mycontext.gregs[REG_RDI] = (unsigned long) fn;
	createdThread->mycontext.gregs[REG_RSI] = (unsigned long) parg;

	// add to ready Q

	createdThread->status  = 1;
	enQ(createdThread);

	return assignedID;
}


Tid thread_yield(Tid want_tid) {
	
	remove_tobedeleted();

	int err = 0;
	if (want_tid == runningThread->id || want_tid == THREAD_SELF){
		assert(!err);
		return runningThread->id;

	} else if(want_tid == THREAD_ANY){

		if (rq->head == NULL){
			return THREAD_NONE;
		}
		err = getcontext(&(runningThread->mycontext));
		runningThread->status = 1;
		assert(!err);
		enQ(runningThread);

		struct thread *threadFromQueue = poll();
		runningThread = threadFromQueue;
		threadFromQueue->status = 0;

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
		threadFromQueue->status = 0;
		runningThread = threadFromQueue;

		err = setcontext(&(threadFromQueue->mycontext));
		assert(!err);
		return want_tid;
	}

	return THREAD_SELF;
}



Tid thread_exit() {

	if (readyQ->head == NULL{
		return THREAD_NONE;
	}
	else{
		runningThread->status = 2; // set flag
		int err = thread_yield(THREAD_ANY);

	}
	return THREAD_FAILED;
}



Tid checkQ(Tid tid){
	if (rq->head == NULL){
		return THREAD_NONE;	
	}
	else{
		node *temp = rq->head;
		
		while(temp != NULL){
			if (temp->thr->id == tid){
				
				temp->thr->status = 2;
				return temp->thr->id; //TODO: change	
			}
			temp = temp->next;
		}
	return THREAD_INVALID;
	} 
}

Tid thread_kill(Tid tid) {
	if (tid == runningThread->id){
		return THREAD_INVALID;
	}
	int err = checkQ(tid);
	if (err == THREAD_NONE || err == THREAD_INVALID){
	return THREAD_INVALID;
	}
	return err;
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
