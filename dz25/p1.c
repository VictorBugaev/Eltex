#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

#define SIZE 200

int main (){
    char memory[] = "/shm";
    char semaphore[] = "/sem";
    sem_t *sem;
    int fd, sem_fd, sem_val;
    char *mes;
    struct sembuf semaphoree;

    printf("P1 - start\n");
 
    if((fd = shm_open(memory, O_CREAT| O_RDWR, 0666)) == -1){
        perror("shm_open");
        return -1;
    }

    if((sem = sem_open(semaphore, O_CREAT, 0666, 1)) == SEM_FAILED){
        perror("sem_open");
        return -1;
    }       
    
    if(ftruncate(fd, SIZE)){
        perror("ftruncate");
        return -1;
    }

    if((mes = mmap(NULL, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == (char *)-1){
        perror("mmap");
        return -1;
    }


    printf("Enter the message: ");
    sem_wait(sem);
    fgets(mes, SIZE, stdin);
    sem_post(sem);
    printf("waiting for a message...\n");
    sleep(1);
    sem_wait(sem);
    printf("Message from the P2 - %s", mes);
    sem_post(sem);
    sleep(1);
    munmap(mes, SIZE);
    
    printf("P1 - end\n");
}
