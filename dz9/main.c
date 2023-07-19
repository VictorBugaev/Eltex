#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <curses.h>

#define KEY_ENTER 10
#define KEY_ESCAPE 27

void sigWinch(int signo) {
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
    resizeterm(size.ws_row, size.ws_col);
}

void printDirectoryContents(WINDOW *win, DIR *dir, int pos) {
    struct dirent *entry;
    int i = 0;

    wclear(win);
    wattron(win, COLOR_PAIR(3));
    box(win, '|', '-');
    wmove(win, 2, 1);

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0)
            continue;

        if (i == pos)
            wattron(win, COLOR_PAIR(1));
        else
            wattron(win, COLOR_PAIR(2));

        wprintw(win, "%s", entry->d_name);
        wmove(win, 2 + i + 1, 1);
        i++;
    }

    wrefresh(win);
}

int countDirectoryContents(DIR *dir) {
    struct dirent *entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0)
            count++;
    }

    rewinddir(dir);
    return count;
}

char *getCurrentDirectory() {
    char *cwd = (char *)malloc(PATH_MAX);
    if (cwd != NULL)
        getcwd(cwd, PATH_MAX);
    return cwd;
}

int main() {
    initscr();
    start_color();

    const int sizey_conp = 5;
    const int sizex_conp = 50;
    const int posy_conp = 1;
    const int posx_conp = 1;

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);

    signal(SIGWINCH, sigWinch);
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    refresh();

    printw("File Manager (demo)\n");

    WINDOW *panel_file = newwin(30, 50, posy_conp + sizey_conp, posx_conp);
    box(panel_file, '|', '-');
    wrefresh(panel_file);

    int key;
    int run_main_while = 1;
    int pos_set = 0;
    char *current_dir = getCurrentDirectory();
    DIR *dir = opendir(current_dir);

    printDirectoryContents(panel_file, dir, pos_set);

    while (run_main_while) {
        key = getch();

        switch (key) {
            case KEY_ESCAPE:
                run_main_while = 0;
                break;
            case KEY_UP:
                if (pos_set > 0)
                    pos_set--;
                break;
            case KEY_DOWN:
                if (pos_set < countDirectoryContents(dir) - 1)
                    pos_set++;
                break;
            case KEY_ENTER:
                if (pos_set < countDirectoryContents(dir)) {
                    struct dirent *entry = NULL;
                    int count = 0;

                    rewinddir(dir);

                    while ((entry = readdir(dir)) != NULL) {
                        if (strcmp(entry->d_name, ".") != 0) {
                            if (count == pos_set) {
                                if (entry->d_type == DT_DIR) {
                                    char new_path[PATH_MAX];
                                    snprintf(new_path, sizeof(new_path), "%s/%s", current_dir, entry->d_name);

                                    DIR *new_dir = opendir(new_path);

                                    if (new_dir != NULL) {
                                        closedir(dir);
                                        dir = new_dir;
                                        pos_set = 0;
                                        free(current_dir);
                                        current_dir = strdup(new_path);
                                        printDirectoryContents(panel_file, dir, pos_set);
                                    }
                                }
                            }
                            count++;
                        }
                    }
                }
                break;
            default:
                break;
        }

        move(posy_conp + sizey_conp + 4, posx_conp);
        printw("Current Directory: %s", current_dir);
        refresh();

        printDirectoryContents(panel_file, dir, pos_set);
    }

    free(current_dir);
    closedir(dir);
    delwin(panel_file);
    endwin();
    exit(EXIT_SUCCESS);
}