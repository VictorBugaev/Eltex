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
#define COUNT_BUTTON_PANEL 3
#define CONTROL_PANEL 0
#define EDIT_PANEL 1
#define KEY_Q 113
#define KEY_TAB 9
#define KEY_SPACE 32
#define MKEY_ENTER 10

struct Symbol {
    struct Symbol *next;
    struct Symbol *prev;
    unsigned char character;
};

struct StringSymbol {
    struct Symbol *head;
    struct StringSymbol *next;
    struct StringSymbol *prev;
};

char buttonPanel[COUNT_BUTTON_PANEL][20] = {
    "exit", "open", "save"
};

void setButton(WINDOW*, int, int, int);
WINDOW *renderControlPanel(int, int, int, int);
void sigWinch(int signo);
void printSubpanel(WINDOW *, struct StringSymbol *);
struct Symbol *createSymbol(char, struct Symbol *);
struct StringSymbol *createStringSymbol(struct Symbol *, struct StringSymbol *);
struct Symbol *returnEndSymbol(struct Symbol *, int *);
void deleteStringSymbol(struct StringSymbol *);
void getFilename(WINDOW *, char *, int);
struct StringSymbol *readFile(int);
void writeFile(int, struct StringSymbol *);
void deleteTableSymbol(struct StringSymbol *);

void setButton(WINDOW *controlPanel, int buttonPos, int posY, int posX) {
    wattron(controlPanel, COLOR_PAIR(1));
    wmove(controlPanel, posY, posX);
    int i;
    for(i = 0; i < COUNT_BUTTON_PANEL; ++i) {
        if(i == buttonPos) {
            wattron(controlPanel, COLOR_PAIR(2));
        }
        else {
            wattron(controlPanel, COLOR_PAIR(1));
        }
        wprintw(controlPanel, "%s  ", buttonPanel[i]);
    }
    wrefresh(controlPanel);
}

WINDOW *renderControlPanel(int sizeY, int sizeX, int posY, int posX) {
    WINDOW *controlPanel = newwin(sizeY, sizeX, posY, posX);
    box(controlPanel, '|', '-');
    setButton(controlPanel, 0, 1, 1);
    wrefresh(controlPanel);
    return controlPanel;
}

void sigWinch(int signo) {
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
    resizeterm(size.ws_row, size.ws_col);
}

void printSubpanel(WINDOW *panel, struct StringSymbol *head) {
    struct StringSymbol *rowPtr = head;
    struct Symbol *colPtr;
    int posX = 1, posY = 1;
    wclear(panel);
    box(panel, '|', '=');
    wmove(panel, posY, posX);
    while (rowPtr) {
        colPtr = rowPtr->head;
        while (colPtr) {
            wprintw(panel, "%c", colPtr->character);
            colPtr = colPtr->next;
        }
        posX = 1;
        wmove(panel, ++posY, posX);
        rowPtr = rowPtr->next;
    }
    wrefresh(panel);
}

struct Symbol *createSymbol(char c, struct Symbol *prev) {
    struct Symbol *newSymbol = (struct Symbol *)malloc(sizeof(struct Symbol));
    newSymbol->character = c;
    newSymbol->next = NULL;
    newSymbol->prev = prev;
    return newSymbol;
}

struct StringSymbol *createStringSymbol(struct Symbol *head, struct StringSymbol *prev) {
    struct StringSymbol *newStringSymbol = (struct StringSymbol *)malloc(sizeof(struct StringSymbol));
    newStringSymbol->head = head;
    newStringSymbol->next = NULL;
    newStringSymbol->prev = prev;
    return newStringSymbol;
}

struct Symbol *returnEndSymbol(struct Symbol *head, int *posX) {
    int count = 0;
    while (head->next) {
        head = head->next;
        count++;
    }
    (*posX) = count;
    return head;
}

void deleteStringSymbol(struct StringSymbol *head) {
    struct Symbol *ptr = head->head;
    while (ptr->next) {
        if (ptr->prev)
            free(ptr->prev);
        ptr = ptr->next;
    }
    free(ptr);
    free(head);
}

void getFilename(WINDOW *win, char *filename, int maxLen) {
    int i = 0;
    int ch;
    keypad(win, TRUE);
    int x = 4, y = 5;
    while (((ch = wgetch(win)) != 10) && (i < maxLen - 1)) {
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

struct StringSymbol *readFile(int fd) {
    unsigned char ch;
    struct StringSymbol *head;
    if (read(fd, &ch, 1))
        head = createStringSymbol(createSymbol(ch, NULL), NULL);
    else {
        head = createStringSymbol(createSymbol(' ', NULL), NULL);
        return head;
    }
    struct StringSymbol *rowPtr = head;
    struct Symbol *colPtr = head->head;
    int isNewRow = 0;
    while (read(fd, &ch, 1)) {
        if (isNewRow == 1) {
            rowPtr->next = createStringSymbol(createSymbol(ch, NULL), rowPtr);
            rowPtr->next->prev = rowPtr;
            rowPtr = rowPtr->next;
            colPtr = rowPtr->head;
            isNewRow = 0;
        } else if (ch == '\n') {
            isNewRow = 1;
        } else {
            colPtr->next = createSymbol(ch, colPtr);
            colPtr->next->prev = colPtr;
            colPtr = colPtr->next;
        }
    }
    return head;
}

void writeFile(int fd, struct StringSymbol *head) {
    lseek(fd, 0L, 0);
    struct Symbol *colPtr;
    char newLine = '\n';
    while (head) {
        colPtr = head->head;
        while (colPtr) {
            write(fd, (void *)&colPtr->character, 1);
            colPtr = colPtr->next;
        }
        write(fd, (void *)&newLine, 1);
        head = head->next;
    }
}

void deleteTableSymbol(struct StringSymbol *head) {
    while (head->next) {
        if (head->prev)
            deleteStringSymbol(head->prev);
        head = head->next;
    }
    deleteStringSymbol(head);
}

int main(int argc, char *argv[]) {
    initscr();
    const int controlPanelSizeY = 5;
    const int controlPanelSizeX = 50;
    const int controlPanelPosY = 1;
    const int controlPanelPosX = 1;
    const int editPanelSizeY = SIZE_PAGE_STR_Y;
    const int editPanelSizeX = SIZE_PAGE_STR_X;
    const int editPanelPosY = controlPanelPosY + controlPanelSizeY;
    const int editPanelPosX = 1;
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_GREEN);
    init_pair(2, COLOR_BLUE, COLOR_GREEN);
    WINDOW *editPanel;
    WINDOW *controlPanel;
    signal(SIGWINCH, sigWinch);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    refresh();
    printw("File editor(demo)\n");
    controlPanel = renderControlPanel(controlPanelSizeY, controlPanelSizeX, controlPanelPosY, controlPanelPosX);
    editPanel = newwin(editPanelSizeY, editPanelSizeX, editPanelPosY, editPanelPosX);
    box(editPanel, '|', '=');
    wrefresh(editPanel);
    refresh();
    int key;
    int editPanelPosXCur = 0, editPanelPosYCur = 0;
    int panel = 0;
    int selectedButtonControlPanel = 0;
    int runMainLoop = 1;
    int fileCondition = 0;
    char filenameBuffer[FILENAME_MAX];
    int fileDescriptor;
    struct StringSymbol *head;
    if (argc > 1) {
        fileDescriptor = open(argv[1], O_CREAT | O_RDWR);
        if (fileDescriptor > -1) {
            fileCondition = 1;
            head = readFile(fileDescriptor);
        } else {
            head = createStringSymbol(createSymbol(' ', NULL), NULL);
        }
    } else {
        head = createStringSymbol(createSymbol(' ', NULL), NULL);
    }
    struct StringSymbol *rowPtr = head, *rowPtrBuf;
    struct Symbol *colPtr = head->head, *colPtrBuf;
    printSubpanel(editPanel, head);
    while (runMainLoop) {
        switch (panel) {
            case CONTROL_PANEL:
                curs_set(FALSE);
                key = getch();
                switch (key) {
                    case KEY_TAB:
                        panel = 1;
                        break;
                    case KEY_LEFT:
                        if (selectedButtonControlPanel > 0) {
                            --selectedButtonControlPanel;
                        }
                        setButton(controlPanel, selectedButtonControlPanel, 1, 1);
                        break;
                    case KEY_RIGHT:
                        if (selectedButtonControlPanel < COUNT_BUTTON_PANEL - 1) {
                            ++selectedButtonControlPanel;
                        }
                        setButton(controlPanel, selectedButtonControlPanel, 1, 1);
                        break;
                    case MKEY_ENTER:
                        switch (selectedButtonControlPanel) {
                            case 0:
                                runMainLoop = 0;
                                break;
                            case 1:
                                getFilename(controlPanel, filenameBuffer, FILENAME_MAX);
                                move(30, 5);
                                if (fileCondition) {
                                    close(fileDescriptor);
                                }
                                fileDescriptor = open(filenameBuffer, O_CREAT | O_RDWR);
                                if (fileDescriptor > -1) {
                                    wmove(controlPanel, 3, 1);
                                    wprintw(controlPanel, "file open- %s\n", filenameBuffer);
                                    fileCondition = 1;
                                    deleteTableSymbol(head);
                                    head = readFile(fileDescriptor);
                                    rowPtr = head;
                                    colPtr = head->head;
                                    editPanelPosXCur = 0;
                                    editPanelPosYCur = 0;
                                } else {
                                    wmove(controlPanel, 3, 1);
                                    wprintw(controlPanel, "file not found - %d\n", fileDescriptor);
                                    fileCondition = 0;
                                }
                                break;
                            case 2:
                                if (fileCondition) {
                                    writeFile(fileDescriptor, head);
                                    close(fileDescriptor);
                                    fileDescriptor = open(filenameBuffer, O_CREAT | O_RDWR);
                                } else {
                                    getFilename(controlPanel, filenameBuffer, FILENAME_MAX);
                                    fileDescriptor = open(filenameBuffer, O_CREAT | O_RDWR);
                                    if (fileDescriptor > -1) {
                                        wmove(controlPanel, 3, 1);
                                        wprintw(controlPanel, "file open- %s\n", filenameBuffer);
                                        writeFile(fileDescriptor, head);
                                        close(fileDescriptor);
                                        fileDescriptor = open(filenameBuffer, O_CREAT | O_RDWR);
                                        fileCondition = 1;

                                    } else {
                                        wmove(controlPanel, 3, 1);
                                        wprintw(controlPanel, "error - %s\n", filenameBuffer);
                                        fileCondition = 0;
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
                move(editPanelPosY + editPanelPosYCur + 1, editPanelPosX + editPanelPosXCur + 1);
                key = getch();
                switch (key) {
                    case KEY_TAB:
                        panel = 0;
                        break;
                    case KEY_DOWN:
                        if (rowPtr->next) {
                            rowPtr = rowPtr->next;
                            colPtr = rowPtr->head;
                            editPanelPosXCur = 0;
                            ++editPanelPosYCur;
                        }
                        break;
                    case KEY_UP:
                        if (rowPtr->prev) {
                            rowPtr = rowPtr->prev;
                            colPtr = rowPtr->head;
                            editPanelPosXCur = 0;
                            --editPanelPosYCur;
                        }
                        break;
                    case KEY_LEFT:
                        if (colPtr->prev) {
                            colPtr = colPtr->prev;
                            --editPanelPosXCur;
                        } else if (rowPtr->prev) {
                            rowPtr = rowPtr->prev;
                            colPtr = returnEndSymbol(rowPtr->head, &editPanelPosXCur);
                            --editPanelPosYCur;
                        }
                        break;
                    case KEY_RIGHT:
                        if (colPtr->next) {
                            colPtr = colPtr->next;
                            ++editPanelPosXCur;
                        } else if (rowPtr->next) {
                            rowPtr = rowPtr->next;
                            colPtr = rowPtr->head;
                            editPanelPosXCur = 0;
                            ++editPanelPosYCur;
                        }
                        break;
                    case KEY_SPACE:
                        colPtrBuf = colPtr->next;
                        colPtr->next = createSymbol(' ', colPtr);
                        colPtr->next->next = colPtrBuf;
                        if (colPtrBuf)
                            colPtrBuf->prev = colPtr->next;
                        colPtr = colPtr->next;
                        ++editPanelPosXCur;
                        break;
                    case MKEY_ENTER:
                        if (rowPtr->next) {
                            rowPtrBuf = rowPtr->next;
                            if (colPtr->next) {
                                colPtrBuf = colPtr->next;
                                colPtrBuf->prev = NULL;
                                colPtr->next = NULL;
                            } else {
                                colPtrBuf = createSymbol(' ', NULL);
                            }
                            rowPtr->next = createStringSymbol(colPtrBuf, rowPtr);
                            rowPtr->next->next = rowPtrBuf;
                            rowPtrBuf->prev = rowPtr->next;
                            rowPtr = rowPtr->next;
                            colPtr = rowPtr->head;
                            editPanelPosXCur = 0;
                            ++editPanelPosYCur;
                        } else {
                            if (colPtr->next) {
                                colPtrBuf = colPtr->next;
                                colPtrBuf->prev = NULL;
                                colPtr->next = NULL;
                            } else {
                                colPtrBuf = createSymbol(' ', NULL);
                            }
                            rowPtr->next = createStringSymbol(colPtrBuf, rowPtr);
                            rowPtr = rowPtr->next;
                            colPtr = rowPtr->head;
                            editPanelPosXCur = 0;
                            ++editPanelPosYCur;
                        }
                        break;
                    case KEY_BACKSPACE:
                        if (colPtr->prev) {
                            colPtrBuf = colPtr;
                            colPtr->prev->next = colPtr->next;
                            if (colPtr->next)
                                colPtr->next->prev = colPtr->prev;
                            colPtr = colPtr->prev;
                            free(colPtrBuf);
                            --editPanelPosXCur;
                        } else if (rowPtr->prev) {
                            rowPtrBuf = rowPtr;
                            rowPtr->prev->next = rowPtr->next;
                            if (rowPtr->next) {
                                rowPtr->next->prev = rowPtr->prev;
                            }
                            colPtr = returnEndSymbol(rowPtr->prev->head, &editPanelPosXCur);
                            if (rowPtr->head->next) {
                                colPtr->next = rowPtr->head->next;
                                rowPtr->head->next->prev = colPtr;
                                rowPtr->head->next = NULL;
                            }
                            rowPtr = rowPtr->prev;
                            deleteStringSymbol(rowPtrBuf);
                            --editPanelPosYCur;
                        }
                        break;
                    default:
                        colPtrBuf = colPtr->next;
                        colPtr->next = createSymbol(key, colPtr);
                        colPtr->next->next = colPtrBuf;
                        if (colPtrBuf)
                            colPtrBuf->prev = colPtr->next;
                        colPtr = colPtr->next;
                        ++editPanelPosXCur;
                        break;
                }
                printSubpanel(editPanel, head);
                break;
            default:
                panel = 0;
                break;
        }
    }
    delwin(controlPanel);
    delwin(editPanel);
    endwin();
    exit(EXIT_SUCCESS);
    return 0;
}