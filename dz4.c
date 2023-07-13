#include"stdio.h"
#include"stdlib.h"
#define MAX_ABONENT 100
#define MAX_VALUE 20
struct abonent{
    char name[MAX_VALUE];
    char surname[MAX_VALUE];
    char number[MAX_VALUE];
};
void add_abonent(struct abonent entries[], int *num_entries);
void delelte_abonent(struct abonent entries[], int *num_entries);
void search_abonent(struct abonent entries[], int num_entries);
void print_abonent(struct abonent entries[],int num_entries);

void add_abonent(struct abonent entries[], int *num_entries){
    if (*num_entries >= MAX_ABONENT){
        printf("Телефонная книга переполнена\n");
        return;
    }
    struct abonent new_abonent;
    printf("Введите имя:\n");
    scanf("%s", new_abonent.name);
    printf("Введите фамилию:\n");
    scanf("%s", new_abonent.surname);
    printf("Введите номер телефона:\n");
    scanf("%s", new_abonent.number);
    entries[*num_entries]=new_abonent;
    *num_entries += 1;
    printf("Запись добавлена!\n");
}
void delelte_abonent(struct abonent entries[], int *num_entries){
    if (*num_entries == 0){
        printf("Телефонная книга пуста\n");
        return;
    }
    char name[MAX_VALUE];
    char surname[MAX_VALUE];
    printf("Введите имя:");
    scanf("%s", name);
    printf("Введите фамилию:");
    scanf("%s", surname);
    int index = -1;
    for (int i = 0; i < *num_entries; i++){
        if (strcmp(entries[i].name, name) == 0 && strcmp(entries[i].surname, surname) == 0){
            index = i;
            break;
        }
    }
    if (index == -1){
        printf("Запись не найдена\n");
    }
    else{
        for (int i = index; i < *num_entries - 1; i++){
            entries[i] = entries[i + 1];
        }
        *num_entries -= 1;
        printf("Запись удалена\n");
    }
}
void search_abonent(struct abonent entries[], int num_entries){
    int i=0;
    int index=-1;
    char name[MAX_VALUE];
    char surname[MAX_VALUE];
    if (num_entries==0){
        printf("Телефонная книга пуста\n");
        return;
    }
    printf("Введите имя: \n");
    scanf("%s",name);
    printf("Введите фамилию: \n");
    scanf("%s", surname);
    for(i;i<num_entries;i++){
        if (strcmp(entries[i].name,name)==0 && strcmp(entries[i].surname,surname)==0){
            index=i;
            if (index==-1){
                printf("Запись не найдена\n");
            }
            else{
                for(i;i<num_entries;i++){
                    printf("Запись найдена\n");
                    printf("%s %s %s\n",entries[index].name,entries[index].surname,entries[index].number);
                }
            }
        }
    }
}
void print_abonent(struct abonent entries[],int num_entries){
    int i=0;
    if (num_entries==0)
    {
        printf ("Телефонная книга пуста\n");
        return;
    }
    printf("Записи в телефонной книге: \n");
    for(i;i<num_entries;i++){
        printf("%d %s %s %s\n",i+1,entries[i].name,entries[i].surname,entries[i].number);
    }
    
}
int main(){
    struct abonent entries [MAX_ABONENT];
    int num_entries = 0;
    int choise=0;
    do{
        printf("1. Добавить запись\n");
        printf("2. Удалить абонента\n");
        printf("3. Найтиа бонента\n");
        printf("4. Вывести телефонную книгу\n");
        printf("5. Выход\n");
        printf("Выберите действие\n");
        scanf("%d",&choise);
        switch (choise)
        {
        case 1:
            add_abonent(entries,&num_entries);
            break;
        case 2:
            delelte_abonent(entries,&num_entries);
            break;
        case 3:
            search_abonent(entries,num_entries);
            break;
        case 4:
            print_abonent(entries,num_entries);
            break;
        case 5:
            printf("Выход\n");
            break;
        default:
            printf("Неверный ввод\n");
            break;
        }
    } while (choise!=5);
    
    return 0;
}