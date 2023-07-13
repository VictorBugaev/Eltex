#include "stdio.h"

int main (){
    int a = 0xAABBCCDD;
    int i = 1;
    unsigned char *ptr;
    ptr = (unsigned char *)&a;
    for(i;i<=4;i++){
        printf("%d байт a=%X\n",i,*ptr);
        ptr++;
        if (i==2){
            *ptr=0xEE;
        }
    }

}
