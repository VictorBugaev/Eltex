#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main (){
    int len;
    printf("\tp2 Start\n\t My PID = %d\n", getpid());
    char buf[256];
    len = read (0, buf,sizeof(buf));
    if (len>0){
        printf("\t read: %d, %s\n", len, buf);
    }
    else{
        printf("\t read_error: %d\n", len);
    }
    printf("\tp2 End\n");
    return 0;
}
