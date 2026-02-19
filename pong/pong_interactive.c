// Запуск: clang -std=c11 -Wall -Wextra -Werror -o code pong_interactive.c -lncurses && ./code

#include <ncurses.h>
#include <stdio.h>

#define WIDTH 80
#define HEIGHT 25
#define RACKET_LENGTH 3
#define WIN_SCORE 21
#define SPEED 100

// Функция отрисовки поля
void render_field(int player1_racket_y, int player2_racket_y, int ball_x, int ball_y, int player1_points,
                  int player2_points) {
    clear();

    // Отрисовка верхней границы
    for (int i = 0; i < WIDTH + 2; i++) {
        mvaddch(0, i, '#');
    }

    // Игровое поле
    for (int y = 0; y < HEIGHT; y++) {
        mvaddch(y + 1, 0, '#');  // Левая граница

        for (int x = 0; x < WIDTH; x++) {
            int screen_x = x + 1;
            int screen_y = y + 1;

            // Ракетка игрока 1 (слева)
            if (x == 1 && y >= player1_racket_y && y < player1_racket_y + RACKET_LENGTH) {
                mvaddch(screen_y, screen_x, '|');
            }
            // Ракетка игрока 2 (справа)
            else if (x == WIDTH - 2 && y >= player2_racket_y && y < player2_racket_y + RACKET_LENGTH) {
                mvaddch(screen_y, screen_x, '|');
            }
            // Мяч
            else if (x == ball_x && y == ball_y) {
                mvaddch(screen_y, screen_x, 'O');
            }
            // Сетка посередине (пунктирная)
            else if (x == WIDTH / 2 && y % 2 == 0) {
                mvaddch(screen_y, screen_x, '.');
            }
            // Пустое пространство
            else {
                mvaddch(screen_y, screen_x, ' ');
            }
        }

        mvaddch(y + 1, WIDTH + 1, '#');  // Правая граница
    }

    // Нижняя граница
    for (int i = 0; i < WIDTH + 2; i++) {
        mvaddch(HEIGHT + 1, i, '#');
    }

    // Счет
    mvprintw(HEIGHT + 3, 0, "Player 1: %d", player1_points);
    mvprintw(HEIGHT + 3, WIDTH - 10, "Player 2: %d", player2_points);

    // Управление
    mvprintw(HEIGHT + 5, 0, "Controls: A/Z (P1 up/down)  K/M (P2 up/down)  Q (quit)");

    refresh();
}

// Функция проверки счета
int check_score(int player1_points, int player2_points) {
    if (player1_points >= WIN_SCORE || player2_points >= WIN_SCORE) {
        return 1;
    }
    return 0;
}

// Функции для обновления состояния мяча
int update_ball_x(int ball_x, int ball_dx) { return ball_x + ball_dx; }

int update_ball_y(int ball_y, int ball_dy) { return ball_y + ball_dy; }

int check_ball_collision_top_bottom(int ball_y, int ball_dy) {
    if (ball_y <= 0) {
        return 1;
    } else if (ball_y >= HEIGHT - 1) {
        return -1;
    }
    return ball_dy;
}

// Функция проверки гола
int check_player1_goal(int ball_x, int ball_y, int player1_racket_y) {
    if (ball_x <= 1) {
        if (ball_y >= player1_racket_y && ball_y < player1_racket_y + RACKET_LENGTH) {
            return 1;  // отскок
        } else {
            return 2;  // гол
        }
    }
    return 0;
}

int check_player2_goal(int ball_x, int ball_y, int player2_racket_y) {
    if (ball_x >= WIDTH - 2) {
        if (ball_y >= player2_racket_y && ball_y < player2_racket_y + RACKET_LENGTH) {
            return 1;  // отскок
        } else {
            return 2;  // гол
        }
    }
    return 0;
}

int main() {
    initscr();  // Инициализация ncurses
    raw();  // Отключает буферизацию строк (символы приходят сразу, а не после Enter)
    noecho();  // Запрещает автоматический вывод символов на экран
    nodelay(stdscr, TRUE);  // Делает getch() неблокирующей
    curs_set(0);            // Скрывает мигающий курсор терминала

    // Переменные состояния
    int player1_racket_y = HEIGHT / 2 - RACKET_LENGTH / 2;
    int player2_racket_y = HEIGHT / 2 - RACKET_LENGTH / 2;
    int ball_x = WIDTH / 2;
    int ball_y = HEIGHT / 2;
    int ball_dx = -1;
    int ball_dy = 1;
    int player1_points = 0;
    int player2_points = 0;
    int game_over = 0;

    // Начальная отрисовка
    render_field(player1_racket_y, player2_racket_y, ball_x, ball_y, player1_points, player2_points);

    // Основной игровой цикл
    while (!game_over) {
        // Обработка ввода
        int ch = getch();

        // Движение ракетки игрока 1
        if (ch == 'a' || ch == 'A') {
            if (player1_racket_y > 0) {
                player1_racket_y--;
            }
        } else if (ch == 'z' || ch == 'Z') {
            if (player1_racket_y < HEIGHT - RACKET_LENGTH) {
                player1_racket_y++;
            }
        }

        // Движение ракетки игрока 2
        if (ch == 'k' || ch == 'K') {
            if (player2_racket_y > 0) {
                player2_racket_y--;
            }
        } else if (ch == 'm' || ch == 'M') {
            if (player2_racket_y < HEIGHT - RACKET_LENGTH) {
                player2_racket_y++;
            }
        }

        // Выход из игры
        if (ch == 'q' || ch == 'Q') {
            game_over = 1;
        }

        // Движение мяча
        ball_x = update_ball_x(ball_x, ball_dx);
        ball_y = update_ball_y(ball_y, ball_dy);

        // Проверка столкновений
        ball_dy = check_ball_collision_top_bottom(ball_y, ball_dy);

        int collision_result = check_player1_goal(ball_x, ball_y, player1_racket_y);
        if (collision_result == 1) {
            ball_dx = 1;
            ball_x = 2;
        } else if (collision_result == 2) {
            player2_points++;
            ball_x = WIDTH / 2;
            ball_y = HEIGHT / 2;
            ball_dx = 1;
            ball_dy = (ball_y > HEIGHT / 2) ? -1 : 1;
        }

        collision_result = check_player2_goal(ball_x, ball_y, player2_racket_y);
        if (collision_result == 1) {
            ball_dx = -1;
            ball_x = WIDTH - 3;
        } else if (collision_result == 2) {
            player1_points++;
            ball_x = WIDTH / 2;
            ball_y = HEIGHT / 2;
            ball_dx = -1;
            ball_dy = (ball_y > HEIGHT / 2) ? -1 : 1;
        }

        // Отрисовка
        render_field(player1_racket_y, player2_racket_y, ball_x, ball_y, player1_points, player2_points);

        // Проверка завершения игры
        if (check_score(player1_points, player2_points)) {
            nodelay(stdscr, FALSE);  // Переключаемся в блокирующий режим
            clear();
            if (player1_points >= WIN_SCORE) {
                mvprintw(HEIGHT / 2, WIDTH / 2 - 10, "Player 1 wins! Congratulations!");
            } else {
                mvprintw(HEIGHT / 2, WIDTH / 2 - 10, "Player 2 wins! Congratulations!");
            }
            mvprintw(HEIGHT / 2 + 2, WIDTH / 2 - 10, "Press any key to exit...");
            refresh();
            getch();
            game_over = 1;
        }

        // Контроль скорости
        napms(SPEED);
    }

    endwin();
    return 0;
}
