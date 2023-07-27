#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int main (char *argv[]){
    pid_t rec_pid;
    rec_pid = atoi(argv[1]);
    kill(rec_pid,SIGUSR1);
}