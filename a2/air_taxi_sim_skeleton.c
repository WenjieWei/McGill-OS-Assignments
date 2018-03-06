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
//Name: Wenjie Wei
//McGill ID: 260685967

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>

int BUFFER_SIZE = 100; //size of queue

//declare semaphore and thread mutex
pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;
sem_t queueMutex;
sem_t queueFull;

struct arg_struct
{
    int *taxi_id;
    int *taxi_waiting;
};

// A structure to represent a queue
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int *array;
};

// function to create a queue of given capacity.
// It initializes size of queue as 0
struct Queue *createQueue(unsigned capacity)
{
    struct Queue *queue = (struct Queue *)malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1; // This is important, see the enqueue
    queue->array = (int *)malloc(queue->capacity * sizeof(int));
    return queue;
}

// Queue is full when size becomes equal to the capacity
int isFull(struct Queue *queue)
{
    return ((queue->size) >= queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue *queue)
{
    return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue *queue, int item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

// Function to remove an item from queue.
// It changes front and size
int dequeue(struct Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

// Function to get rear of queue
int rear(struct Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

void print(struct Queue *queue)
{
    if (queue->size == 0)
    {
        return;
    }

    for (int i = queue->front; i < queue->front + queue->size; i++)
    {

        printf(" Element at position %d is %d \n ", i % (queue->capacity), queue->array[i % (queue->capacity)]);
    }
}

struct Queue *queue;

/*Producer Function: Simulates an Airplane arriving and dumping 5-10 passengers to the taxi platform */
void *FnAirplane(void *pl_id)
{
    int cl_id = *(int *)pl_id; //cast the void pointer to an integer

    while (1)
    {
        int passengers_to_plat = (int)rand() % 6 + 5;
        printf("Airplane %d arrives with %d passengers\n", cl_id, passengers_to_plat);

        //the following few lines determines the passenger ID.
        int platform_id = 0;
        for (platform_id; platform_id < passengers_to_plat; platform_id++)
        {
            //there is a racing condition here?
            int passenger_id = 1000000;
            passenger_id += cl_id * 1000;
            passenger_id += platform_id;

            sem_wait(&queueMutex);
            if (!isFull(queue))
            {
                enqueue(queue, passenger_id);
                printf("Passenger %d of airplane %d arrives to platform.\n", passenger_id, cl_id);
                sem_post(&queueMutex);
                sem_post(&queueFull);
            }
            else
            {
                printf("The platform queue is full. Rest of passengers of plane %d take the bus.\n", cl_id);
                sem_post(&queueMutex);
                break;
            }
        }

        sleep(1); //repeat the same process in the following hour.
    }
}

/* Consumer Function: simulates a taxi that takes n time to take a passenger home and come back to the airport */
void *FnTaxi(void *arguments)
{
    struct arg_struct *args = arguments;

    int pr_id = *(int *)args->taxi_id; //convert the void pointer to an integer pointer
    int waiting_ptr = *(int *)  args->taxi_waiting;

    while (1)
    {
        sem_wait(&queueMutex);

        if(waiting_ptr == 0){
            printf("Taxi driver %d arrives\n", pr_id);
        }

        if(isEmpty(queue) && waiting_ptr){
            sem_post(&queueMutex);
        } else if(isEmpty(queue) && !waiting_ptr){
            waiting_ptr = 1;
            sem_post(&queueMutex);
            printf("Taxi driver %d waits for passengers to enter the platform\n", pr_id);
        }
        else{
            printf("Taxi driver %d picked up passenger %d from the platform\n", pr_id, dequeue(queue));
            sem_post(&queueMutex);

            int time_min = rand()% 11 + 20;
            int time_usec = time_min * 1000000/60;

            usleep(time_usec);
            waiting_ptr = 0;
        }
    }
}

int main(int argc, char *argv[])
{
    int num_airplanes;
    int num_taxis;

    num_airplanes = atoi(argv[1]);
    num_taxis = atoi(argv[2]);

    printf("You entered: %d airplanes per hour\n", num_airplanes);
    printf("You entered: %d taxis\n", num_taxis);

    //initialize queue
    queue = createQueue(BUFFER_SIZE);

    //seed the random number
    srand(time(NULL));

    //declare arrays of threads and initialize semaphore(s)
    sem_init(&queueMutex, 0, 1);
    sem_init(&queueFull, 0, 0);
    pthread_t taxiThreads[num_taxis];
    pthread_t airplaneThreads[num_airplanes];

    //create arrays of integer pointers to ids for taxi / airplane threads
    int *taxi_ids[num_taxis];
    int *airplane_ids[num_airplanes];
    int taxi_waiting[num_taxis];
    int *taxi_waiting_ptr[num_taxis];
    struct arg_struct args;

    //create threads for airplanes

    //Firstly lock all threads before every thread is created.
    pthread_mutex_lock(&threadMutex);
    int i = 0;
    for (i; i < num_airplanes; i++)
    {
        airplane_ids[i] = malloc(sizeof(int));
        *airplane_ids[i] = i;
        if (pthread_create(&airplaneThreads[i], NULL, FnAirplane, airplane_ids[i]))
        {
            fprintf(stderr, "Failed to create thread.\n");
            return 1;
        }
        else
        {
            printf("Creating airplane thread %d\n", i);
        }
    }

    //create threads for taxis
    int j = 0;
    for (j; j < num_taxis; j++)
    {
        taxi_ids[j] = malloc(sizeof(int));
        *taxi_ids[j] = j;
        taxi_waiting[j] = 0;
        taxi_waiting_ptr[j] = malloc(sizeof(int));
        *taxi_waiting_ptr[j] = taxi_waiting[j];

        args.taxi_id = taxi_ids[j];
        args.taxi_waiting = taxi_waiting_ptr[j];
        if (pthread_create(&taxiThreads[j], NULL, FnTaxi, (void *)&args))
        {
            fprintf(stderr, "Failed to create thread.\n");
            return 1;
        }
        else
        {
        }
    }
    //all created. unlock all the threads now.
    pthread_mutex_unlock(&threadMutex);
    sleep(10);

    //all threads have finished running,
    //shut down all the threads
    pthread_mutex_lock(&threadMutex);

    for (i = 0; i < num_airplanes; i++)
    {
        pthread_cancel(airplaneThreads[i]);
    }

    for (j = 0; j < num_taxis; j++)
    {
        pthread_cancel(taxiThreads[j]);
    }

    sem_destroy(&queueMutex);
    sem_destroy(&queueFull);
    pthread_mutex_unlock(&threadMutex);

    pthread_exit(NULL);
}