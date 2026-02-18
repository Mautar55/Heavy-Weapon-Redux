#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "raylib.h"
#include "raymath.h"
#include "wutils.h"
#include "generated_assets.h"
#include "game_character.h"
#include "game_globals.h"

extern struct WGlobals w;
extern struct WFonts wfonts;

void UpdateResolutionVars(Camera2D *camera) {
    w.refW = w.ReferenceWindowSize.x;
    w.refH = w.ReferenceWindowSize.y;
    w.rw = (float)GetRenderWidth();
    w.rh = (float)GetRenderHeight();
    w.viewScale = Min(w.rw / w.refW, w.rh / w.refH);

    camera->zoom = w.viewScale;
    camera->target = (Vector2){ w.refW * 0.5f, w.refH * 0.5f };
    camera->offset = (Vector2){ w.rw * 0.5f, w.rh * 0.5f };
}

int main(void) {

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    w.ReferenceWindowSize = (Vector2){800, 480};
    InitWindow(w.ReferenceWindowSize.x, w.ReferenceWindowSize.y, "Hello Raylib");
    SetWindowMinSize(w.ReferenceWindowSize.x, w.ReferenceWindowSize.y);
    SetRandomSeed(time(NULL));

    Camera2D camera = { 0 };
    camera.offset = (Vector2){ 0.0f, 0.0f };
    camera.target = (Vector2){ 0.0f, 0.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    CharacterInitialize();

    SetTargetFPS(60);

    UpdateResolutionVars(&camera);
    LoadFonts(w.viewScale);

    bool pendingResolutionRefresh = false;
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Dynamic resolution management
        UpdateResolutionVars(&camera);

        if (IsWindowResized()) {
            pendingResolutionRefresh = true;
        } else {
            if (pendingResolutionRefresh) {
                UnloadFonts();
                LoadFonts(w.viewScale);
                pendingResolutionRefresh = false;
            }
        }

        CharacterUpdate();

        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(RAYWHITE);

        DrawRectangleLines(0, 0, w.refW, w.refH, RED);

        CharacterDraw();

        DrawTextEx(wfonts.Console, "Hello World from Raylib + CLion! AEIOU aeiou",
            (Vector2){1,1}, wfonts.Console.baseSize/w.viewScale, 0, BLACK);
        DrawTextEx(wfonts.Regular, "Hello World from Raylib + CLion! AEIOU aeiou",
            (Vector2){20,20}, wfonts.Regular.baseSize/w.viewScale, 1, BLACK);
        EndMode2D();
        EndDrawing();
    }
    CharacterUnload();
    UnloadFonts();
    CloseWindow();
    WMemClear();
    return 0;
}
