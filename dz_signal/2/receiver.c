#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(){
    printf("PID процесса приёма = %d\n", getpid());
    sigset_t signalSet;
    if(sigemptyset(&signalSet)){
        perror("sigemptyset");
        return -1;
    }
    if(sigfillset(&signalSet)){
        perror("sigfillset");
        return -1;
    }
    int signal;
    while(1){
        sigwait(&signalSet, &signal);
        printf("Сигнал = %d\n", signal);
    }
    printf("End\n");
    return 0;
}
