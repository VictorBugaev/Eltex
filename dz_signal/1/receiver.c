#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void signal_hendler(int signo){
    if (signo == SIGUSR1){
        printf ("Получен сигнал SIGUSR1\n");
    }
}

int main (){
    printf("PID процесса получаетля - %d\n", getpid());
    signal(SIGUSR1,signal_hendler);
    while (1)
    {
        sleep(1);
    }
    
}