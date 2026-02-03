#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"
#include "wutils.h"

#define random_position\
    {\
        (float)(((rand() % 1000)/1000.0f*8)-4),\
        (float)(((rand() % 1000)/1000.0f*1)-0),\
        (float)(((rand() % 1000)/1000.0f*8)-4)\
    }\

#define random_floats\
    (float)(((rand() % 1000)/1000.0f*8)-4),(float)(((rand() % 1000)/1000.0f*1)-0),(float)(((rand() % 1000)/1000.0f*8)-4)

int main(void) {

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Hello Raylib");
    srand(time(NULL));

    //CircleDataArray circles = {0};
    arr_new(circles)

    for (int i = 0; i < 20; i++) {
        ShapeData c = random_position;
        arr_append(circles, c);
    }

    list_new(cubes_list, ColoredShape);

    // fill
    /*for (int i = 0; i < 10; i++) {
        ColoredShapeInList c = {0};
        c.item = (ColoredShape){random_floats, BLUE};
        if (i<=0) {
            c.prev = NULL_OFFSET;
        } else {
            c.prev = i-1;
        }
        if (i>=10-1) {
            c.next = NULL_OFFSET;
        } else {
            c.next = i+1;
        }
        arr_append(cubes_list.items, c);
        cubes_list.list_size++;
    }
    cubes_list.starting_index = 0;*/

    //list_insert_at(&cubes_list, &(ColoredShape){random_floats, LIME}, 0);

    for (int i = 0; i < 10; i++) {

        list_insert_last(&cubes_list, &(ColoredShape){random_floats, BLUE});
    }

    ColoredShape to_add = {0};
    to_add.position = (Vector3)random_position;
    to_add.color = RED;
    WList* target_list = &cubes_list;
    list_insert_at(target_list, &to_add, 10);

    to_add.position = (Vector3)random_position;
    to_add.color = GREEN;
    list_insert_at(target_list, &to_add, 0);

    to_add.position = (Vector3)random_position;
    to_add.color = PINK;
    list_insert_at(target_list, &to_add, 5);

    list_remove_first(target_list);
    list_remove_last(target_list);

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
            ShapeData c = random_position;
            arr_append(circles, c);
        }
        if (IsKeyPressed(KEY_L)) {
            if (circles.size > 0) circles.size--;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawGrid(10, 1.0f);

        for (size_t i = 0; i < circles.size; i++) {
            DrawSphere(arr_get(ShapeData,circles,i).position, 0.25f, DARKGREEN);
            DrawCircle3D(
                Vector3Multiply((Vector3){1.0,0.0,1.0},
                Vector3Add(arr_get(ShapeData,circles,i).position,(Vector3){0.0,-0.25,0.0})
                )
                , 0.25f
                , (Vector3){1,0,0}
                ,90.0
                , DARKGRAY);
        }

        EndMode3D();

        {
            char str[3] = "xxx";
            const int squareSize = 18;
            const int padding    = 6;
            const int startX     = 10;
            const int startY     = 50;

            size_t idx = cubes_list.starting_index;
            for (size_t n = 0; n < cubes_list.list_size && idx != NULL_OFFSET; n++) {
                ColoredShapeInList node = arr_get(ColoredShapeInList, cubes_list.items, idx);

                int x = startX + (int)n * (squareSize + padding);
                int y = startY;

                DrawRectangle(x, y, squareSize, squareSize, node.item.color);
                DrawRectangleLines(x, y, squareSize, squareSize, BLACK);
                snprintf(str, sizeof(str), "%zu", idx);
                DrawText(str, x-2 + squareSize / 2, y-2 + squareSize / 2, 10, BLACK);

                idx = node.next;
            }
        }

        DrawText("Hello World from Raylib + CLion!", 10, 20, 20, BLACK);


        
        EndDrawing();
    }
    CloseWindow();
    WMemClear();
    return 0;
}