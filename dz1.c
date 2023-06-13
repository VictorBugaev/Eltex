#include"stdio.h"
int main(){
    int a = 0xDDCCBBAA;
    unsigned char b;
    int d=0xDD00BBAA;
    int c;
    int i=0;
    for (i;i<4;i++){
        b=((a>>i*8) & 0xFF);
       switch (i)
       {
            case 0:
                printf("Первый байт - %x\n",b);
                break;
            case 1:
                printf("Второй байт - %x\n",b);
                break;
            case 2:
                printf("Третий байт - %x\n",b);
                break;
            case 3:
                printf("Четвёртый байт - %x\n",b);
                break;
           default:
                break;
       }
    }
    c = a|d;
    printf("Изменение третьего байта %x\n",c);
}