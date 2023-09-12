



#define FILEMEMORY "mem"
#define KEYMEM 'A'
#define FILESEM "server"
#define KEYSEM 'B'

#define SIZE 1000
#define POS_LIST_USER 500
#define SIZE_LOGIN 20
#define SIZE_CELL (SIZE_LOGIN + 2 + 2 + 2)
#define SIZE_MSG 100

#define MASK "%2d%2d%2d"
#define POS_SR (SIZE_LOGIN + 4)
#define POS_ITER_USER (100 + 8)
//Хранение информации о пользователе
//login[20], found_cell[2], status user[2] status read[2]
//Обмен сообщением
//count user[4],user position send msg[4], msg[100],count received msg[4]

struct message{
    int pos_login;
    char msg[100];
};
