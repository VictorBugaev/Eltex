#include <stdio.h>

//#define _GNU_SOURCE
#define __USE_GNU
#include <dlfcn.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "libs_src/operations.h"

#define SIZENAME 40
//#define DEBUG

struct Stack{
    char filename[SIZENAME];
    struct Stack *next;
    void *descriptor;
    struct Info_func_operation *func_table;
    int count_func;
};

struct Stack *Create_stack(struct Stack *next){
    struct Stack *new_element = (struct Stack*)malloc( sizeof(struct Stack) );
    new_element->next = next;
    return new_element;
}
void Delete_all_stack(struct Stack *head){
    struct Stack *ptr;
    while(head){
        ptr = head;
        head = head->next;
        free(ptr);
    }
}
void Print_stack(struct Stack *head){
    while(head){
        printf("\t%s\n", head->filename);
        head = head->next;
    }
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
void Close_libs(struct Stack *head){
    while(head){
        if(head->descriptor){
            dlclose(head->descriptor);
            head->descriptor = NULL;
        }
        head = head->next;
    }
}
int Open_libs(struct Stack *head){
    struct Stack *ptr = head;
    int count_command = 0;
    while(ptr){
        ptr->descriptor = dlopen(ptr->filename, RTLD_LAZY);
        if(!ptr->descriptor){
            printf("Error: не удалось загрузить плагин: %s\n", ptr->filename);
            dlerror();
            ptr->func_table = NULL;
        }
        else{
            ptr->func_table = (struct Info_func_operation*) dlsym(ptr->descriptor, "func_table");
            if(dlerror() != NULL){
                printf("Error: неверный формат модуля - нет данных о функциях\n");
            }
            ptr->count_func = *(int*)dlsym(ptr->descriptor, "count_func");
            if(dlerror() != NULL){
                printf("Error: неверный формат модуля - нет данных о количестве функций\n");
            }
            else{
                count_command += ptr->count_func;
            }
        }
        ptr = ptr->next;
    }
    return count_command;
}

int main(int argc, char *argv[]){
    printf("START\n");
    int i;
    struct Stack *head = Create_stack(NULL), *ptr = NULL;
    char buffer_name[SIZENAME];
    // Ввод библеотек
    if(argc < 2){
        i = 0;
        while(1){
            printf("Введите space чтоб закончить\n");
            printf("Библеотека: ");
            fgets(buffer_name, SIZENAME, stdin);
            replace_enter(buffer_name);
            if( strcmp(buffer_name, " ") == 0 ){
                break;
            }
            strcpy(head->filename, buffer_name);
            head = Create_stack(head);
            ++i;
        } 
        ptr = head;
        head = head->next;
        free(ptr);
    }
    // библеотеки как аргументы при запуске программы
    else{
        i = argc-1;
        while(1){
            sprintf(head->filename, "%s", argv[i]);
            if(1 == i){
                break;
            }
            head = Create_stack(head);
            --i;
        }
    }
    printf("Введеные библеотеки:\n");
    Print_stack(head);

    int count_command = Open_libs(head);
    
    #ifdef DEBUG
    printf("count comand = %d\n", count_command);
    #endif
    if(count_command < 1){
        printf("Функции в библеотеках не найдены\n");
        printf("Exit program\n");
        Close_libs(head);
        Delete_all_stack(head);
        return 0;
    }
    // Интерфейс с командами
    char *interface = (char*)malloc( sizeof(char) * ( count_command * (SIZENAMEFUNC + 6) ) );
    // Массив указателей на операции
    void **funcs = (void**)malloc(sizeof(void*) * count_command);
    // Количество аргументов
    int *funcs_argc = (int*)malloc(sizeof(int) * count_command);
    // Сдвиг в строке с интерфейсом
    int shift = sprintf(interface, "exit(0)");
    int iter = 1;
    ptr = head;
    // Цикл формирования интрефейса, загрузка всех функций(открываются сразу все, для более быстрого доступа)
    while(ptr){
        if(ptr->descriptor){
            for(i = 0; i < ptr->count_func; ++i){
                shift += sprintf(interface + shift, ", %s(%d)", ptr->func_table[i].operation_name, iter);
                funcs[iter-1] = dlsym(ptr->descriptor, ptr->func_table[i].func_name);
                if(dlerror() != NULL){
                    printf("Error: функция %s не найдена\n", ptr->func_table[i].func_name);
                }
                funcs_argc[iter - 1] = ptr->func_table[i].argc;
                #ifdef DEBUG
                printf("[-] %s %s %d\n", ptr->filename, ptr->func_table[i].func_name, ptr->func_table[i].argc);
                #endif
                ++iter;
            }
        }
        ptr = ptr->next;
    }
    #ifdef DEBUG
    printf("shift = %d, size = %d\n", shift, ( count_command * (SIZENAMEFUNC + 6) ));
    #endif
    int input_command;
    double input_prm[3];
    // Главный цикл
    while(1){
        printf("Commands: %s\n", interface);
        printf("Input: ");
        if( !(1 == scanf("%d", &input_command)) ){
            while (getchar() != '\n');
            printf("Команда не определена\n");
            continue;
        }
        if(input_command < 0 || input_command > count_command){
            printf("Команда не определена\n");
            continue;
        }
        if(input_command == 0){
            break;
        }
        --input_command;
        #ifdef DEBUG
        printf("input_command = %d, funcs_argc = %d\n", input_command, funcs_argc[input_command]);
        #endif
        switch(funcs_argc[input_command]){
            case 0:
                printf("result = %lf\n", ((double(*)())funcs[input_command])() );
                break;
            case 1:
                printf("Введите 1 аргумент: ");
                if( !(scanf("%lf", input_prm) == 1) ){
                    printf("\tНеверные аргументы\n");
                    continue;
                }
                printf("result = %lf\n", ((double(*)(double))funcs[input_command])(input_prm[0]) );
                break;
            case 2:
                printf("Введите 2 аргумента: ");
                if( !(scanf("%lf%lf", input_prm, input_prm + 1) == 2) ){
                    printf("\tНеверные аргументы\n");
                    continue;
                }
                printf("result = %lf\n", ((double(*)(double, double))funcs[input_command])(input_prm[0], input_prm[1]) );
                break;
            case 3:
                printf("Введите 3 аргумента: ");
                if( !(scanf("%lf%lf%lf", input_prm, input_prm + 1, input_prm + 2) == 3) ){
                    printf("\tНеверные аргументы\n");
                    continue;
                }
                printf("result = %lf\n", ((double(*)(double, double, double))funcs[input_command])(input_prm[0], input_prm[1], input_prm[2]) );
                break;
            default:
                printf("Для текущей функции превышено количество агрументов\n");
                break;
        }
    }
    free(funcs);
    free(funcs_argc);
    free(interface);
    Close_libs(head);
    Delete_all_stack(head);
    printf("END\n");
    return 0;
}





