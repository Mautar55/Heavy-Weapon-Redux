#include "game_character.h"
#include "generated_assets.h"
#include "game_globals.h"

Texture2D texTank;

void CharacterInitialize() {
    Image Tank = LoadImage(ass_tank_png);
    texTank = LoadTextureFromImage(Tank);
    SetTextureFilter(texTank, TEXTURE_FILTER_BILINEAR);
    UnloadImage(Tank);
}

void CharacterCheckKeysPressed() {
    float vel = 0;
    if (IsKeyDown(KEY_A)) {
        vel -= GetFrameTime()*100.0;
    }
    if (IsKeyDown(KEY_D)) {
        vel += GetFrameTime()*100.0;
    }
    main_character_state.ground_velocity = vel;
    main_character_state.ground_position += vel;
}

void CharacterDraw() {
    DrawTextureEx(texTank,
        (Vector2){w.refW/2-texTank.width/2.0+main_character_state.ground_position,
            w.refH-50 -texTank.height/2.0},
            0.0, 1.0, WHITE);
}

void CharacterUnload() {
    UnloadTexture(texTank);
}