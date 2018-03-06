/*
 ----------------- COMP 310/ECSE 427 Winter 2018 -----------------
 Dimitri Gallos
 Assignment 2 skeleton
 
 -----------------------------------------------------------------
 I declare that the awesomeness below is a genuine piece of work
 and falls under the McGill code of conduct, to the best of my knowledge.
 -----------------------------------------------------------------
 */

//Please enter your name and McGill ID below
//Name: Jiawei Ni
//McGill ID: 260615965

 

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>
#include <time.h>


int BUFFER_SIZE = 100; //size of queue
sem_t queueAccess; //===========================
sem_t queueNotEmpty;
pthread_mutex_t threadOperation = PTHREAD_MUTEX_INITIALIZER;


// A structure to represent a queue
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int* array;
};
 
// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}
 
// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue* queue)
{
    return ((queue->size ) >= queue->capacity);
}
 
// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}
 
// Function to add an item to the queue.  
// It changes rear and size
void enqueue(struct Queue* queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue. 
// It changes front and size
int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}
 
// Function to get rear of queue
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

void print(struct Queue* queue){
    if (queue->size == 0){
        return;
    }
    
    for (int i = queue->front; i < queue->front +queue->size; i++){
        
        printf(" Element at position %d is %d \n ", i % (queue->capacity ), queue->array[i % (queue->capacity)]);
    }
    
}

struct Queue* queue;

/*Producer Function: Simulates an Airplane arriving and dumping 5-10 passengers to the taxi platform */
void *FnAirplane(void* cl_id)
{   
    
    int thisPlaneId=*(int*)cl_id;
    while(1){
        int passengerNum = 5+rand()%6;
        printf("Airplane %d arrives with %d passengers\n",thisPlaneId,passengerNum); // airplane arrives
        
        for(int i=0;i < passengerNum;i++){ //each passenger trys to enter queue individually
            int passengerId =1000000+1000*thisPlaneId+i;

            sem_wait(&queueAccess);
            if(isFull(queue)){ //if queue is full
                printf("Platform is full: Rest of passengers of plane %d take the bus\n",thisPlaneId);
                sem_post(&queueAccess);
                break; //remove all other passengers on this plane
                
            }else{  //if not full
                printf("Passenger %d of airplane %d arrives to platform\n",passengerId,thisPlaneId);
                enqueue(queue, passengerId); //passenger enters queue
                sem_post(&queueNotEmpty);
                sem_post(&queueAccess); 
            }
            
        }
        
        sleep(1);//this plane will arrive again after 1 hour
    }

    
}

/* Consumer Function: simulates a taxi that takes n time to take a passenger home and come back to the airport */
void *FnTaxi(void* pr_id)
{
    
    int thisTaxiId=*(int*)pr_id;
    enum taxiWaitingState{JUST_ARRIVED,WAITING} thisTaxi=JUST_ARRIVED;
    
    //printf("Taxi driver %d arrives\n",thisTaxiId);
    while(1){
        sem_wait(&queueAccess);

        if(thisTaxi==JUST_ARRIVED){ //print when just arrived
            printf("Taxi driver %d arrives\n",thisTaxiId); 
        }

        
        if(isEmpty(queue)&&thisTaxi==WAITING){ //don`t print message when waiting
            sem_post(&queueAccess);
            sem_wait(&queueNotEmpty);

        }else if(isEmpty(queue)&&thisTaxi==JUST_ARRIVED){ //print message when waiting starts
            
            printf("Taxi driver %d waits for passengers to enter the platform\n",thisTaxiId);
            sem_post(&queueAccess);
            thisTaxi=WAITING;

        }else{ // pick up passenger, print message
            int pickedUpPassenger=front(queue);
            dequeue(queue);
            printf("Taxi driver %d picked up client %d from the platform\n",thisTaxiId,pickedUpPassenger);
            sem_post(&queueAccess);
            int taxiTravelTime=10+rand()%21; //travel time takes 10-30 mins
            usleep(taxiTravelTime*1000000/60);// sleep for (t min)*(1000000us/h)*(1/60hour/min)
            thisTaxi=JUST_ARRIVED;
            
        }

    }

}
 
int main(int argc, char *argv[])
{

  int num_airplanes;
  int num_taxis;

  num_airplanes=atoi(argv[1]);
  num_taxis=atoi(argv[2]);
  
  printf("You entered: %d airplanes per hour\n",num_airplanes);
  printf("You entered: %d taxis\n", num_taxis);
  
  srand(time(NULL));
 
  
  //initialize queue
  queue = createQueue(BUFFER_SIZE);
  
  //declare arrays of threads and initialize semaphore(s)
    pthread_t taxiThreadId[num_taxis];
    pthread_t planeThreadId[num_airplanes];

    sem_init(&queueAccess,0,1);
    sem_init(&queueNotEmpty,0,0);


  //create arrays of integer pointers to ids for taxi / airplane threads
    int *taxi_ids[num_taxis];
    int *airplane_ids[num_airplanes];
    
  //create threads for airplanes
  pthread_mutex_lock(&threadOperation); //lock threads before all airplane/taxi threads created
    for(int i=0; i<num_airplanes; i++){
        airplane_ids[i] = malloc(sizeof(int));
        *airplane_ids[i]=i;
        printf("Creating airplane thread %d \n",i);
        pthread_create(&planeThreadId[i],NULL,FnAirplane,airplane_ids[i]);
    }
  //create threads for taxis
    for(int i=0; i<num_taxis; i++){
        taxi_ids[i] = malloc(sizeof(int));
        *taxi_ids[i]=i;
        //printf("Creating taxi thread %d \n",i);
        pthread_create(&taxiThreadId[i],NULL,FnTaxi,taxi_ids[i]);
    }
    pthread_mutex_unlock(&threadOperation);

    sleep(10);// run simulator for 10 hours(sec)

    //close airplane threads and taxi threads
    pthread_mutex_lock(&threadOperation);
    for(int i=0; i<num_airplanes; i++){
        //printf("Closing airplane thread %d \n",i);
        pthread_cancel(planeThreadId[i]);
    }
    for(int i=0; i<num_taxis; i++){
        //printf("Closing taxi thread %d \n",i);
        pthread_cancel(taxiThreadId[i]);
    }
    sem_destroy(&queueAccess);
    sem_destroy(&queueNotEmpty);
    pthread_mutex_unlock(&threadOperation);
    
    pthread_exit(NULL);
}