#include "random_shapes.h"
#include "wutils.h"
#include <stdio.h>
#include <string.h>

#include "raymath.h"

declare_list_item(ColoredShape);

WArray circles;
WList cubes_list;

void InitializeRandomShapes() {
    //CircleDataArray circles = {0};
    arr_new(circles_array)
    circles = circles_array;

    for (int i = 0; i < 20; i++) {
        ShapeData c = random_position;
        arr_append(circles, c);
    }

    list_new(cubes_list_new, ColoredShape);
    cubes_list = cubes_list_new;

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
};

void AddRandomShape() {
    ShapeData c = random_position;
    arr_append(circles, c);
};
void RemoveLastShape() {
    if (circles.size > 0) circles.size--;
}

void RandomShapes_CheckKeysPressed() {
    if (IsKeyPressed(KEY_K)) {
        AddRandomShape();
    }
    if (IsKeyPressed(KEY_L)) {
        RemoveLastShape();
    }
};
void DrawRandomShapes3D() {
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
};
void DrawRandomShapes2D() {
    {
        char str[3] = "xxx";
        const int squareSize = 18;
        const int padding    = 6;
        const int startX     = 10;
        int startY     = 50;

        foreach_list(ColoredShape, shape, cubes_list,
            int x = startX + (int)n * (squareSize + padding);
            int y = startY;
            DrawRectangle(x, y, squareSize, squareSize, node.item.color);
            DrawRectangleLines(x, y, squareSize, squareSize, BLACK);
            snprintf(str, sizeof(str), "%zu", idx);
            DrawText(str, x-2 + squareSize / 2, y-2 + squareSize / 2, 10, BLACK);
        );

        startY = 100;
        foreach_list_inverted(ColoredShape, shape, cubes_list,
            int x = startX + (int)n * (squareSize + padding);
            int y = startY;
            DrawRectangle(x, y, squareSize, squareSize, node.item.color);
            DrawRectangleLines(x, y, squareSize, squareSize, BLACK);
            snprintf(str, sizeof(str), "%zu", idx);
            DrawText(str, x-2 + squareSize / 2, y-2 + squareSize / 2, 10, BLACK);
        );
    }
};