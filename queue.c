#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <time.h>
#include <wait.h>
#include <sys/sem.h>
#include "functions.h"

qnode * newNode(pd proc){
	qnode *temp = (qnode*)malloc(sizeof(qnode));
	temp->next = NULL;
	temp->proc=proc;
	return temp;
}


queue *createQueue(){
	queue *q = (queue*)malloc(sizeof(queue));
	q->front = q->rear = NULL;
	q->size=0;
	return q;
}


void enQueue(queue *q, pd proc){
	qnode *temp = newNode(proc);
	if (q->rear == NULL){
		q->front = q->rear = temp;
		q->size++;
		return;
	}
	q->size++;
	q->rear->next = temp;
	q->rear = temp;
}


qnode *deQueue(queue *q){
	if (q->front == NULL)
		return NULL;
	q->size--;
	qnode *temp = q->front;
	q->front = q->front->next;
	if (q->front == NULL)
	q->rear = NULL;
	return temp;
}


int isEmpty(queue* pQueue) {		//needs to be qnode , dereffernece!!!
	if (pQueue == NULL) return 0;
//	if (pQueue->front == pQueue->rear)return 1;
//	else return 0;
	if (pQueue->size==0)return 1;
	else return 0;

}

