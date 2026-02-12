#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "random_shapes.h"
#include "raylib.h"
#include "raymath.h"
#include "wutils.h"
#include "generated_assets.h"
#include "wfonts.h"
#include "game_utils.h"

int main(void) {

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    Vector2 ReferenceWindowSize = {800, 480};
    InitWindow(ReferenceWindowSize.x, ReferenceWindowSize.y, "Hello Raylib");
    SetRandomSeed(time(NULL));

    Camera2D camera = { 0 };
    camera.offset = (Vector2){ 0.0f, 0.0f };    // Camera position
    camera.target = (Vector2){ 0.0f, 0.0f };      // Camera looking at point
    camera.rotation = 0.0f;          // Camera up vector (rotation towards target)
    camera.zoom = 1.0f;  // Camera field-of-view Y

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second

    Image Tank = LoadImage(ass_tank_png);
    Texture2D texTank = LoadTextureFromImage(Tank);
    SetTextureFilter(texTank, TEXTURE_FILTER_BILINEAR);
    UnloadImage(Tank);

    LoadFonts(1.0f);

    bool pendingResolutionRefresh = false;
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        const float refW = ReferenceWindowSize.x;
        const float refH = ReferenceWindowSize.y;

        const float rw = (float)GetRenderWidth();
        const float rh = (float)GetRenderHeight();

        const float viewScale = Min(rw / refW, rh / refH);

        if (IsWindowResized()) {
            pendingResolutionRefresh = true;
        } else {
            if (pendingResolutionRefresh) {
                UnloadFonts();
                LoadFonts(viewScale);
                pendingResolutionRefresh = false;
            }
        }
        camera.zoom = viewScale;

        // Center the reference area in the viewport
        camera.target = (Vector2){ refW * 0.5f, refH * 0.5f };
        camera.offset = (Vector2){ rw * 0.5f, rh * 0.5f };

        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(RAYWHITE);

        DrawRectangleLines(0, 0, refW, refH, RED);
        DrawTextureEx(texTank, (Vector2){refW/2-texTank.width/2.0, refH/2 -texTank.height/2.0}, 0.0, 1.0, WHITE);

        DrawTextEx(Wfonts.Console, "Hello World from Raylib + CLion! AEIOU aeiou", (Vector2){1,1}, Wfonts.Console.baseSize/viewScale, 0, BLACK);
        DrawTextEx(Wfonts.Regular, "Hello World from Raylib + CLion! AEIOU aeiou", (Vector2){20,20}, Wfonts.Regular.baseSize/viewScale, 1, BLACK);
        EndMode2D();
        EndDrawing();
    }
    UnloadTexture(texTank);
    UnloadFonts();
    CloseWindow();
    WMemClear();
    return 0;
}
