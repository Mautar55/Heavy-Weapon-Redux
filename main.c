#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Hello Raylib");

    //CircleDataArray circles = {0};
    da_new(circles)

    srand(time(NULL));
    for (int i = 0; i < 30; i++) {
        CircleData c = random_sphere;
        da_append(circles, c);
        // ARRAY , NEW_ITEM
        // ((WMemRef*)(WMemGetStart[(array).items_ref])).ptr[(array).size++] = new_item;
        //WMemRef *ref = WMemRefFromOffset(circles.items_ref);
        //size_t dest_i = circles.size++;
        //size_t elem_size = sizeof c;
        //memcpy((char*)ref->ptr + dest_i * elem_size, &c, elem_size);
    }

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

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
            DrawSphere(da_get(CircleData,circles,i).position, 0.25f, DARKGREEN);
            DrawCircle3D(Vector3Add(da_get(CircleData,circles,i).position,(Vector3){0.0,-0.25,0.0})
                , 0.25f
                , (Vector3){1,0,0}
                ,90.0
                , DARKGRAY);
        }

        EndMode3D();

        DrawText("Hello World from Raylib + CLion!", 10, 20, 20, BLACK);


        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}