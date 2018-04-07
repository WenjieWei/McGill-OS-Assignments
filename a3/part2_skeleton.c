/*
----------------- COMP 310/ECSE 427 Winter 2018 -----------------
I declare that the awesomeness below is a genuine piece of work
and falls under the McGill code of conduct, to the best of my knowledge.
-----------------------------------------------------------------
*/

//Please enter your name and McGill ID below
//Name: Wenjie Wei
//McGill ID: 260685967

#include <stdio.h>  //for printf and scanf
#include <stdlib.h> //for malloc

#define LOW 0
#define HIGH 199
#define START 53

//compare function for qsort
//you might have to sort the request array
//use the qsort function
// an argument to qsort function is a function that compares 2 quantities
//use this there.
int cmpfunc(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

//function to swap 2 integers
void swap(int *a, int *b)
{
    if (*a != *b)
    {
        *a = (*a ^ *b);
        *b = (*a ^ *b);
        *a = (*a ^ *b);
        return;
    }
}

//Prints the sequence and the performance metric
void printSeqNPerformance(int *request, int numRequest)
{
    int i, last, acc = 0;
    last = START;
    printf("\n");
    printf("%d", START);
    for (i = 0; i < numRequest; i++)
    {
        printf(" -> %d", request[i]);
        acc += abs(last - request[i]);
        last = request[i];
    }
    printf("\nPerformance : %d\n", acc);
    return;
}

//access the disk location in FCFS
void accessFCFS(int *request, int numRequest)
{
    //simplest part of assignment
    printf("\n----------------\n");
    printf("FCFS :");
    printSeqNPerformance(request, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SSTF
void accessSSTF(int *request, int numRequest)
{
    //write your logic here
    //define a new array of integers storing the sequence of the outputs
    int *sequence, i, j, index, finish, minDiff, CURR, nearest, remaining;
    sequence = malloc(numRequest * sizeof(int));

    //flag to indicate if the process is finished
    finish = 0;

    //initialize all variables defined
    index = 0;
    CURR = START;
    remaining = numRequest;
    j = 0;

    while (!finish)
    {
        minDiff = HIGH;
        for (i = 0; i < numRequest; i++)
        {
            if (request[i] >= 0 && abs(CURR - request[i]) < minDiff)
            {
                minDiff = CURR - request[i];
                nearest = request[i];
                index = i;
            }
        }
        request[index] = -1;
        remaining--;

        sequence[j] = nearest;
        j++;

        if (remaining == 0)
        {
            finish = 1;
        }
    }

    printf("\n----------------\n");
    printf("SSTF :");
    printSeqNPerformance(sequence, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SCAN
void accessSCAN(int *request, int numRequest)
{
    //write your logic heree
    //first sort the request array
    qsort(request, numRequest, sizeof(int), cmpfunc);

    //define a new array of int to store the new request sequence
    int *sequence, twdLow, finish, i, j, remaining, index;
    sequence = malloc(numRequest * sizeof(int));

    //flag to indicate if the entire process is finished
    finish = 0;
    //flag to indicate which direction the head is moving
    if (START <= (HIGH / 2))
        twdLow = 1;
    else if (START == LOW)
        twdLow = 0;
    else if(START == HIGH)
        twdLow = 1;
    else
        twdLow = 0;

    //iterate through the array to find where START is sitting
    //and determine which track prints first.
    if (START <= request[0])
        i = 0;
    else if (START >= request[numRequest - 1])
        i = numRequest - 1;
    else
    {
        for (i = 0; i < numRequest; i++)
        {
            if (request[i] <= START && request[i + 1] > START)
                break;
        }
    }

    //initialize variables
    remaining = numRequest;
    j = 0;

    //now i is the index where the number <= START is sitting at.
    //and i+1 is the index where the smallest number that is greater than START is sitting at.
    while (!finish)
    {
        if (!twdLow)
        {
            index = i + 1;
            for(index; index < numRequest; index++){
                sequence[j] = request[index];
                request[index] = -1;
                j++;
            }
            twdLow = 1;
        }
        else
        {
            index = i;
            for(index; index >= 0; index--){
                sequence[j] = request[index];
                request[index] = -1;
                j++;
            }
            twdLow = 0;
        }

        if (j == (numRequest))
            finish = 1;
    }

    printf("\n----------------\n");
    printf("SCAN :");
    printSeqNPerformance(sequence, numRequest);
    printf("----------------\n");
    return;
}

// //access the disk location in CSCAN
// void accessCSCAN(int *request, int numRequest)
// {
//     //write your logic here
//     printf("\n----------------\n");
//     printf("CSCAN :");
//     printSeqNPerformance(newRequest, newCnt);
//     printf("----------------\n");
//     return;
// }

// //access the disk location in LOOK
// void accessLOOK(int *request, int numRequest)
// {
//     //write your logic here
//     printf("\n----------------\n");
//     printf("LOOK :");
//     printSeqNPerformance(newRequest, newCnt);
//     printf("----------------\n");
//     return;
// }

// //access the disk location in CLOOK
// void accessCLOOK(int *request, int numRequest)
// {
//     //write your logic here
//     printf("\n----------------\n");
//     printf("CLOOK :");
//     printSeqNPerformance(newRequest, newCnt);
//     printf("----------------\n");
//     return;
// }

int main()
{
    int *request, numRequest, i, ans;

    //allocate memory to store requests
    printf("Enter the number of disk access requests : ");
    scanf("%d", &numRequest);
    request = malloc(numRequest * sizeof(int));

    printf("Enter the requests ranging between %d and %d\n", LOW, HIGH);
    for (i = 0; i < numRequest; i++)
    {
        scanf("%d", &request[i]);
    }

    printf("\nSelect the policy : \n");
    printf("----------------\n");
    printf("1\t FCFS\n");
    printf("2\t SSTF\n");
    printf("3\t SCAN\n");
    printf("4\t CSCAN\n");
    printf("5\t LOOK\n");
    printf("6\t CLOOK\n");
    printf("----------------\n");
    scanf("%d", &ans);

    switch (ans)
    {
    //access the disk location in FCFS
    case 1:
        accessFCFS(request, numRequest);
        break;

    //access the disk location in SSTF
    case 2:
        accessSSTF(request, numRequest);
        break;

    //access the disk location in SCAN
    case 3:
        accessSCAN(request, numRequest);
        break;

        //     //access the disk location in CSCAN
        // case 4:
        //     accessCSCAN(request, numRequest);
        //     break;

        // //access the disk location in LOOK
        // case 5:
        //     accessLOOK(request, numRequest);
        //     break;

        // //access the disk location in CLOOK
        // case 6:
        //     accessCLOOK(request, numRequest);
        //     break;

    default:
        break;
    }
    return 0;
}
