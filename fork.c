#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/errno.h>
#include <time.h>
#include <wait.h>
#include <sys/sem.h>
#include <string.h>
#include "functions.h"


int main(int argc, char * argv[]){
	if (argc!=2){
		printf("Error: This program can only accept 2 arguments: [./prog_name Y]. Try again.\n");
		perror("");
		exit(-1);
	}
	int y=atoi(argv[1]);				//input: number of components to create
	srand(123);					//srand function with seed=123;
							//tested components' queues(type 0, 1,2) 
	queue *q0 = createQueue();
	queue *q1 = createQueue();
	queue *q2 = createQueue();

	//creation of shared memory segments
	sm * sm[3];					//shared memory segments
	int smkey[3];					//shared memory keys
	for(int i=0; i<3; i++){				//shared memory segments, and initialise semaphores
		smkey[i]=sharedMemoryCreate((SHMKEY+i));
		sm[i]=sharedMemoryAttach(smkey[i]);
		sm[i]->semSM=semaphoreCreate((SEMKEY+10*i), 1);
                sm[i]->semFull=semaphoreCreate((SEMKEY+10*i+1), 0);
                sm[i]->semEmpty=semaphoreCreate((SEMKEY+10*i+2), 1);

	}

	//first 3 processes, break each piece up
	int components[3];					//processes for each phase
	int painter;
	int tester[3];
	int assembly;
	int pids[8];
	for(int j=0; j<3; j++){
		if((components[j]=pids[j]=fork())<0){			//pids is for the final stage, stores all process id's
			printf("Problem while creating child.\n");
			perror("");
			return -1;
		}
		else if(components[j]==0){			//if forks succeedes
			for(int i=0; i<y; i++){
				pd proc;			//create process data
				int prid=assignPID(proc, j + i);		//assign random 4-digit id
		                proc.id=prid;	//4-digit
		                proc.timestamp=clock();				//current time
				proc.number=j;			//current component number, passed on for tester
				if(down(&(sm[0]->semEmpty))<0){
					printf("Error while decrementing semaphore empty.\n");
					perror("");
					exit(1);
				}
				if(down(&(sm[0]->semSM))<0){
                                        printf("Error while decrementing shared memory semaphore.\n");
					perror("");
                                        exit(1);
                                }
				//access in sm[0]
				sm[0]->procdata=proc;		//pass data into first shared memory segment

				if(up(&(sm[0]->semFull))<0){
                                        printf("Error while incrementing semaphore full.\n");
					perror("");
                                        exit(1);
                                }
				if(up(&(sm[0]->semSM))<0){
                                        printf("Error while incrementing shared memory semaphore.\n");
					perror("");
                                        exit(1);
                                }
				//shut access in sm[0]
			}
			return 0;
		}

	}

	if((painter=pids[3]=fork())<0){
		printf("Error while creating the painter process\n");
		perror("");
		return -1;
	}
	else if(painter==0){
		//manage to access shared memory segment between component production and painter
		long double uptime = (long double)((long double)clock() / (long double)(CLOCKS_PER_SEC)); //get current time of import in painter per second
		for(int a=0; a<3*y; a++){		//3*y loops-> for every component
		if(down(&(sm[0]->semFull))<0){
                	printf("Error while decrementing semaphore full.\n");
			perror("");
                        exit(1);
                }
                if(down(&(sm[0]->semSM))<0){
                	printf("Error while decrementing shared memory semaphore.\n");
			perror("");
                        exit(1);
                }
		//access in sm[0]
		pd proc=sm[0]->procdata;			//copy the data from the shared memory segment
		//now shut sm[0]
		if(up(&(sm[0]->semEmpty))<0){
                	printf("Error while incrementing semaphore empty.\n");
			perror("");
                        exit(1);
                }
                if(up(&(sm[0]->semSM))<0){
                	printf("Error while incrementing shared memory semaphore.\n");
			perror("");
                        exit(1);
                }
		//pass this into memory segment between painter and tester
		if(down(&(sm[1]->semEmpty))<0){
                	printf("Error while decrementing semaphore empty.\n");
			perror("");
                        exit(1);
                }
                if(down(&(sm[1]->semSM))<0){
                	printf("Error while decrementing shared memory semaphore.\n");
			perror("");
                        exit(1);
                }
		long double fintime = (long double)((long double)clock() / (long double)(CLOCKS_PER_SEC));//  get time again to calculate elapsed time
		proc.timeSpent=(long double)(fintime-uptime);		//secs in painter
		sm[1]->procdata=proc;            //pass data into shared memory
                if(up(&(sm[1]->semFull))<0){
                	printf("Error while incrementing semaphore full.\n");
			perror("");
                	exit(1);
                }


                if(up(&(sm[1]->semSM))<0){
	                printf("Error while incrementing shared memory semaphore.\n");
                        perror("");
			exit(1);
                }
		}
		return 0;
	}
	//tester
	for(int k=0; k<3; k++){
                if((tester[k]=pids[k+4]=fork())<0){			//fork tester processes
                        printf("Problem while creating tester process.\n");
			perror("");
                        return -1;
                }
                else if(tester[k]==0){
			int c=0;
			while(c<y){				//for is not as helpful in this case
			if(down(&(sm[1]->semFull))<0){
                	        printf("Error while decrementing semaphore full.\n");
				perror("");
                	        exit(1);
                	}
                	if(down(&(sm[1]->semSM))<0){
                	        printf("Error while decrementing shared memory semaphore.\n");
				perror("");
                	        exit(1);
                	}
			//access sm[1]
			if((sm[1]->procdata.number)!=k){			//make sure to send the correct type of component in the right tester
                		if(up(&(sm[1]->semFull))<0){			//close the memory for someone else to read
                		        printf("Error while incrementing semaphore full.\n");
					perror("");
                		        exit(1);
                		}
				if(up(&(sm[1]->semSM))<0){
                                        printf("Error while incrementing shared memory semaphore.\n");
					perror("");
                                        exit(1);
                                }
				continue;		//if it's no the right one, redirect with same c
			}
			else{				//if correct type of tester
				pd proc=sm[1]->procdata;//copy data from sm[1]
				//now close sm[1]
                		if(up(&(sm[1]->semEmpty))<0){
                        		printf("Error while incrementing semaphore empty.\n");
					perror("");
                        		exit(1);
                		}
                		if(up(&(sm[1]->semSM))<0){
              				printf("Error while incrementing shared memory semaphore.\n");
					perror("");
                			exit(1);
                		}
				sleep(k);			//so it sleeps according to type of component
				//access sm[2]
		                if(down(&(sm[2]->semEmpty))<0){
		                        printf("Error while decrementing semaphore empty.\n");
					perror("");
		                        exit(1);
		                }
		                if(down(&(sm[2]->semSM))<0){
		                        printf("Error while decrementing shared memory semaphore.\n");
					perror("");
		                        exit(1);
				}
				//copy from data
				sm[2]->procdata=proc;            //pass data into shared memory

		                if(up(&(sm[2]->semFull))<0){
		                        printf("Error while incrementing semaphore full.\n");
					perror("");
		                        exit(1);
		                }
		                if(up(&(sm[2]->semSM))<0){
		                      printf("Error while incrementing shared memory semaphore.\n");
					perror("");
		                      exit(1);
		                }
				//shut sm[2]
				c++;		//now increase counter
			}
		}
		return 0;
		}
	}

        long double ts=0.0;		//ts=time spent in painter
	if((assembly=pids[7]=fork())<0){
                printf("Error while creating the assembly line.\n");
		perror("");
		return -1;
        }
        else if(assembly==0){
		long double travelTime=0.0;     //travelTime= time from earliest to latest component that create a product
		for(int b=0; b<3*y; b++){
		//access sm[2]
		if(down(&(sm[2]->semFull))<0){
                        printf("Error while decrementing semaphore full.\n");
			perror("");
                        exit(1);
                }
                if(down(&(sm[2]->semSM))<0){
                        printf("Error while decrementing shared memory semaphore.\n");
			perror("");
                        exit(1);
                }
		pd proc	=sm[2]->procdata;			//this needs to be passed onto the correct queue
		//copy data
		ts+=proc.timeSpent;				//add up the time all processes spend in the painter
		//shut sm[2]
		if(up(&(sm[2]->semEmpty))<0){
                        printf("Error while incrementing semaphore empty.\n");
			perror("");
                        exit(1);
                }
                if(up(&(sm[2]->semSM))<0){
                        printf("Error while incrementing shared memory semaphore.\n");
			perror("");
                        exit(1);
                }
		proc.finishTime=clock();			//finishing time of product


		if((proc.number)==0)enQueue(q0, proc);		//send each component to the correct queue
		else if((proc.number)==1)enQueue(q1, proc);
		else if((proc.number)==2)enQueue(q2, proc);
		//the component was pushed into the queue, time to check for components in all 3 queues
		while(q0->size>0 && q1->size>0 && q2->size>0){	//if the are all 3 components for a product
			qnode * qn0=deQueue(q0);	//pop them from queue to assemble
			qnode * qn1=deQueue(q1);
			qnode * qn2=deQueue(q2);
			long double impTime= findMin(qn0->proc.timestamp,qn1->proc.timestamp,qn2->proc.timestamp);	//earliest component production
			long double exitTime= findMax(qn0->proc.finishTime,qn1->proc.finishTime,qn2->proc.finishTime);	//latest component pop
			long long int finalid=((qn0->proc.id)* 1e+08) + ((qn1->proc.id)* 1e+04) +(qn2->proc.id);	//calculate product id
			printf("Product id: [%Ld]\n", finalid);
			travelTime+=((long double)(exitTime-impTime))/ (long double)(CLOCKS_PER_SEC / 1000);		//count for all components
		}
                if (b==(3*y-1)){        //final loop before exit
                        printf("Time spent inside painter on average: %Lf\n", ts/(3.0*y));
                }
		}
		printf("Average travel time: %Lf sec\n", travelTime/(3.0*y));		//average travel time
		return 0;
	}
	//time to exit

	int status;
	for(int i=0;i<8;i++){				//wait to exit
		waitpid(pids[i],&status,0);
		if(WIFEXITED(status)){
;//			printf("Process with pid %d exited normally\n",pids[i]);

		}
	}
	for(int j=0; j<3; j++){				//delete semaphores
		semctl(sm[j]->semEmpty, 0, IPC_RMID, 0);
                semctl(sm[j]->semFull, 0, IPC_RMID, 0);
                semctl(sm[j]->semSM, 0, IPC_RMID, 0);

		if(shmdt(sm[j])<0){									//detach shared memory segments
                	printf("Error while detaching shared memory segment.\n");
			perror("");
                	exit(-1);
        	}

		if(shmctl(smkey[j], IPC_RMID, (struct shmid_ds *) NULL)<0){
			printf("Error while deleting a shared memory segment.Sorry.\n");
			perror("");
	       	        exit(-1);
	       	}
	}

	free(q0);			//free allocated memory blocks
	free(q1);
	free(q2);

	exit(0);
}
