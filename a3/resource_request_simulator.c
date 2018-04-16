/*
----------------- COMP 310/ECSE 427 Winter 2018 -----------------
I declare that the awesomeness below is a genuine piece of work
and falls under the McGill code of conduct, to the best of my knowledge.
-----------------------------------------------------------------
*/

//Please enter your name and McGill ID below
//Name: Wenjie Wei
//McGill ID: 260685967


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>

//declare thread mutex
pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;
sem_t allocateMutex;
int numProcesses, numResources, *resources, **maxRequests, **allocated, **need;

int isSafe(int pr_id)
{
    int isSafe = 1;
    int *work, *finish;

    int i, j;
    work = malloc(numResources * sizeof(int));
    finish = malloc(numProcesses * sizeof(int));

    //step 1: initialization work & finish
    for (j = 0; j < numResources; j++)
    {
        work[j] = resources[j];
    }
    for (i = 0; i < numProcesses; i++)
    {
        finish[i] = 0;
    }

    for (i = 0; i < numProcesses; i++)
    {
        for (j = 0; j < numResources; j++)
        {
            //step 2
            if (finish[i] == 0 && need[pr_id][j] <= work[j])
            {
                //step 3
                work[j] += allocated[pr_id][j];
                finish[i] = 1;

                //loop check every threads starting from the beginning
                i = -1;
                break;
            }
        }
    }
    //finish of the NEED matrix, no such process. Proceed to step 4

    //step 4
    for (i = 0; i < numProcesses; i++)
    {
        if (finish[i] == 0)
        {
            isSafe = 0;
        }
    }
    return isSafe;
}

//Simulating resource allocation
//return: 1 on success
//0 on failure
//-1 on error
int request_resources(int pr_id, int *request)
{
    int i, j;
    //now run the banker's algorithm

    //start with the verification step
    //step 1
    //printf("Process %d is executing verification step.\n", pr_id);
    for (j = 0; j < numResources; j++)
    {
        //error, abort the job
        //return -1 to indicate error
        if (request[j] > need[pr_id][j])
            return -1;

        //step 2
        if (request[j] > resources[j])
        {
            //Process requires more resources
            //failure. Wait
            return 0;
        }
    }

    //step 3, provisional allocation
    //printf("Process %d is doing provisional allocation.\n", pr_id);
    pthread_mutex_lock(&threadMutex);
    for (j = 0; j < numResources; j++)
    {
        resources[j] = resources[j] - request[j];
        allocated[pr_id][j] = allocated[pr_id][j] + request[j];
        need[pr_id][j] = need[pr_id][j] - request[j];
    }
    pthread_mutex_unlock(&threadMutex);

    if (!isSafe(pr_id))
    {
        //printf("Provisional allocation is not safe. Reverting.\n");
        //unsafe to allocate resources
        //revert all the provisional allocation
        pthread_mutex_lock(&threadMutex);
        for (j = 0; j < numResources; j++)
        {
            resources[j] = resources[j] + request[j];
            allocated[pr_id][j] = allocated[pr_id][j] - request[j];
            need[pr_id][j] = need[pr_id][j] + request[j];
        }
        pthread_mutex_unlock(&threadMutex);
        return 0;
    }
    else
    {
        //safe to allocate resources.
        //sleep the thread for 3s before proceeding
        //return 1.
        printf("Resources allocated safely.\n");
        sleep(3);
        return 1;
    }
}

void *bankers(void *pros_id)
{
    int pr_id = *(int *)pros_id;

    int *request, i, j, pros_finished;
    request = malloc(numResources * sizeof(int));

    pros_finished = 0;
    //generate random request array
    while (!pros_finished)
    {
        printf("The resource request vector is: ");
        for (j = 0; j < numResources; j++)
        {
            request[j] = rand() % (need[pr_id][j] + 1) + 0;
            printf("%d ", request[j]);
        }

        printf("\n");

        int request_resources_result = request_resources(pr_id, request);
        //a simple busy waiting if allocation is not safe
        while (request_resources_result != 1)
        {
            if (request_resources_result == -1)
            {
                printf("An error has occured: \n");
                printf("Process requesting more resources than needed.\n");
                return NULL;
            }
            request_resources_result = request_resources(pr_id, request);
        }

        //check if there are remaining processes that haven't been allocated resources
        for (j = 0; j < numResources; j++)
        {
            if (need[pr_id][j] != 0)
                pros_finished = 0;
            else
                pros_finished = 1;
        }

        if (!pros_finished)
        {
            sleep(3);
        }
    }

    //process has finished executing
    //relinquish the resources it holds
    printf("Process finished executing. Relinquishing resources\n");
    pthread_mutex_lock(&threadMutex);
    for (j = 0; j < numResources; j++)
    {
        resources[j] += allocated[pr_id][j];
    }
    pthread_mutex_unlock(&threadMutex);

    return NULL;
}

int main()
{
    //request user inputs for number of processes and resources
    int i, j;

    printf("Enter the number of processes.\n");
    scanf("%d", &numProcesses);
    printf("Enter the number of resources.\n");
    scanf("%d", &numResources);

    //initialize all the resources
    resources = malloc(numResources * sizeof(int));

    printf("Enter the units of each resource available in the system\n");
    for (i = 0; i < numResources; i++)
    {
        scanf("%d", &resources[i]);
    }

    //initialize the 2-D array of maximum requests
    maxRequests = malloc(numProcesses * sizeof(int *));

    printf("Enter maximum resources each process can claim:\n");
    //printf("Press Enter to finish inputting the requested %d resources for each process.\n", numResources);
    for (i = 0; i < numProcesses; i++)
    {
        maxRequests[i] = malloc(numResources * sizeof(int));
    }

    for (i = 0; i < numProcesses; i++)
    {
        for (j = 0; j < numResources; j++)
        {
            scanf("%d", &maxRequests[i][j]);
        }
    }

    //initialize allocation table
    allocated = malloc(numProcesses * sizeof(int *));
    for (i = 0; i < numProcesses; i++)
    {
        allocated[i] = malloc(numResources * sizeof(int));
    }
    for (i = 0; i < numProcesses; i++)
    {
        for (j = 0; j < numResources; j++)
        {
            allocated[i][j] = 0;
        }
    }

    //initialize the need table
    need = malloc(numProcesses * sizeof(int *));
    for (i = 0; i < numProcesses; i++)
    {
        need[i] = malloc(numResources * sizeof(int));
    }
    for (i = 0; i < numProcesses; i++)
    {
        for (j = 0; j < numResources; j++)
        {
            need[i][j] = maxRequests[i][j] - allocated[i][j];
        }
    }
    //test the intialized values
    //print the resources available array
    printf("The available Resources Array is: ");
    for (i = 0; i < (sizeof(resources) / sizeof(int)); i++)
    {
        printf("%d ", resources[i]);
    }
    printf("\n");

    //print the allocated table
    printf("The allocated table is: \n");
    for (i = 0; i < numProcesses; i++)
    {
        for (j = 0; j < numResources; j++)
        {
            printf("%d ", allocated[i][j]);
        }
        printf("\n");
    }

    //print the table of maximum requests
    printf("The maximum claim table is:\n");
    for (i = 0; i < numProcesses; i++)
    {
        for (j = 0; j < numResources; j++)
        {
            printf("%d ", maxRequests[i][j]);
        }
        printf("\n");
    }

    //print the array of resources available
    printf("The available resources array is:");
    for (i = 0; i < numResources; i++)
    {
        printf("%d ", resources[i]);
    }
    printf("\n");

    //seed the random number
    srand(time(NULL));

    //initalize all threads to simulate the processes
    pthread_t pros_threads[numProcesses];

    //create arrays of integer pointers to ids of threads
    //who simulate the processes
    int *pros_id[numProcesses];

    for (i = 0; i < numProcesses; i++)
    {
        pros_id[i] = malloc(sizeof(int));
        *pros_id[i] = i;
        if (pthread_create(&pros_threads[i], NULL, bankers, pros_id[i]))
        {
            fprintf(stderr, "Failed to create thread.\n");
            return 1;
        }
        else
        {
        }
    }

    pthread_exit(NULL);
    return 0;
}
