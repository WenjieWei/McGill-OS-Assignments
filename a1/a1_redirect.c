

/**
 *  This is the code of redirection.
 *  Author: Wenjie Wei, McGILL ID: 260685967
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
    int pid, fildes;
    printf("First: Print to stdout\n");

    int defout = dup(1);
    fildes = open("redirect_out.txt", O_RDWR | O_CREAT);
    dup2(fildes, 1); //redirect output to the file
    pid = fork();
    if (pid == 0){  //this is the child
        close(fildes);
        close(defout);
        printf("Second: Print to redirect_out.txt\n");
        //exit();
    } else {
        dup2(defout, 1);
        close(fildes);
        close(defout);
        waitpid(pid, 0, 0); //wait until child finishes
        printf("Third: Print to stdout");
    }

    return 0;
}