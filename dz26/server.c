#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <pthread.h>

#include "info.h"

int count_user = 1;
char *ptr;
int fd_sem;
int fd;
int exit_status = 0;
struct sembuf sem_lock = {0, -10, 0}, sem_unlock = {0, 10, 0};

void *thread_message_processing(void *args){
    printf("start message processing\n");
    int st_read;
    int cr_users;
    int user;
    char suser[4];
    while(1){
        semop(fd_sem, &sem_lock, 1);
        if(exit_status){
            semop(fd_sem, &sem_unlock, 1);
            break;
        }
        sscanf(ptr + POS_LIST_USER + POS_SR, "%2d", &st_read);
        if(st_read == 0){
            semop(fd_sem, &sem_unlock, 1);
            continue;
        }
        strncpy(suser, ptr + 4, 4);
        sscanf(suser, "%4d", &user);
        if(user == 0){
            semop(fd_sem, &sem_unlock, 1);
            break;
        }
        for(int i = 0; i < SIZE_LOGIN; ++i){
            printf("%c", ptr[POS_LIST_USER + (SIZE_CELL * user) + i]);
        }
        printf(": %s\n",ptr + 8);
        sscanf(ptr + POS_ITER_USER, "%4d", &cr_users);
        --cr_users;
        sprintf(ptr + POS_ITER_USER, "%4d", cr_users);
        if(cr_users > 0){
            semop(fd_sem, &sem_unlock, 1);
        }
    }
    return NULL;
}

void replace_enter(char *str){
    int i = 0;
    while('\0' != str[i]){
        if(str[i] == '\n'){
            str[i] = '\0';
            break;
        }
        ++i;
    }
}

void init_server(){
    char login[SIZE_LOGIN];
    memset(login, 0, SIZE_LOGIN);
    strcpy(login, "SERVER");
    for(int i = 0; i < SIZE_LOGIN; ++i){
        ptr[POS_LIST_USER + i] = login[i];
    }
    sprintf(ptr + POS_LIST_USER + SIZE_LOGIN, MASK, 1, 1, 0);
}

int init_memory_and_semaphore(char *filename_mem, char *filename_sem){
    key_t fd_key = ftok(filename_mem, KEYMEM);
    if(fd_key < 0){
        perror("ftok mem");
        return -1;
    }
    printf("fd_key mem = 0x%x\n", fd_key);
    fd = shmget(fd_key, SIZE, IPC_CREAT | 0666);
    if(fd < 0){
        printf("errno = %d\n", errno);
        perror("shmget");
        return -1;
    }
    ptr = (char*)shmat(fd, NULL, 0);
    if(ptr == (char*)-1){
        printf("errno = %d\n", errno);
        perror("shmat");
        return -1;
    }
    printf("Init memory\n");
    key_t fd_key_sem = ftok(filename_sem, KEYSEM);
    if(fd_key_sem < 0){
        perror("ftok sem");
        return -1;
    }
    printf("fd_key_sem = 0x%x\n", fd_key_sem);
    fd_sem = semget(fd_key_sem, 1, IPC_CREAT | 0666);
    if(fd_sem < 0){
        printf("errno = %d\n", errno);
        perror("semget sem");
        return -1;
    }
    printf("Init semaphore\n");
    return 0;
}

int main(){

    char filename_mem[] = FILEMEMORY;
    char filename_sem[] = FILESEM;

    if( init_memory_and_semaphore(filename_mem, filename_sem) ){
        return -1;
    }
    semctl (fd_sem, 0, SETVAL, 0); 
    sprintf(ptr + POS_ITER_USER, "%4d", 0);
    init_server();
    pthread_t id;
    pthread_create(&id, NULL, thread_message_processing, (void**)NULL);

    sprintf(ptr, "%4d", count_user);
    printf("sem = %d\n", semctl (fd_sem, 0, GETVAL, 0)); 

    char buffer_com[SIZE_MSG];
    memset(buffer_com, 0, sizeof(buffer_com));

    while(1){
        printf("Input: ");
        fgets(buffer_com, sizeof(buffer_com), stdin);
        replace_enter(buffer_com);
        if( !strcmp(buffer_com, "exit") ){
            break;
        }
    }
    sscanf(ptr, "%d", &count_user);
    int count_user_iter = count_user;
    int found_user;
    for(int i = 0; i < count_user_iter; ++i){
        sscanf(ptr + POS_LIST_USER + (SIZE_CELL * i) + SIZE_LOGIN, "%2d", &found_user);
        if(found_user == 0)
            i++;
        else
            sprintf(ptr + POS_LIST_USER + (SIZE_CELL * i) + POS_SR, "%2d", 8);
    }
    sprintf(ptr + 4, "%4d", 0);
    sprintf(ptr + 8, "%s", buffer_com);
    sprintf(ptr + POS_ITER_USER, "%4d", count_user-1);
    exit_status = 1;
    semop(fd_sem, &sem_unlock, 1);
    sleep(2);
    pthread_join(id, (void*)&found_user);
    // Delete resource
    if(shmdt(ptr) < 0){
        printf("errno = %d\n", errno);
        perror("shmdt");
        return -1;
    }
    if(shmctl(fd, IPC_RMID, 0) < 0){
        printf("errno = %d\n", errno);
        perror("shmctl");
        return -1;
    }
    printf("Delete memory\n");
    if(semctl(fd_sem, 0, IPC_RMID, 0) < 0){
        printf("errno = %d\n", errno);
        perror("semctl sem");
        return -1;
    }
    printf("Delete semaphore\n");
    printf("End server\n");
    return 0;
}






