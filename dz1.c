#include"stdio.h"
int main(){ 
   
    unsigned char b;
    int b1,b2,b3,b4;
    int a = 0xDDCCBBAA;
    int i=0;
    for (i;i<4;i++){
        b=((a>>i*8) & 0xFF);
       switch (i)
       {
            case 0:
                printf("Первый байт - %x\n",b);
                b1=b;
                break;
            case 1:
                printf("Второй байт - %x\n",b);
                b2=b;
                break;
            case 2:
                printf("Третий байт - %x\n",b);
                b3=0xAA;// Меняем значение третьего байта            
                break;
            case 3:
                printf("Четвёртый байт - %x\n",b);
                b4=b;
                break;
            default:
                break;
       }
    }
    int a1= (b4<<24) | (b3<<16) | (b2<<8) | b1;
    printf("Изменение третьего байта %x\n",a1);
}