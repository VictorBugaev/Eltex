#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

int main (){
    int sig;
    pid_t pid;
    printf("Введите PID процесса-получателя\n-");
    scanf("%d",&pid);
    while(1){
        printf("Введите номер сигнала (0-выход)\n-");
        scanf("%d",&sig);
        printf("%d\n",sig);
        if(sig == 0){
            break;
        }
        else{
            kill(pid,sig);
        }
    }
}