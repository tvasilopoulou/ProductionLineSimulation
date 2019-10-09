#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>
#include <wait.h>
#include <sys/sem.h>
#include <stdbool.h>

#define SHMKEY 1111
#define SEMKEY 9999

typedef struct procData{
        clock_t timestamp;      //time of process birth
        int id;                 //4-digit proc_id
        bool painted;
	int number;
	long double timeSpent;
	clock_t finishTime;
}pd;                            //add component id for tester


typedef struct qnode{
    pd proc;
    struct qnode * next;
}qnode;

typedef struct queue{
    struct qnode *front, *rear;
    int size;
}queue;

//queue qe;

typedef struct sharedMemory{
	int semSM;              //shared memory semaphore
        int semFull;            //full semaphore
        int semEmpty;           //empty semaphore
	bool painted;
	char str[1024];						//PROCDATA *
	pd procdata;
}sm;

typedef struct timeVar{
	long double key;
}timeVar;


int assignPID(pd , int);
int sharedMemoryCreate(int);
//sm * sharedMemoryInitialise(sm *);
sm * sharedMemoryAttach(int);
void sharedMemoryDetach(sm * , int );
int semaphoreCreate(int , int );
int down(int *);
int up(int * );
qnode * newNode(pd );
queue *createQueue();
void enQueue(queue *, pd );
qnode *deQueue(queue *);
int isEmpty(queue* );
long double findMin(long double time1, long double time2,long double time3);
long double findMax(long double time1, long double time2,long double time3);
