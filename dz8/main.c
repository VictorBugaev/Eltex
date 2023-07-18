#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define SIZE_PAGE_STR_X 100
#define SIZE_PAGE_STR_Y 30
#define COUNT_BUTTON_CONP 3
#define CONTORL_PANEL 0
#define EDIT_PANEL 1
#define KEY_Q 113
#define KEY_TAB 9
#define KEY_SPACE 32
#define MKEY_ENTER 10

struct symbol {
    struct symbol *next;
    struct symbol *pred;
    unsigned char s;
};

struct str_symbol {
    struct symbol *head_symbol;
    struct str_symbol *next;
    struct str_symbol *pred;
};

char button_conp[COUNT_BUTTON_CONP][20] = {
    "exit", "open", "save"
};

void set_button(WINDOW*, int, int, int);
WINDOW *render_control_panel(int, int, int, int);
void sig_winch(int signo);
void Print_subpanel(WINDOW *, struct str_symbol *);
struct symbol *Create_symbol(char, struct symbol *);
struct str_symbol *Create_str_symbol(struct symbol *, struct str_symbol *);
struct symbol *return_ptr_end_symbol(struct symbol *, int *);
void delete_str_symbol(struct str_symbol *);
void get_filename(WINDOW *, char *, int);
struct str_symbol *read_file(int);
void write_file(int, struct str_symbol *);
void delete_table_symbol(struct str_symbol *);

void set_button(WINDOW *control_panel, int pos_button, int pos_y, int pos_x) {
    wattron(control_panel, COLOR_PAIR(1));
    wmove(control_panel, pos_y, pos_x);
    int i;
    for(i = 0; i < COUNT_BUTTON_CONP; ++i) {
        if(i == pos_button) {
            wattron(control_panel, COLOR_PAIR(2));
        }
        else {
            wattron(control_panel, COLOR_PAIR(1));
        }
        wprintw(control_panel, "%s  ", button_conp[i]);
    }
    wrefresh(control_panel);
}

WINDOW *render_control_panel(int sizey_conp, int sizex_conp, int posy_conp, int posx_conp) {
    WINDOW *control_panel = newwin(sizey_conp, sizex_conp, posy_conp, posx_conp);
    box(control_panel, '|', '-');
    set_button(control_panel, 0, 1, 1);
    wrefresh(control_panel);
    return control_panel;
}

void sig_winch(int signo) {
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
    resizeterm(size.ws_row, size.ws_col);
}

void Print_subpanel(WINDOW *panel, struct str_symbol *head) {
    struct str_symbol *ptr_row = head;
    struct symbol *ptr_col;
    int posx = 1, posy = 1;
    wclear(panel);
    box(panel, '|', '=');
    wmove(panel, posy, posx);
    while (ptr_row) {
        ptr_col = ptr_row->head_symbol;
        while (ptr_col) {
            wprintw(panel, "%c", ptr_col->s);
            ptr_col = ptr_col->next;
        }
        posx = 1;
        wmove(panel, ++posy, posx);
        ptr_row = ptr_row->next;
    }
    wrefresh(panel);
}

struct symbol *Create_symbol(char s, struct symbol *pred) {
    struct symbol *new_s = (struct symbol *)malloc(sizeof(struct symbol));
    new_s->s = s;
    new_s->next = NULL;
    new_s->pred = pred;
    return new_s;
}

struct str_symbol *Create_str_symbol(struct symbol *ptr_s, struct str_symbol *pred) {
    struct str_symbol *new_ss = (struct str_symbol *)malloc(sizeof(struct str_symbol));
    new_ss->head_symbol = ptr_s;
    new_ss->next = NULL;
    new_ss->pred = pred;
    return new_ss;
}

struct symbol *return_ptr_end_symbol(struct symbol *head, int *posx) {
    int iter = 0;
    while (head->next) {
        head = head->next;
        iter++;
    }
    (*posx) = iter;
    return head;
}

void delete_str_symbol(struct str_symbol *head) {
    struct symbol *ptr = head->head_symbol;
    while (ptr->next) {
        if (ptr->pred)
            free(ptr->pred);
        ptr = ptr->next;
    }
    free(ptr);
    free(head);
}

void get_filename(WINDOW *win, char *filename, int max_len) {
    int i = 0;
    int ch;
    keypad(win, TRUE);
    int x = 4, y = 5;
    while (((ch = wgetch(win)) != 10) && (i < max_len - 1)) {
        if (ch == KEY_BACKSPACE) {
            int x, y;
            if (i == 0)
                continue;
            i--;
        } else {
            filename[i] = ch;
            wmove(win, 2, i + 1);
            wprintw(win, "%c", ch);
            ++i;
        }
    }
    filename[i] = 0;
    wechochar(win, '\n');
}

struct str_symbol *read_file(int fd) {
    unsigned char ch;
    struct str_symbol *head;
    if (read(fd, &ch, 1))
        head = Create_str_symbol(Create_symbol(ch, NULL), NULL);
    else {
        head = Create_str_symbol(Create_symbol(' ', NULL), NULL);
        return head;
    }
    struct str_symbol *ptr_row = head;
    struct symbol *ptr_col = head->head_symbol;
    int con = 0;
    while (read(fd, &ch, 1)) {
        if (con == 1) {
            ptr_row->next = Create_str_symbol(Create_symbol(ch, NULL), ptr_row);
            ptr_row->next->pred = ptr_row;
            ptr_row = ptr_row->next;
            ptr_col = ptr_row->head_symbol;
            con = 0;
        } else if (ch == '\n') {
            con = 1;
        } else {
            ptr_col->next = Create_symbol(ch, ptr_col);
            ptr_col->next->pred = ptr_col;
            ptr_col = ptr_col->next;
        }
    }
    return head;
}

void write_file(int fd, struct str_symbol *head) {
    lseek(fd, 0L, 0);
    struct symbol *ptr_col;
    char n = '\n';
    while (head) {
        ptr_col = head->head_symbol;
        while (ptr_col) {
            write(fd, (void *)&ptr_col->s, 1);
            ptr_col = ptr_col->next;
        }
        write(fd, (void *)&n, 1);
        head = head->next;
    }
}

void delete_table_symbol(struct str_symbol *head) {
    while (head->next) {
        if (head->pred)
            delete_str_symbol(head->pred);
        head = head->next;
    }
    delete_str_symbol(head);
}

int main(int argc, char *argv[]) {
    initscr();
    const int sizey_conp = 5;
    const int sizex_conp = 50;
    const int posy_conp = 1;
    const int posx_conp = 1;
    const int sizey_editp = SIZE_PAGE_STR_Y;
    const int sizex_editp = SIZE_PAGE_STR_X;
    const int posy_editp = posy_conp + sizey_conp;
    const int posx_editp = 1;
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_GREEN);
    init_pair(2, COLOR_BLUE, COLOR_GREEN);
    WINDOW *edit_panel;
    WINDOW *control_panel;
    signal(SIGWINCH, sig_winch);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    refresh();
    printw("File editor(demo)\n");
    control_panel = render_control_panel(sizey_conp, sizex_conp, posy_conp, posx_conp);
    edit_panel = newwin(sizey_editp, sizex_editp, posy_editp, posx_editp);
    box(edit_panel, '|', '=');
    wrefresh(edit_panel);
    refresh();
    int key;
    int posx_cur_edit = 0, posy_cur_edit = 0;
    int panel = 0;
    int set_but_conp = 0;
    int run_main_while = 1;
    int file_condition = 0;
    char buffer_fname[FILENAME_MAX];
    int descriptor;
    struct str_symbol *head;
    if (argc > 1) {
        descriptor = open(argv[1], O_CREAT | O_RDWR);
        if (descriptor > -1) {
            file_condition = 1;
            head = read_file(descriptor);
        } else {
            head = Create_str_symbol(Create_symbol(' ', NULL), NULL);
        }
    } else {
        head = Create_str_symbol(Create_symbol(' ', NULL), NULL);
    }
    struct str_symbol *ptr_row = head, *ptr_row_buf;
    struct symbol *ptr_col = head->head_symbol, *ptr_col_buf;
    Print_subpanel(edit_panel, head);
    while (run_main_while) {
        switch (panel) {
            case CONTORL_PANEL:
                curs_set(FALSE);
                key = getch();
                switch (key) {
                    case KEY_TAB:
                        panel = 1;
                        break;
                    case KEY_LEFT:
                        if (set_but_conp > 0) {
                            --set_but_conp;
                        }
                        set_button(control_panel, set_but_conp, 1, 1);
                        break;
                    case KEY_RIGHT:
                        if (set_but_conp < COUNT_BUTTON_CONP - 1) {
                            ++set_but_conp;
                        }
                        set_button(control_panel, set_but_conp, 1, 1);
                        break;
                    case MKEY_ENTER:
                        switch (set_but_conp) {
                            case 0:
                                run_main_while = 0;
                                break;
                            case 1:
                                get_filename(control_panel, buffer_fname, FILENAME_MAX);
                                move(30, 5);
                                if (file_condition) {
                                    close(descriptor);
                                }
                                descriptor = open(buffer_fname, O_CREAT | O_RDWR);
                                if (descriptor > -1) {
                                    wmove(control_panel, 3, 1);
                                    wprintw(control_panel, "file open- %s\n", buffer_fname);
                                    file_condition = 1;
                                    delete_table_symbol(head);
                                    head = read_file(descriptor);
                                    ptr_row = head;
                                    ptr_col = head->head_symbol;
                                    posx_cur_edit = 0;
                                    posy_cur_edit = 0;
                                } else {
                                    wmove(control_panel, 3, 1);
                                    wprintw(control_panel, "file not found - %d\n", descriptor);
                                    file_condition = 0;
                                }
                                break;
                            case 2:
                                if (file_condition) {
                                    write_file(descriptor, head);
                                    close(descriptor);
                                    descriptor = open(buffer_fname, O_CREAT | O_RDWR);
                                } else {
                                    get_filename(control_panel, buffer_fname, FILENAME_MAX);
                                    descriptor = open(buffer_fname, O_CREAT | O_RDWR);
                                    if (descriptor > -1) {
                                        wmove(control_panel, 3, 1);
                                        wprintw(control_panel, "file open- %s\n", buffer_fname);
                                        write_file(descriptor, head);
                                        close(descriptor);
                                        descriptor = open(buffer_fname, O_CREAT | O_RDWR);
                                        file_condition = 1;

                                    } else {
                                        wmove(control_panel, 3, 1);
                                        wprintw(control_panel, "error - %s\n", buffer_fname);
                                        file_condition = 0;
                                    }
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case EDIT_PANEL:
                curs_set(TRUE);
                move(posy_editp + posy_cur_edit + 1, posx_editp + posx_cur_edit + 1);
                key = getch();
                switch (key) {
                    case KEY_TAB:
                        panel = 0;
                        break;
                    case KEY_DOWN:
                        if (ptr_row->next) {
                            ptr_row = ptr_row->next;
                            ptr_col = ptr_row->head_symbol;
                            posx_cur_edit = 0;
                            ++posy_cur_edit;
                        }
                        break;
                    case KEY_UP:
                        if (ptr_row->pred) {
                            ptr_row = ptr_row->pred;
                            ptr_col = ptr_row->head_symbol;
                            posx_cur_edit = 0;
                            --posy_cur_edit;
                        }
                        break;
                    case KEY_LEFT:
                        if (ptr_col->pred) {
                            ptr_col = ptr_col->pred;
                            --posx_cur_edit;
                        } else if (ptr_row->pred) {
                            ptr_row = ptr_row->pred;
                            ptr_col = return_ptr_end_symbol(ptr_row->head_symbol, &posx_cur_edit);
                            --posy_cur_edit;
                        }
                        break;
                    case KEY_RIGHT:
                        if (ptr_col->next) {
                            ptr_col = ptr_col->next;
                            ++posx_cur_edit;
                        } else if (ptr_row->next) {
                            ptr_row = ptr_row->next;
                            ptr_col = ptr_row->head_symbol;
                            posx_cur_edit = 0;
                            ++posy_cur_edit;
                        }
                        break;
                    case KEY_SPACE:
                        ptr_col_buf = ptr_col->next;
                        ptr_col->next = Create_symbol(' ', ptr_col);
                        ptr_col->next->next = ptr_col_buf;
                        if (ptr_col_buf)
                            ptr_col_buf->pred = ptr_col->next;
                        ptr_col = ptr_col->next;
                        ++posx_cur_edit;
                        break;
                    case MKEY_ENTER:
                        if (ptr_row->next) {
                            ptr_row_buf = ptr_row->next;
                            if (ptr_col->next) {
                                ptr_col_buf = ptr_col->next;
                                ptr_col_buf->pred = NULL;
                                ptr_col->next = NULL;
                            } else {
                                ptr_col_buf = Create_symbol(' ', NULL);
                            }
                            ptr_row->next = Create_str_symbol(ptr_col_buf, ptr_row);
                            ptr_row->next->next = ptr_row_buf;
                            ptr_row_buf->pred = ptr_row->next;
                            ptr_row = ptr_row->next;
                            ptr_col = ptr_row->head_symbol;
                            posx_cur_edit = 0;
                            ++posy_cur_edit;
                        } else {
                            if (ptr_col->next) {
                                ptr_col_buf = ptr_col->next;
                                ptr_col_buf->pred = NULL;
                                ptr_col->next = NULL;
                            } else {
                                ptr_col_buf = Create_symbol(' ', NULL);
                            }
                            ptr_row->next = Create_str_symbol(ptr_col_buf, ptr_row);
                            ptr_row = ptr_row->next;
                            ptr_col = ptr_row->head_symbol;
                            posx_cur_edit = 0;
                            ++posy_cur_edit;
                        }
                        break;
                    case KEY_BACKSPACE:
                        if (ptr_col->pred) {
                            ptr_col_buf = ptr_col;
                            ptr_col->pred->next = ptr_col->next;
                            if (ptr_col->next)
                                ptr_col->next->pred = ptr_col->pred;
                            ptr_col = ptr_col->pred;
                            free(ptr_col_buf);
                            --posx_cur_edit;
                        } else if (ptr_row->pred) {
                            ptr_row_buf = ptr_row;
                            ptr_row->pred->next = ptr_row->next;
                            if (ptr_row->next) {
                                ptr_row->next->pred = ptr_row->pred;
                            }
                            ptr_col = return_ptr_end_symbol(ptr_row->pred->head_symbol, &posx_cur_edit);
                            if (ptr_row->head_symbol->next) {
                                ptr_col->next = ptr_row->head_symbol->next;
                                ptr_row->head_symbol->next->pred = ptr_col;
                                ptr_row->head_symbol->next = NULL;
                            }
                            ptr_row = ptr_row->pred;
                            delete_str_symbol(ptr_row_buf);
                            --posy_cur_edit;
                        }
                        break;
                    default:
                        ptr_col_buf = ptr_col->next;
                        ptr_col->next = Create_symbol(key, ptr_col);
                        ptr_col->next->next = ptr_col_buf;
                        if (ptr_col_buf)
                            ptr_col_buf->pred = ptr_col->next;
                        ptr_col = ptr_col->next;
                        ++posx_cur_edit;
                        break;
                }
                Print_subpanel(edit_panel, head);
                break;
            default:
                panel = 0;
                break;
        }
    }
    delwin(control_panel);
    delwin(edit_panel);
    endwin();
    exit(EXIT_SUCCESS);
    return 0;
}