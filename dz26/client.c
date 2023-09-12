#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <pthread.h>

#include "info.h"

char login[SIZE_LOGIN];
int pos_log;
int count_user = 0;

char *ptr;
int fd_sem;
int fd;
struct sembuf sem1;

struct sembuf sem_lock = {0, -10, 0}, sem_unlock = {0, 10, 0}, sem_unlocks = {0, 10, 0};

void *thread_message_processing(void *args){
    int st_read;
    char sst_read[4];
    int user;
    char suser[4];
    char log_user[SIZE_LOGIN];
    printf("start message processing\n");
    int cr_users;
    while(1){
        semop(fd_sem, &sem_lock, 1);
        sscanf(ptr + POS_LIST_USER + (SIZE_CELL * pos_log) + POS_SR, "%2d", &st_read);
        strncpy(suser, ptr + 4, 4);
        sscanf(suser, "%4d", &user);
        if(user == pos_log){
            semop(fd_sem, &sem_unlock, 1);
            continue;
        }
        if(st_read == 0){
            semop(fd_sem, &sem_unlock, 1);
            continue;
        }
        if(user == 0){
            printf("Server exit\n");
            semop(fd_sem, &sem_unlock, 1);
            exit(0);
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
void init_user(){
    sscanf(ptr, "%4d", &count_user);
    printf("cu = %d\n", count_user);
    int delta = POS_LIST_USER + (count_user * SIZE_CELL);
    printf("delta = %d\n", delta);
    for(int i = 0; i < SIZE_LOGIN; ++i){
        login[i] = 'A' + (rand()%27);
        ptr[delta + i + 1] = login[i];
    }
    printf("delta + SL = %d\n", delta + SIZE_LOGIN);
    sprintf(ptr + delta + SIZE_LOGIN, MASK, 1, 5, 0);
    login[SIZE_LOGIN-1] = '\0';
    printf("login = %s\n", login);
    
    pos_log = count_user;
    printf("poslog = %d\n", pos_log);
    count_user+=1;
    sprintf(ptr, "%d", count_user);
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
    srand(time(NULL));
    char filename_mem[] = FILEMEMORY;
    char filename_sem[] = FILESEM;
    if( init_memory_and_semaphore(filename_mem, filename_sem) ){
        return -1;
    }
    init_user();
    pthread_t id;
    pthread_create(&id, NULL, thread_message_processing, (void**)NULL);
    char buffer_com[SIZE_MSG];
    memset(buffer_com, 0, sizeof(buffer_com));
    int found_user;
    int count_user_iter;
    while(1){
        printf("Input: ");
        fgets(buffer_com, sizeof(buffer_com), stdin);
        replace_enter(buffer_com);
        if( !strcmp(buffer_com, "exit") ){
            break;
        }
        else if(!strcmp(buffer_com, "list")){
            sscanf(ptr, "%d", &count_user);
            count_user_iter = count_user;
            for(int i = 0; i < count_user_iter; ++i){
                sscanf(ptr + POS_LIST_USER + (SIZE_CELL * i) + SIZE_LOGIN, "%2d", &found_user);
                if(found_user == 0)
                    i++;
                else{
                    for(int i2 = 0; i2 < SIZE_LOGIN; ++i2){
                        printf("%c",ptr[POS_LIST_USER + (SIZE_CELL * i) + i2]); 
                    }
                    printf("\n");
                }
            }
        }
        else{
            sscanf(ptr, "%d", &count_user);
            count_user_iter = count_user;
            for(int i = 0; i < count_user_iter; ++i){
                sscanf(ptr + POS_LIST_USER + (SIZE_CELL * i) + SIZE_LOGIN, "%2d", &found_user);
                if(found_user == 0)
                    i++;
                else
                    sprintf(ptr + POS_LIST_USER + (SIZE_CELL * i) + POS_SR, "%2d", 8);
            }
            sprintf(ptr + 4, "%4d", pos_log);
            sprintf(ptr + 8, "%s", buffer_com);
            sprintf(ptr + POS_ITER_USER, "%4d", count_user-1);
            semop(fd_sem, &sem_unlock, 1);
        }
    }
    sscanf(ptr, "%d", &count_user);
    count_user--;
    sprintf(ptr,"%d", count_user);
    sprintf(ptr + POS_LIST_USER + (SIZE_CELL * pos_log) + SIZE_LOGIN, "%2d", 0);
    if(shmdt(ptr) < 0){
        printf("errno = %d\n", errno);
        perror("shmdt");
        return -1;
    }
    printf("End client\n");
    return 0;
}