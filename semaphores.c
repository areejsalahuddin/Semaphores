#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <time.h>
#include <unistd.h>

#define size 5 


int sharedCounter = 0;
sem_t counter_sem,buffer_sem,empty_sem,full_sem;
int buffer[size];



void* counterFunc(void* args){
int threadCount = *((int*) args);
while(1){

printf("Counter thread %d: recieved a message \n",threadCount);

printf("Counter thread %d is waiting to write\n",threadCount);

sem_wait(&counter_sem);
sharedCounter++;
printf("Counter thread %d: now adding to counter, counter value = %d\n",threadCount,sharedCounter); 
sem_post(&counter_sem);

int randnum = (random()% (15-1+1))+1; 
sleep(randnum);


}

}

void* monitorFunc(void* args){
int i=0;
while (1){

printf("Monitor thread: waiting to read counter\n");
sem_wait(&counter_sem);
printf("Monitor thread: reading a count value of %d\n",sharedCounter);
int tempCount= sharedCounter;
sharedCounter=0;
sem_post(&counter_sem);
// check if buffer is full 
int empty;
sem_getvalue(&empty_sem,&empty);
if(empty<=0){
printf("Monitor thread: buffer is full\n");
}
sem_wait(&empty_sem);

sem_wait(&buffer_sem);

buffer[i] = tempCount;

printf("Monitor thread: writing to buffer at position %d\n",i);

sem_post(&buffer_sem);

sem_post(&full_sem);

i++;
if(i>= size)
{
i=0;
}
int randnum = (random()% (15-1+1))+1;
sleep(randnum);

}
}

void* collectorFunc(void* args){

int i =0 ;
while(1){
// check if buffer is empty
int full;
sem_getvalue(&full_sem,&full);
if(full<=0){
printf("Collector thread: buffer is EMPTY\n");
}
sem_wait(&full_sem);
sem_wait(&buffer_sem);
printf("Collector thread: reading from buffer at position %d\n",i);
buffer[i]=0;
sem_post(&buffer_sem);
sem_post(&empty_sem);

i++;
if(i>= size)
{
i=0;
}

int randnum = (random()% (15-1+1))+1;
sleep(randnum);

}
}
void initializeSemaphores(){

sem_init(&counter_sem,0,1);
sem_init(&buffer_sem,0,1);
sem_init(&empty_sem,0,size);
sem_init(&full_sem,0,0);
}

int main(){
int n;
printf("Enter number of counters: ");
scanf("%d",&n);

initializeSemaphores();
pthread_t monitor_thread;
pthread_t counterthreads[n];
pthread_t collector_thread;
srandom(time(0));
int args[n];
int i;

for (i=0; i<n ; i++){
args[i]=i;
pthread_create(&counterthreads[i],NULL, &counterFunc, &args[i]);
}
pthread_create(&monitor_thread,NULL, &monitorFunc, NULL);

pthread_create(&collector_thread,NULL, &collectorFunc, NULL);

pthread_join(collector_thread,NULL);


return 0;
}

