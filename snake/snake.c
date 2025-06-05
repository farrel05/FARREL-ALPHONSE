#include "raylib.h"
#include <stdlib.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CELL_SIZE 20
#define MAX_SNAKE_LENGTH 1000

typedef struct {
    int x, y;
} Vector2i;

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Game");
    SetTargetFPS(10);

    Vector2i snake[MAX_SNAKE_LENGTH];
    int snakeLength = 3;
    Vector2i direction = {1, 0};

    // Initialisation de la position du serpent
    for (int i = 0; i < snakeLength; i++) {
        snake[i].x = 10 - i;
        snake[i].y = 10;
    }

    // Fruit
    Vector2i fruit = { GetRandomValue(0, SCREEN_WIDTH / CELL_SIZE - 1),
                       GetRandomValue(0, SCREEN_HEIGHT / CELL_SIZE - 1) };

    bool gameOver = false;
    bool gameStarted = false;
    int score = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (!gameStarted) {
            DrawText("SNAKE GAME", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 30, GREEN);
            DrawText("Appuyez sur ENTREE pour commencer", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, 20, WHITE);
            if (IsKeyPressed(KEY_ENTER)) {
                gameStarted = true;
            }
            EndDrawing();
            continue;
        }

        if (!gameOver) {
            // Contrôles
            if (IsKeyPressed(KEY_UP) && direction.y == 0) direction = (Vector2i){0, -1};
            if (IsKeyPressed(KEY_DOWN) && direction.y == 0) direction = (Vector2i){0, 1};
            if (IsKeyPressed(KEY_LEFT) && direction.x == 0) direction = (Vector2i){-1, 0};
            if (IsKeyPressed(KEY_RIGHT) && direction.x == 0) direction = (Vector2i){1, 0};

            // Déplacement du corps
            for (int i = snakeLength; i > 0; i--) {
                snake[i] = snake[i - 1];
            }
            snake[0].x += direction.x;
            snake[0].y += direction.y;

            // Collision avec fruit
            if (snake[0].x == fruit.x && snake[0].y == fruit.y) {
                snakeLength++;
                score += 10;
                fruit.x = GetRandomValue(0, SCREEN_WIDTH / CELL_SIZE - 1);
                fruit.y = GetRandomValue(0, SCREEN_HEIGHT / CELL_SIZE - 1);
            }

            // Collision avec murs
            if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH / CELL_SIZE ||
                snake[0].y < 0 || snake[0].y >= SCREEN_HEIGHT / CELL_SIZE) {
                gameOver = true;
            }

            // Collision avec soi-même
            for (int i = 1; i < snakeLength; i++) {
                if (snake[i].x == snake[0].x && snake[i].y == snake[0].y) {
                    gameOver = true;
                    break;
                }
            }

            // Dessin fruit
            DrawRectangle(fruit.x * CELL_SIZE, fruit.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);

            // Dessin serpent
            for (int i = 0; i < snakeLength; i++) {
                DrawRectangle(snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE, GREEN);
            }

            // Score
            DrawText(TextFormat("Score: %d", score), 10, 10, 20, YELLOW);
        } else {
            DrawText("GAME OVER", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 40, RED);
            DrawText(TextFormat("Score: %d", score), SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2, 20, WHITE);
            DrawText("Appuyez sur Echapp pour quitter", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 40, 20, GRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

