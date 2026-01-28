#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"
#include "wutils.h"

#define random_sphere\
    {{\
        (float)(((rand() % 1000)/1000.0f*8)-4),\
        (float)(((rand() % 1000)/1000.0f*1)-0),\
        (float)(((rand() % 1000)/1000.0f*8)-4)\
    }};\

int main(void) {
    InitWindow(800, 600, "Hello Raylib");
    SetTargetFPS(60);

    CircleList circles = {0};
    srand(time(NULL));
    for (int i = 0; i < 30; i++) {
        CircleData c = random_sphere;
        da_append(circles, c);
    }

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_K)) {
            CircleData c = random_sphere;
            da_append(circles, c);
        }
        if (IsKeyPressed(KEY_L)) {
            if (circles.size > 0) circles.size--;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawGrid(10, 1.0f);

        for (size_t i = 0; i < circles.size; i++) {
            DrawSphere(circles.items[i].position, 0.25f, DARKGREEN);
            DrawCircle3D(Vector3Add(circles.items[i].position,(Vector3){0.0,-0.125,0.0}), 0.25f, (Vector3){0,0,1},90.0, DARKGRAY);
        }

        EndMode3D();

        DrawText("Hello World from Raylib + CLion!", 10, 20, 20, BLACK);


        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}