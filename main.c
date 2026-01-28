#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include "wutils.h"

int main(void) {
    InitWindow(800, 600, "Hello Raylib");
    SetTargetFPS(60);

    CircleList circles = {0};
    srand(time(NULL));
    for (int i = 0; i < 30; i++) {
        CircleData c = { rand() % 800, rand() % 600 };
        da_append(circles, c);
    }

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_K)) {
            CircleData c = { rand() % GetScreenWidth(), rand() % GetScreenHeight() };
            da_append(circles, c);
        }
        if (IsKeyPressed(KEY_L)) {
            if (circles.size > 0) circles.size--;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello World from Raylib + CLion!", 190, 200, 20, BLACK);
        
        for (size_t i = 0; i < circles.size; i++) {
            DrawCircle(circles.items[i].position_x, circles.items[i].position_y, 5, DARKGREEN);
        }
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}