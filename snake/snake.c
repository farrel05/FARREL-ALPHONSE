// === Snake Game PRO v2 ===
// Features: Menu, Level Selection, High Score Save, Background, Grid, Sound, Music

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CELL_SIZE 20
#define MAX_SNAKE_LENGTH 1000
#define HIGHSCORE_FILE "highscore.txt"

typedef struct { int x, y; } Vector2i;

bool PositionsEqual(Vector2i a, Vector2i b) {
    return a.x == b.x && a.y == b.y;
}

bool IsPositionOnSnake(Vector2i pos, Vector2i snake[], int length) {
    for (int i = 0; i < length; i++) {
        if (PositionsEqual(pos, snake[i])) return true;
    }
    return false;
}

Vector2i GetRandomFruit(Vector2i snake[], int length) {
    Vector2i fruit;
    do {
        fruit.x = GetRandomValue(0, SCREEN_WIDTH / CELL_SIZE - 1);
        fruit.y = GetRandomValue(0, SCREEN_HEIGHT / CELL_SIZE - 1);
    } while (IsPositionOnSnake(fruit, snake, length));
    return fruit;
}

int LoadHighScore() {
    FILE *file = fopen(HIGHSCORE_FILE, "r");
    int score = 0;
    if (file) {
        fscanf(file, "%d", &score);
        fclose(file);
    }
    return score;
}

void SaveHighScore(int score) {
    FILE *file = fopen(HIGHSCORE_FILE, "w");
    if (file) {
        fprintf(file, "%d", score);
        fclose(file);
    }
}

bool Button(Rectangle rect, const char *text) {
    DrawRectangleRec(rect, LIGHTGRAY);
    DrawRectangleLinesEx(rect, 2, DARKGRAY);
    DrawText(text, rect.x + 10, rect.y + 10, 20, BLACK);
    return CheckCollisionPointRec(GetMousePosition(), rect) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake PRO v2");
    InitAudioDevice();
    SetTargetFPS(60);

    Texture2D background = LoadTexture("background.png");
    Sound eatSound = LoadSound("eat.mp3");
    Music music = LoadMusicStream("music.mp3");
    PlayMusicStream(music);

    int startLevel = 1;

menu:
    while (!WindowShouldClose()) {
        UpdateMusicStream(music);

        BeginDrawing();
        ClearBackground(WHITE);
        DrawTexture(background, 0, 0, WHITE);

        DrawText("SNAKE PRO", SCREEN_WIDTH / 2 - 100, 80, 40, DARKGREEN);

        Rectangle playBtn = {SCREEN_WIDTH / 2 - 100, 200, 200, 50};
        Rectangle levelBtn = {SCREEN_WIDTH / 2 - 100, 270, 200, 50};
        Rectangle quitBtn = {SCREEN_WIDTH / 2 - 100, 340, 200, 50};

        if (Button(playBtn, "Jouer")) break;
        if (Button(levelBtn, TextFormat("Niveau: %d", startLevel))) {
            startLevel = (startLevel % 3) + 1; // 1 à 3
        }
        if (Button(quitBtn, "Quitter")) goto end;

        EndDrawing();
    }

    int snakeLength = 5;
    Vector2i snake[MAX_SNAKE_LENGTH];
    Vector2i direction = {1, 0};
    for (int i = 0; i < snakeLength; i++) {
        snake[i] = (Vector2i){10 - i, 10};
    }

    Vector2i fruit = GetRandomFruit(snake, snakeLength);
    int score = 0, level = startLevel, speed = 10 + (level - 1) * 5;
    SetTargetFPS(speed);

    bool gameOver = false;

    while (!gameOver && !WindowShouldClose()) {
        UpdateMusicStream(music);

        if (IsKeyPressed(KEY_UP) && direction.y == 0) direction = (Vector2i){0, -1};
        if (IsKeyPressed(KEY_DOWN) && direction.y == 0) direction = (Vector2i){0, 1};
        if (IsKeyPressed(KEY_LEFT) && direction.x == 0) direction = (Vector2i){-1, 0};
        if (IsKeyPressed(KEY_RIGHT) && direction.x == 0) direction = (Vector2i){1, 0};

        for (int i = snakeLength; i > 0; i--) snake[i] = snake[i - 1];
        snake[0].x += direction.x;
        snake[0].y += direction.y;

        if (PositionsEqual(snake[0], fruit)) {
            PlaySound(eatSound);
            fruit = GetRandomFruit(snake, snakeLength);
            snakeLength++;
            score += 10;
            if (score % 100 == 0) {
                level++;
                speed += 2;
                SetTargetFPS(speed);
            }
        }

        if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH / CELL_SIZE ||
            snake[0].y < 0 || snake[0].y >= SCREEN_HEIGHT / CELL_SIZE) {
            gameOver = true;
        }

        for (int i = 1; i < snakeLength; i++) {
            if (PositionsEqual(snake[0], snake[i])) {
                gameOver = true;
                break;
            }
        }

        BeginDrawing();
        ClearBackground(WHITE);
        DrawTexture(background, 0, 0, WHITE);

        for (int x = 0; x < SCREEN_WIDTH; x += CELL_SIZE)
            DrawLine(x, 0, x, SCREEN_HEIGHT, LIGHTGRAY);
        for (int y = 0; y < SCREEN_HEIGHT; y += CELL_SIZE)
            DrawLine(0, y, SCREEN_WIDTH, y, LIGHTGRAY);

        DrawRectangle(fruit.x * CELL_SIZE, fruit.y * CELL_SIZE, CELL_SIZE, CELL_SIZE, RED);
        for (int i = 0; i < snakeLength; i++) {
            DrawRectangle(snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE, i == 0 ? YELLOW : GREEN);
            DrawRectangleLines(snake[i].x * CELL_SIZE, snake[i].y * CELL_SIZE, CELL_SIZE, CELL_SIZE, DARKGREEN);
        }

        DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);
        DrawText(TextFormat("Niveau: %d", level), 10, 40, 20, DARKGRAY);
        EndDrawing();
    }

    int highScore = LoadHighScore();
    if (score > highScore) SaveHighScore(score);

    while (!WindowShouldClose()) {
        UpdateMusicStream(music);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(background, 0, 0, WHITE);
        DrawText("GAME OVER", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 60, 40, RED);
        DrawText(TextFormat("Score: %d", score), SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2, 20, WHITE);
        DrawText(TextFormat("Meilleur score: %d", (score > highScore) ? score : highScore), SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 30, 20, YELLOW);
        DrawText("ENTREE: Rejouer | ESC: Quitter", SCREEN_WIDTH / 2 - 170, SCREEN_HEIGHT / 2 + 70, 20, GRAY);
        EndDrawing();

        if (IsKeyPressed(KEY_ENTER)) goto menu;
        if (IsKeyPressed(KEY_ESCAPE)) break;
    }

end:
    UnloadSound(eatSound);
    UnloadMusicStream(music);
    UnloadTexture(background);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
