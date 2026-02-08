#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "random_shapes.h"
#include "raylib.h"
#include "raymath.h"
#include "wutils.h"
#include "generated_assets.h"

int main(void) {

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Hello Raylib");
    SetRandomSeed(time(NULL));

    InitializeRandomShapes();

    

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

        RandomShapes_CheckKeysPressed();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawGrid(10, 1.0f);

        DrawRandomShapes3D();

        EndMode3D();

        DrawRandomShapes2D();

        DrawText("Hello World from Raylib + CLion!", 10, 20, 20, BLACK);
        
        EndDrawing();
    }
    CloseWindow();
    WMemClear();
    return 0;
}