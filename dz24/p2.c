#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SIZE 100

int main(){
    char memory[] = ".";
    char semaphore[] = "p1";
    key_t key, sem_key;
    int fd, sem_fd, val;
    char * mes;
    struct sembuf semb;
    
    printf("P2 - start\n");

    if((key = ftok(memory, 'A')) == -1){
        perror("ftok");
        return -1;
    }

    printf("\tKey - %d\n", key);

    if((fd = shmget(key, SIZE, 0666)) == -1){
        perror("semget");
        return -1;
    }

    if((mes = shmat(fd, NULL, 0)) == (char *)-1){
        perror("shmat");
        return -1;
    }

    if((sem_key = ftok(semaphore, 'B')) == -1){
        perror("ftok");
        return -1;
    }

    printf("\tSem_key - %d\n", sem_key);

    if((sem_fd = semget(sem_key, 1, 0666)) == -1){
        perror("semget");
        return -1;
    }

    semb.sem_flg = 0;
    semb.sem_num = 0;
    semb.sem_op = -167;

    if((val = semctl(sem_fd, 0, GETVAL, 0)) == -1){
        perror("semctl");
        return -1;
    }

    printf("\tSemaphore - %d\n", val);

    printf("\t\tWaiting for a message from P1 ...\n");
    semop(sem_fd, &semb, 1);

    printf("\t\tMessage from the P1 - %s", mes);

      if((val = semctl(sem_fd, 0, GETVAL, 0)) == -1){
        perror("semctl");
        return -1;
    }

    printf("\tSemaphore - %d\n", val);

    printf("\t\tEnter the message - ");
    fgets(mes, SIZE, stdin);

    semb.sem_op *= -1;
    semop(sem_fd, &semb, 1);

    if(shmdt(mes) == -1){
        perror("shmdt");
        return -1;
    }

    printf("P2 - end\n");
    return 0;
}
