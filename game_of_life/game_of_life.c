#include <ncurses.h>
#include <stdio.h>

#define ROWS 25
#define COLS 80
#define MAX_FILENAME 100

void print_board(int board[ROWS][COLS]);  // Отображение поля
void read_file(int matrix[ROWS][COLS]);  // Чтение начального состояния из файла
int button_logic(int *delay_ptr);        // Обработка нажатий клавиш
void update_cell(int new_board[ROWS][COLS],
                 int board[ROWS][COLS]);             // Обновление состояния клеток
void update_board(int board[ROWS][COLS]);            // Применение обновлений к полю
int neighbors(int i, int j, int board[ROWS][COLS]);  // Подсчет живых соседей
void get_filename_from_user(char *filename);  // Получение имени файла от пользователя

int main(void) {
    int board[ROWS][COLS];
    int delay = 100;
    int running = 1;

    initscr();              // Инициализация экрана ncurses
    cbreak();               // Отключает буферизацию экрана
    noecho();               // Не отображать вводимые символы
    nodelay(stdscr, TRUE);  // Неблокирующий ввод

    read_file(board);

    while (running) {
        clear();
        print_board(board);

        int speed_percent;
        if (delay <= 10) {
            speed_percent = 100;
        } else if (delay >= 1000) {
            speed_percent = 1;
        } else {
            speed_percent = 100 - ((delay - 10) * 99 / 990);
        }
        mvprintw(ROWS, 0, "Speed: %d (A-faster, Z-slower, SPACE-exit)", speed_percent);
        refresh();

        running = button_logic(&delay);

        update_board(board);
    }

    endwin();
    return 0;
}

void print_board(int board[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (board[i][j] == 1) {
                printw("X");
            } else {
                printw(".");
            }
        }
        printw("\n");
    }
}

void get_filename_from_user(char *filename) {
    int pos = 0;

    mvprintw(0, 0, "Enter filename (press ENTER to finish): ");
    refresh();

    echo();       // Включаем отображение ввода
    nocbreak();   // Временно отключаем cbreak для нормального ввода
    timeout(-1);  // Блокирующий ввод

    flushinp();  // Очистка буфера ввода

    while (1) {
        int ch = getch();

        if (ch == '\n') {
            break;
        } else if (pos < MAX_FILENAME - 1 && ch >= 32 && ch <= 126) {
            filename[pos] = ch;
            pos++;
            filename[pos] = '\0';
        }
    }

    noecho();    // Выключаем отображение ввода
    cbreak();    // Включаем cbreak обратно
    timeout(0);  // Неблокирующий ввод для основного цикла
    flushinp();  // Очищаем буфер ввода
}

void read_file(int matrix[ROWS][COLS]) {
    FILE *fp = NULL;
    char filename[MAX_FILENAME] = {0};

    get_filename_from_user(filename);

    fp = fopen(filename, "r");

    while (fp == NULL) {
        mvprintw(0, 0, "\nFile '%s' not found. Please try again...\n", filename);
        get_filename_from_user(filename);
        fp = fopen(filename, "r");
    }

    for (int i = 0; i < ROWS; i++) {
        char line[COLS + 2];

        if (fgets(line, sizeof(line), fp) != NULL) {
            for (int j = 0; j < COLS; j++) {
                if (line[j] == 'X') {
                    matrix[i][j] = 1;
                } else {
                    matrix[i][j] = 0;
                }
            }
        } else {
            for (int j = 0; j < COLS; j++) {
                matrix[i][j] = 0;
            }
        }
    }

    fclose(fp);

    clear();
    refresh();
}

int button_logic(int *delay_ptr) {
    int ch = getch();

    if (ch != ERR) {
        if (ch == 'A' || ch == 'a') {
            *delay_ptr -= 10;
            if (*delay_ptr < 10) {
                *delay_ptr = 10;
            }
        } else if (ch == 'Z' || ch == 'z') {
            *delay_ptr += 10;
            if (*delay_ptr > 1000) {
                *delay_ptr = 1000;
            }
        } else if (ch == ' ') {
            return 0;
        }
    }

    timeout(*delay_ptr);
    return 1;
}

void update_cell(int new_board[ROWS][COLS], int board[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (board[i][j] == 1 && (neighbors(i, j, board) < 2 || neighbors(i, j, board) > 3)) {
                new_board[i][j] = 0;
            } else if (board[i][j] == 0 && neighbors(i, j, board) == 3) {
                new_board[i][j] = 1;
            } else {
                new_board[i][j] = board[i][j];
            }
        }
    }
}

void update_board(int board[ROWS][COLS]) {
    int new_board[ROWS][COLS];
    update_cell(new_board, board);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            board[i][j] = new_board[i][j];
        }
    }
}

int neighbors(int i, int j, int board[ROWS][COLS]) {
    int live_neighbors = 0;
    for (int k = -1; k <= 1; k++) {
        for (int l = -1; l <= 1; l++) {
            if (k != 0 || l != 0) {
                int x = i + k;
                int y = j + l;

                if (x < 0) {
                    x = ROWS - 1;
                } else if (x >= ROWS) {
                    x = 0;
                }

                if (y < 0) {
                    y = COLS - 1;
                } else if (y >= COLS) {
                    y = 0;
                }

                if (x >= 0 && x < ROWS && y >= 0 && y < COLS) {
                    live_neighbors += board[x][y];
                }
            }
        }
    }
    return live_neighbors;
}
