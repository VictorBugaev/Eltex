#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>


#define SIZE 200

int main(){
    char memory[] = ".";
    char semaphore[] = "p1";
    char * mes;
    key_t key, sem_key;
    int sem_fd, fd, val;
    struct sembuf semb;

    printf("P1 - start\n");

    if((key = ftok(memory, 'A')) == -1){
        perror("ftok");
        return -1;
    }

    printf("\tKey - %d\n", key);

    if((fd = shmget(key, SIZE, IPC_CREAT | 0666)) == -1){
        perror("shmget");
        return -1;
    }

    if((mes = (char *)shmat(fd, NULL, 0)) == (char *)-1){
        perror("shmat");
        return -1;
    }

    if((sem_key = ftok(semaphore, 'B')) == -1){
        perror("ftok");
        return -1;
    }

    printf("\tSem_key - %d\n", sem_key);

    if((sem_fd = semget(sem_key, SIZE, IPC_CREAT | 0666)) == -1){
        perror("semget");
        return -1;
    }

    semb.sem_flg = 0;
    semb.sem_num = 0;
    semb.sem_op = 167;
    
    val = semctl(sem_fd, 0, GETVAL, 0);

    printf("\tSemaphore - %d\n", val);

    printf("\t\tEnter the message: ");
    fgets(mes, SIZE, stdin);
    
    semop(sem_fd, &semb, 1);
    val = semctl(sem_fd, 0, GETVAL, 0);
    printf("\tSemaphore - %d\n", val);
    semb.sem_op *= -1;
    printf("\t\tWaiting for a message from P2 ...\n");
    semop(sem_fd, &semb, 1);
    printf("\t\tMessage from the P2 - %s", mes);

    if(shmdt(mes) == -1){
        perror("shmdt");
        return -1;
    }

    if(shmctl(fd, IPC_RMID, 0) == -1){
        perror("shmctl");
        return -1;
    }

    if(semctl(sem_fd, 0, IPC_RMID, 0) == -1){
        perror("semctl");
        return -1;
    }

    printf("P1 - end\n");
}
