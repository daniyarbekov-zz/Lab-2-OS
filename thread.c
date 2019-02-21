#include <assert.h>
#include <stdlib.h>
#include <ucontext.h>
#include "thread.h"
#include "interrupt.h"




// **********Two queue version!!!!!*********



struct thread {
	/* ... Fill this in ... */

	Tid id;
	//status =  0(running), 1(ready), 2(delete), //  lab3: 3(sleep)
	int status;
	void *stack_ptr;
	ucontext_t mycontext;
};


typedef struct node {
	struct thread *thr;
	struct node *next;
} node;



// lab3 update: changed typedef from struct readyQ to struct wait_queue
// so that it could be used in sleep and wakeup
typedef struct wait_queue{
	node *head;
	node *back;
} readyQ;

/* This is the wait queue structure */

/*
struct wait_queue {

	node *head;
	node *back;
};
*/


node *buildNode(struct thread *thr)  {
    node *temp = (node*)malloc(sizeof(node));
	temp->thr = thr;
    temp->next = NULL;

	return temp;
}




void enQ (struct thread *thr, readyQ *rq){
	//printf("calling build node\n");
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






struct thread* deQ (Tid id, readyQ *rq){
	if(rq->head == NULL){
		return NULL;

	} else{
		node *temp = NULL;
		node *nextTemp = rq->head;

		while(nextTemp != NULL){
			if (nextTemp->thr->id == id && temp == NULL){
				rq->head = nextTemp->next;
				if(rq->head == NULL){
					rq->back = rq->head;
				}
				struct thread* thr = nextTemp->thr;
				free(nextTemp);
				return thr;

			} else if(nextTemp->thr->id == id){
				if(rq->back == nextTemp){
					rq->back = temp;
				}

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


struct thread* poll (readyQ *rq){
	if(rq->head == NULL){
		return NULL;

	} else{
		node *temp = rq->head;
		node *nextTemp = rq->head->next;

		rq->head = nextTemp;
		struct thread* thre = temp->thr;
		free(temp);

		if(rq->head == NULL){
			rq->back = NULL;
		}

		return thre;
	}
}


struct thread *runningThread;
readyQ *readyq;
readyQ *killq;
readyQ *waitq; //lab3
int *availableThreadIds;




void printNodes(readyQ *rq){
	//printf("PRINTING IS started\n");
	 node *temp = rq->head;
	while(temp != NULL){
	//	printf("node in queue: %d, ", temp->thr->id);
		temp = temp->next;
	}
	//printf("\n");
	//printf("finished printing\n");
	return;
}






void destroyNodes( readyQ *killq){
	//printf("in destror node start0.......\n");
	node *temp = killq->head;
	if(temp == NULL){
		return;
	}
	node *sub = temp;
	while(temp != NULL){
	//	printf("in destror node......\n");
		sub = temp;
		temp = temp->next;
		free(sub->thr->stack_ptr);
		free(sub->thr);
		free(sub);
	}
	//printf("finished destroynode\n");
	killq->head = NULL;
	killq->back = NULL;
	return;
}



/*int checkQ(Tid tid, readyQ *rq){
		//printf("THREAD checkq IS STARTED\n");
	if (rq->head == NULL){
		return THREAD_NONE;
	} else{
		node *temp = rq->head;

		while(temp != NULL){
			if (temp->thr->id == tid){

				temp->thr->status = 2;
				//printf("THREAD checkQ IS done: SENDING ID\n");
				return temp->thr->id; //TODO: change
			}
			temp = temp->next;
		}
		//printf("checkq is done, CANT FIND THIS ID\n");
		return THREAD_INVALID;
	}
}*/

int checkIfExists(Tid tid, readyQ *rq){

	if (rq->head == NULL){
		return THREAD_NONE;
	} else{
		node *temp = rq->head;

		while(temp != NULL){
			if (temp->thr->id == tid){

				//printf("THREAD CHECK IF EXISTS IS done: SENDING ID\n");
				return temp->thr->id; //TODO: change
			}
			temp = temp->next;
		}
		return THREAD_INVALID;
	}
}




/* This is the thread control block */







void thread_init(void) { // lab3 update

	//printf("INTIALIATION STARTED\n");
	int enabled = interrupts_set(0); // set to block state, save Previous state into variable "enabled"

    readyq = (struct wait_queue*)malloc(sizeof(struct wait_queue));
    readyq->head = NULL;
	readyq->back = NULL;

	killq = (struct wait_queue*)malloc(sizeof(struct wait_queue));
	killq->head = NULL;
	killq->back = NULL;



	int err = 0;
	runningThread = (struct thread *)malloc((sizeof(struct thread)));
	err = getcontext(&(runningThread->mycontext));
	assert(!err);
	runningThread->id = 0;
	runningThread->status = 0;
	runningThread->stack_ptr = NULL;
	availableThreadIds = (int *)malloc(THREAD_MAX_THREADS*sizeof(int));

	for(int i = 0; i < THREAD_MAX_THREADS; i++){
		availableThreadIds[i] = 0;
	}
	availableThreadIds[0] = 1;

	printf("init finish \n");
	interrupts_set(enabled);

}

void thread_stub(void (*thread_main)(void *), void *arg) { //lab3 update

	interrupts_on();
	Tid ret;
	//printf("THREAD STUB started\n");
	thread_main(arg); // call thread_main() function with arg
	ret = thread_exit();
	// we should only get here if we are the last thread.
	assert(ret == THREAD_NONE);
	// all threads are done, so process should exit
	exit(0);
}

Tid thread_id() {
	//printf("THREAD ID STARTED\n");
	return runningThread->id;
}

Tid thread_create(void (*fn) (void *), void *parg) {
//	printf("THREAD CREATE IS STARTED\n");

	int enabled = interrupts_set(0); // set to block state, save Previous state into variable "enabled"
	Tid assignedID;
	int i = 1;
	for (; i < THREAD_MAX_THREADS; i++){
		if(availableThreadIds[i] == 0){
			break;
		}
	}
	if(i == THREAD_MAX_THREADS){
		interrupts_set(enabled);
		return THREAD_NOMORE;
	}
	assignedID = i;
	availableThreadIds[i] = 1;

	struct thread *createdThread = (struct thread *)malloc((sizeof(struct thread)));

	if (createdThread == NULL) {
		interrupts_set(enabled);
		return THREAD_NOMEMORY;
	}

	void * stackPTR = (void *)malloc(THREAD_MIN_STACK);

	if(stackPTR == NULL){
		interrupts_set(enabled);
		return THREAD_NOMEMORY;
	}
	createdThread->stack_ptr = stackPTR;
	createdThread->id = assignedID;


	void *assigningStack = (createdThread->stack_ptr +THREAD_MIN_STACK) - (THREAD_MIN_STACK)%16 -8;


	//createdThread->mycontext;
	int err = 0;
	err = getcontext(&(createdThread->mycontext));
	assert(!err);

	createdThread->mycontext.uc_mcontext.gregs[REG_RSP] = (unsigned long) assigningStack;
	createdThread->mycontext.uc_mcontext.gregs[REG_RIP] = (unsigned long) &thread_stub;
	createdThread->mycontext.uc_mcontext.gregs[REG_RDI] = (unsigned long) fn;
	createdThread->mycontext.uc_mcontext.gregs[REG_RSI] = (unsigned long) parg;


	createdThread->status  = 1;
	enQ(createdThread, readyq);
	//printf("THREAD CREATE IS DONE\n");
	interrupts_set(enabled);
	return assignedID;
}


Tid thread_yield(Tid want_tid) { //lab3 update
	//printf("THREAD YEILD IS STARTED\n");

	//printf("want tid: %d \n",want_tid);

	//Tid check1 = checkIfExists(want_tid);

	int enabled = interrupts_set(0);

	destroyNodes(killq);

//	printNodes(readyq);
	//printf("value of runnign thread is %d\n",runningThread->id);

	int err = 0;
	if (want_tid == runningThread->id || want_tid == THREAD_SELF){
		//printf("return itself\n");
		interrupts_set(enabled);
		return runningThread->id;

	}
	else if(want_tid == THREAD_ANY){ //yield any

		if (readyq->head == NULL){
			interrupts_set(enabled);
			return THREAD_NONE;
		}
		// yield any
		//printf("saving context in yeld\n");
		int setcontext_called = 0;
		struct thread *threadFromQueue = poll(readyq);
		err = getcontext(&(runningThread->mycontext));
		assert(!err);

		if(setcontext_called == 1){
			interrupts_set(enabled);
			return threadFromQueue->id;
		}

		//printf("polled from queue id is  %d\n",threadFromQueue->id);
		//printf("running thread is %d\n",runningThread->id);

		enQ(runningThread,readyq);
		runningThread->status = 1;
		runningThread = threadFromQueue;
		threadFromQueue->status = 0;

		setcontext_called = 1;
		err = setcontext(&(threadFromQueue->mycontext));
		assert(!err);
		interrupts_set(enabled);
		return THREAD_FAILED;

	}

	else if (want_tid < -2 || want_tid > THREAD_MAX_THREADS){
		interrupts_set(enabled);
		return THREAD_INVALID;
	}

	else{
		if (readyq->head == NULL){
			interrupts_set(enabled);
			return THREAD_INVALID;
		} else if (!availableThreadIds[want_tid]){
			interrupts_set(enabled);
			return THREAD_INVALID;
		}
		struct thread *threadFromQueue = deQ(want_tid,readyq);
		if (threadFromQueue == NULL){
			interrupts_set(enabled);
			return THREAD_INVALID;
		}
		//printf("we found thread and now we we want to swtich context\n");
		int setcontext_called = 0;
		err = getcontext(&(runningThread->mycontext));
		assert(!err);

		if(setcontext_called == 1){
			interrupts_set(enabled);
			return threadFromQueue->id;
		}


		runningThread->status = 1;
		enQ(runningThread,readyq);
		threadFromQueue->status = 0;
		runningThread = threadFromQueue;


		setcontext_called = 1;
		err = setcontext(&(threadFromQueue->mycontext));
		assert(!err);


		//printf("dequed from queue id is  %d\n",threadFromQueue->id);
		//printf("running thread is %d\n",runningThread->id);
		interrupts_set(enabled);
		return THREAD_FAILED;
	}
		//printf("THREAD YEILD IS DONE\n");
	interrupts_set(enabled);
	return THREAD_FAILED;
}





Tid thread_exit() { //lab3 update
	int enabled = interrupts_set(0);

	if (runningThread == NULL){
		interrupts_set(enabled);
		return THREAD_FAILED;
	}
	if (readyq->head == NULL){
		interrupts_set(enabled);
		return THREAD_NONE;
	}

	struct thread *threadFromQueue = poll(readyq);
	if(threadFromQueue == NULL){
		interrupts_set(enabled);
		return THREAD_NONE;
	}
	runningThread->status = 2;
	enQ(runningThread, killq);

	availableThreadIds[runningThread->id] = 0;

	runningThread = threadFromQueue;
	setcontext(&(runningThread->mycontext));
	interrupts_set(enabled);
	return THREAD_NONE;
}


Tid thread_kill(Tid tid) { //lab3 update
	int enabled = interrupts_set(0);

	if (runningThread == NULL){
		interrupts_set(enabled);

		return THREAD_FAILED;

	}
	if (readyq->head == NULL){
		interrupts_set(enabled);

		return THREAD_INVALID;
	}

	if(tid == runningThread->id || tid < 0 || availableThreadIds[tid] == 0){
		interrupts_set(enabled);
		return THREAD_INVALID;
	}

	struct thread *threadFromQueue = deQ(tid,readyq);

	if(threadFromQueue == NULL){
		interrupts_set(enabled);

		return THREAD_INVALID;
	}

	enQ(threadFromQueue, killq);
	threadFromQueue->status = 2;
	availableThreadIds[threadFromQueue->id] = 0;
	interrupts_set(enabled);

	return threadFromQueue->id;
}







/*******************************************************************
 * Important: The rest of the code should be implemented in Lab 3. *
 *******************************************************************/

/* make sure to fill the wait_queue structure defined above */

// init an empty wait queue
struct wait_queue *
wait_queue_create()
{
	struct wait_queue *wq;

	wq = malloc(sizeof(struct wait_queue));
	assert(wq);

	wq->head = NULL;
	wq->back = NULL;

	return wq;
}

void
wait_queue_destroy(struct wait_queue *wq)
{

	/* method 1
	while(wq->head != NULL){
		node *temp = wq->head;
		wq->head = wq->head->next;
		free(temp);
	}
	 */

	//method 2
	destroyNodes(wq);
	free(wq);
}



// suspend calling thread, push to waitQ
Tid
thread_sleep(struct wait_queue *queue)
{
	int enabled = interrupts_set(0); // set to block state, save Previous state into variable "enabled"
	int err = 0;
	// if waitq is empty
	if (queue->head == NULL){	// why????
		interrupts_set(enabled);
		return THREAD_INVALID;
	}

	// if readyq empty, i.e. no more thread to run
	if (readyq->head == NULL){
		interrupts_set(enabled); //restore to previous state
		return THREAD_NONE;
	}


	int setcontext_called = 0;
	struct thread *threadFromQueue = poll(readyq);
	err = getcontext(&(runningThread->mycontext));
	assert(!err);

	if(setcontext_called == 1){
		interrupts_set(enabled);
		return threadFromQueue->id;
	}

	enQ(runningThread,waitq); // push to end of waitq
	runningThread->status = 3; // set status to sleep
	runningThread = threadFromQueue; // run first thread from readyq
	threadFromQueue->status = 0; // set status to running

	setcontext_called = 1;
	err = setcontext(&(threadFromQueue->mycontext));
	assert(!err);

	interrupts_set(enabled);

	return THREAD_FAILED;
}



int
thread_wakeup(struct wait_queue *queue, int all)
{
	printf("start wake up\n");
	int enabled = interrupts_set(0);
	printf("after enabled\n");

	// if empty waitq
	if (queue->head == NULL || queue == NULL){
	printf("wakeup head NULL \n");
	interrupts_set(enabled);

	return 0;
	}

	// if only wake up one thread in waitq
	if (all == 0){
		struct thread *threadFromQueue = poll(queue); // poll first element in waitq
		threadFromQueue->status = 1; //set status to ready
		enQ(threadFromQueue,readyq); // push to end of readyQ
		printf("wakeup all = 0\n");
		interrupts_set(enabled);

		return 1;
	}

	if (all == 1){
		int counter = 0;
		while(queue->head != NULL){
			struct thread *threadFromQueue = poll(queue);
			threadFromQueue->status = 1; //set status to ready
			enQ(threadFromQueue,readyq); // push to end of readyQ
			counter += 1; //
		}
		printf("wakeup all = 1\n");

		interrupts_set(enabled);
		return counter;
	}
	// will never reach below
	interrupts_set(enabled);
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




//  ****************************************************************************************************************************************************************************

