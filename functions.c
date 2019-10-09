#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <time.h>
#include <wait.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/errno.h>
#include "functions.h"

int assignPID(pd data, int position){
	data.id= rand()%10000 + position;
	if((data.id)<1000) data.id= (data.id) +1000;
	return data.id;
}

int sharedMemoryCreate(int key){
	int shmid = shmget(key, sizeof(sm *), IPC_CREAT|0600);
	if(shmid<0){
		printf("Error while creating a shared memory segment.\n");
		perror("Semcreate\n");
		printf("%d", errno);
		exit(1);
	}
	return shmid;
}

sm * sharedMemoryAttach(int key){
	sm * shMem;
	shMem= (sm *)shmat(key, NULL, 0); 	//DEFINITE NEED FOR CASTING
	if(shMem==(sm *)-1){
		printf("Error while attaching to a shared memory segment.\n");
		exit(-1);
	}
	return shMem;
}



int semaphoreCreate(int key, int value){
	int sem;
	sem=semget(key,1,IPC_CREAT|0600);
	if(sem==-1){
		printf("Error while creating semaphore.\n");
		exit(-1);
	}
	union semun{			//for the use of semctl, linux manual
		int val;
		struct semid_ds * buf;
		ushort * array;
	}arg;
	arg.val=value;
	//now we initialise
	int s=semctl(sem, 0, SETVAL, arg.val);
	if(s==-1){
		printf("Error while initialising the new semaphore.\n");
		exit(-1);
	}
	return sem;
}

int down(int * sem){
	struct sembuf down;
	down.sem_num=0;
	down.sem_op=-1;
	down.sem_flg=0;
	if(semop(*sem, &down, 1)==-1){
		printf("Error while reducing semaphore value.\n");
		exit(-1);
	}
	return 0;
}

int up(int * sem){
	struct sembuf up;//=(struct sembuf)malloc(sizeof(struct sembuf));
        up.sem_num=0;
        up.sem_op=1;
        up.sem_flg=0;
        if(semop(*sem, &up, 1)==-1){
                printf("Error while increasing semaphore value.\n");
                exit(-1);
        }
	return 0;
}


long double findMin(long double time1, long double time2,long double time3){
	long double min=time1;
	if(min > time2) min=time2;
	if(min > time3) min=time3;
	return min;
}

long double findMax(long double time1, long double time2,long double time3){
        long double max=time1;
        if(max < time2) max=time2;
        if(max < time3) max=time3;
        return max;
}
