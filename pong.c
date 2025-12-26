#include <stdio.h>

#define WIDTH 80
#define HEIGHT 25
#define RACKET_LENGTH 3
#define WIN_SCORE 21

// Функция отрисовки поля
void render_field(int player1_racket_y, int player2_racket_y, int ball_x, int ball_y, int player1_points,
                  int player2_points, int current_player) {
    // Очистка экрана (альтернатива system("clear"))
    printf("\033[2J\033[1;1H");

    // Отрисовка верхней границы
    for (int i = 0; i < WIDTH + 2; i++) {
        printf("#");
    }
    printf("\n");

    // Отрисовка игрового поля без верхней и нижней границы
    for (int y = 0; y < HEIGHT; y++) {
        printf("#");  // Левая граница

        for (int x = 0; x < WIDTH; x++) {
            // Ракетка игрока 1 (слева)
            if (x == 1 && y >= player1_racket_y && y < player1_racket_y + RACKET_LENGTH) {
                printf("|");
            }
            // Ракетка игрока 2 (справа)
            else if (x == WIDTH - 2 && y >= player2_racket_y && y < player2_racket_y + RACKET_LENGTH) {
                printf("|");
            }
            // Мяч
            else if (x == ball_x && y == ball_y) {
                printf("O");
            }
            // Сетка посередине (пунктирная)
            else if (x == WIDTH / 2 && y % 2 == 0) {
                printf(".");
            }
            // Пустое пространство
            else {
                printf(" ");
            }
        }

        printf("#\n");  // Правая граница
    }

    // Отрисовка нижней границы
    for (int i = 0; i < WIDTH + 2; i++) {
        printf("#");
    }
    printf("\n");

    // Отрисовка счета
    printf("Player 1: %d", player1_points);
    for (int i = 0; i < WIDTH - 25; i++) {
        printf(" ");
    }
    printf("Player 2: %d\n", player2_points);

    // Отрисовка инструкции по управлению
    printf("\nControls: A/Z (P1 up/down)  K/M (P2 up/down)  SPACE (skip)\n");

    // Отрисовка информации по текущему ходу
    printf("\nCurrent move: Player %d. Waiting for input: ", current_player);
}

// Функция движения ракетки
int move_racket(int racket_y, char button, char up_key, char down_key) {
    if (button == up_key) {
        if (racket_y > 0) {
            return racket_y - 1;
        }
    } else if (button == down_key) {
        if (racket_y < HEIGHT - RACKET_LENGTH) {
            return racket_y + 1;
        }
    }
    return racket_y;
}

// Функция проверки счета и определения победителя
int check_score(int player1_points, int player2_points) {
    if (player1_points >= WIN_SCORE) {
        printf("\n\n========================================\n");
        printf("   Player 1 wins! Congratulations!\n");
        printf("========================================\n");
        return 1;
    }
    if (player2_points >= WIN_SCORE) {
        printf("\n\n========================================\n");
        printf("   Player 2 wins! Congratulations!\n");
        printf("========================================\n");
        return 1;
    }
    return 0;
}

// Функция для обновления состояния мяча по оси x
int update_ball_x(int ball_x, int ball_dx) { return ball_x + ball_dx; }

// Функция для обновления состояния мяча по оси y
int update_ball_y(int ball_y, int ball_dy) { return ball_y + ball_dy; }

// Функция по проверке необходимости в отражении мяча
int check_ball_collision_top_bottom(int ball_y, int ball_dy) {
    if (ball_y <= 0) {
        return 1;
    } else if (ball_y >= HEIGHT - 1) {
        return -1;
    }
    return ball_dy;
}

// Функция проверки гола игрока 1
int check_player1_goal(int ball_x, int ball_y, int player1_racket_y) {
    if (ball_x <= 1) {
        if (ball_y >= player1_racket_y && ball_y < player1_racket_y + RACKET_LENGTH) {
            return 1;  // отскок от ракетки
        } else {
            return 2;  // гол (игроку 2 добавляется очко)
        }
    }
    return 0;  // ничего не произошло
}

// Функция проверки гола игрока 2
int check_player2_goal(int ball_x, int ball_y, int player2_racket_y) {
    if (ball_x >= WIDTH - 2) {
        if (ball_y >= player2_racket_y && ball_y < player2_racket_y + RACKET_LENGTH) {
            return 1;  // отскок от ракетки
        } else {
            return 2;  // гол (игроку 1 добавляется очко)
        }
    }
    return 0;  // ничего не произошло
}

int main() {
    int player1_racket_y = 11;  // положение ракетки игрока 1 (Y координата)
    int player2_racket_y = 11;  // положение ракетки игрока 2 (Y координата)

    int ball_x = 40, ball_y = 12;   // расположение мяча
    int ball_dx = -1, ball_dy = 1;  // направление мяча

    int player1_points = 0, player2_points = 0;  // Очки игроков
    char input;                                  // Введенный символ
    int game_over = 0;                           // Флаг признака окончания игры
    int current_player = 1;                      // Номер текущего игрока
    // int valid_move = 0;                          // Флаг корректного хода

    // Начальная отрисовка поля
    render_field(player1_racket_y, player2_racket_y, ball_x, ball_y, player1_points, player2_points,
                 current_player);

    // Основной игровой цикл
    while (game_over == 0) {
        int valid_move = 0;

        // Ждем корректного ввода от текущего игрока
        while (valid_move == 0) {
            scanf("%c", &input);

            // Проверяем, может ли текущий игрок сделать такой ход
            if (current_player == 1) {
                // Player 1 может нажимать A, Z или Space
                if (input == 'a' || input == 'A' || input == 'z' || input == 'Z') {
                    player1_racket_y = move_racket(player1_racket_y, input, 'A', 'Z');
                    valid_move = 1;
                } else if (input == ' ') {
                    // Space - пропуск хода
                    valid_move = 1;
                }
            } else {
                // Player 2 может нажимать K, M или Space
                if (input == 'k' || input == 'K' || input == 'm' || input == 'M') {
                    player2_racket_y = move_racket(player2_racket_y, input, 'K', 'M');
                    valid_move = 1;
                } else if (input == ' ') {
                    // Space - пропуск хода
                    valid_move = 1;
                }
            }

            // Отрисовка поля после ввода (только если был сделан валидный ход)
            if (valid_move == 1) {
                render_field(player1_racket_y, player2_racket_y, ball_x, ball_y, player1_points,
                             player2_points, current_player);
            }
        }

        // Меняем игрока
        if (current_player == 1) {
            current_player = 2;
        } else {
            current_player = 1;
        }

        // Движение мяча
        ball_x = update_ball_x(ball_x, ball_dx);
        ball_y = update_ball_y(ball_y, ball_dy);

        // Проверка столкновения с верхней/нижней границей
        ball_dy = check_ball_collision_top_bottom(ball_y, ball_dy);

        // Проверка столкновения с ракеткой игрока 1 или гола
        int collision_result = check_player1_goal(ball_x, ball_y, player1_racket_y);
        if (collision_result == 1) {
            ball_dx = 1;
            ball_x = 2;
        } else if (collision_result == 2) {
            // Начисляем поинт игроку 2
            player2_points++;
            // Сбрасываем мяч в центр
            ball_x = WIDTH / 2;
            ball_y = HEIGHT / 2;
            ball_dx = 1;  // Летит к Player 2
            if (ball_y > HEIGHT / 2) {
                ball_dy = -1;
            } else {
                ball_dy = 1;
            }
        }

        // Проверка столкновения с ракеткой игрока 2 или гола
        collision_result = check_player2_goal(ball_x, ball_y, player2_racket_y);
        if (collision_result == 1) {
            ball_dx = -1;
            ball_x = WIDTH - 3;
        } else if (collision_result == 2) {
            player1_points++;
            // Сбрасываем мяч в центр
            ball_x = WIDTH / 2;
            ball_y = HEIGHT / 2;
            ball_dx = -1;  // Летит к Player 1
            if (ball_y > HEIGHT / 2) {
                ball_dy = -1;
            } else {
                ball_dy = 1;
            }
        }

        // Отрисовка поля с новым текущим игроком
        render_field(player1_racket_y, player2_racket_y, ball_x, ball_y, player1_points, player2_points,
                     current_player);

        // Проверка счета
        game_over = check_score(player1_points, player2_points);
    }

    return 0;
}