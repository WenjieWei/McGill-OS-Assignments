#include <stdio.h>
#include <stdlib.h>

/*
Simulates resource requests by processes
*/
void request_simulator(int pr_id, int* request_vector){

}
/*
Implementation of isSafe() as described in the slides
*/
int isSafe(){

    int isSafe = 0;
    return isSafe;

}
/*
Implementation of Bankers Algorithm as described in the slides
returns 1 if safe allocation 0 if not safe
*/
int bankers_algorithm(int pr_id, int* request_vector){

    return 0;

}

/*
Simulates processes running on the system.

*/
void* process_simulator(void* pr_id){

}

/*
Simulates a fault occuring on the system.

*/
void* fault_simulator(void* pr_id){

}

/*
Checks for deadlock
*/
void* deadlock_checker(){
}

int main()
{

    //Initialize all inputs to banker's algorithm

    //create threads simulating processes (process_simulator)

    // create a thread that takes away resources from the available pool (fault_simulator)

    //create a thread to check for deadlock (deadlock_checker)

    return 0;
}
