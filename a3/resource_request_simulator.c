#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>

//declare thread mutex
pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;

void *request_resources(void *pr_id){

}

int main()
{
    //request user inputs for number of processes and resources
    int numProcesses, numResources, i, j;

    printf("Enter the number of processes.\n");
    scanf("%d", &numProcesses);
    printf("Enter the number of resources.\n");
    scanf("%d", &numResources);

    //initialize all the resources
    int *resources;
    resources = malloc(numResources * sizeof(int));

    printf("Enter the units of each resource available in the system\n");
    for (i = 0; i < numResources; i++)
    {
        scanf("%d", &resources[i]);
    }

    //initialize the 2-D array of maximum requests
    int *maxRequests;
    maxRequests = malloc(numProcesses * numResources * sizeof(int));

    printf("Enter maximum resources each process can claim:\n");
    printf("Press Enter to finish inputting the requested %d resources for each process.\n", numResources);
    for (i = 0; i < numProcesses; i++)
    {
        for (j = 0; j < numResources; j++)
        {
            scanf("%d", &maxRequests[i+j]);
        }
    }

    //initialize allocation table
    int *allocated;
    allocated = malloc(numProcesses * numResources * sizeof(int));
    for (i = 0; i < numProcesses; i++)
    {
        for (j = 0; j < numResources; j++)
        {
            allocated[i+j] = 0;
        }
    }
    
    //test the intialized values
    //print the resources available array
    printf("The available Resources Array is: ");
    for(i=0; i< (sizeof(resources) / sizeof(int)); i++){
        printf("%d ", resources[i]);
    }
    printf("\n");

    //print the allocated table
    for (i = 0; i < numProcesses; i++)
    {
        for (j = 0; j < numResources; j++)
        {
            printf("%d ", allocated[i+j]);
        }
        printf("\n");
    }

    //print the table of maximum requests
    printf("The maximum claim table is:\n");
    for (i = 0; i < numProcesses; i++)
    {
        for (j = 0; j < numResources; j++)
        {
            printf("%d ", maxRequests[i+j]);
        }
        printf("\n");
    }

    //print the array of resources available
    printf("The available resources array is:");
    for(i = 0; i<numResources; i++){
        printf("%d ", resources[i]);
    }
    printf("\n");

    //initalize all threads to simulate the processes
    pthread_t procSim[numProcesses];

    //create arrays of integer pointers to ids of threads
    //who simulate the processes
    int *pr_id;
    pr_id = malloc(numProcesses * sizeof(int));

    //create all the threads for simulation
    pthread_mutex_lock(&threadMutex);
    for(i = 0; i<numProcesses; i++){
        pr_id[i] = i;
        if(pthread_create(&pr_id[i], NULL, request_resources; pr_id[i])){
            fprintf(stderr, "Failed to create thread.\n");
            return 1;
        } else {
        }
    }
    

    return 0;
}