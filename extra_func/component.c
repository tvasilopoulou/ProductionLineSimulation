#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <time.h>
#include <wait.h>
#include <sys/sem.h>
#include <string.h>
#include "functions.h"


void components(int i, int j){
	pd proc;
        int prid=assignPID(proc, j + i);
        proc.id=prid;   //4 digit
        proc.timestamp=clock();
        proc.number=j;                  //current component number, passed on for tester
        if(down(&(sm[0]->semEmpty))<0){
        	printf("Error while decrementing semaphore empty.\n");
                exit(1);
        }
        if(down(&(sm[0]->semSM))<0){
        	printf("Error while decrementing shared memory semaphore.\n");
                exit(1);
        }
        sm[0]->procdata=proc;           //pass data into shared memory

        if(up(&(sm[0]->semFull))<0){
        	printf("Error while decrementing shared memory semaphore.\n");
                exit(1);
        }
        if(up(&(sm[0]->semSM))<0){
        	printf("Error while decrementing semaphore empty.\n");
                exit(1);
        }
}
