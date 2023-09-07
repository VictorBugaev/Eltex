#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <semaphore.h>

#define SIZE 100

int main(){
    char memory[] = "/shm";
    char semaphore[] = "/sem";
    int fd;
    char *mes;
    char buf[SIZE];
    sem_t *sem;
    
    printf("P2 - start\n");
    
    if((sem = sem_open(semaphore, 0)) == SEM_FAILED){;
        perror("sem_open");
        return -1;
    }
    
    if((fd = shm_open(memory, O_RDWR, 0666)) == -1){
        perror("shm_open");
        return -1;
    }
    
    if(ftruncate(fd, SIZE) == -1){
        perror("ftruncate");
        return -1;
    }
    
    if((mes = mmap(NULL, SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0)) == (char*)-1){
        perror("mmap");
        return -1;
    }
    
    printf("waiting for a message...\n");
    sem_wait(sem);
    strcpy(buf, mes);
    printf("Message from the P1 - %s", buf);
    printf("Enter the message: ");
    fgets(mes, SIZE, stdin);
    sem_post(sem);
    
    if(shm_unlink(memory) == -1){
        perror("shm_unlink");
        return -1;
    }

    if(sem_close(sem) == -1){
        perror ("sem_close");
        return -1;
    }

    if(sem_unlink(semaphore) == -1){
        perror("sem_unlink");
        return -1;
    }
    
    printf("P2 - end\n");
    return 0;
}

