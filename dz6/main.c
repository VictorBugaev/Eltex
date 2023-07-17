#include <stdio.h>
#include <stdlib.h>

#include "operations.h"


int main(){


    int command;
    int a, b;
    while(1){
        printf("Команда:\n Выход(0)\n Сумма(1)\n Разность(2)\n Умножение(3)\n Деление(4)\n");
        printf("Введите команду: ");
        if( !(scanf("%d", &command) == 1) ){
            command = -1;
            while (getchar() != '\n');
        }
        if(command == 0){
            break;
        }
        printf("Введите a:\n ");
        scanf("%d", &a);
        printf("Введите b:\n ");
        scanf("%d", &b);      
        switch(command){
            case 1:
                printf("%d + %d = %d\n", a, b, sum(a, b));
                break;
            case 2:
                printf("%d - %d = %d\n", a, b, dif(a, b));
                break;
            case 3:
                printf("%d * %d = %d\n", a, b, mul(a, b));
                break;
            case 4:
                printf("%d / %d = %d\n", a, b, del(a, b));
                break;
            default:
                printf("Команда не найдена\n");
                break;
        }
    }

    printf("Выход\n");
    return 0;
}
