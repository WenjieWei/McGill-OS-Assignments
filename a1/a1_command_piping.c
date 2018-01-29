/*
----------------- COMP 310/ECSE 427 Winter 2018 -----------------
I declare that the awesomeness below is a genuine piece of work
and falls under the McGill code of conduct, to the best of my knowledge.
-----------------------------------------------------------------
*/ 

//Name: Wenjie Wei
//McGill ID: 260685967


#include <stdio.h>
#include <unistd.h>

int main(){
    int pfildes[2];
    char *args[2];
    pipe(pfildes);

    int pid = fork();

    if(pid == 0){//this is the child process
        close(pfildes[0]);      //close the reading end in child

        dup2(pfildes[1], 1);     //send the stdout to the pipe
        close(pfildes[1]);

        args[0] = "ls";
        args[1] = NULL;
        execvp(args[0], args);   
    } else {    //parent process
        char buffer[1024];

        close(pfildes[1]);  //close the write end of the pipe in parent
        read(pfildes[0], buffer, sizeof(buffer));
        printf("%s", buffer);

        return 0;
    }
}