#include "game_character.h"

#include <string.h>

#include "generated_assets.h"
#include "game_globals.h"
#include "raymath.h"
#include "wutils.h"

Texture2D texTank;
WArray projectile_pool;

void CharacterInitialize() {
    Image Tank = LoadImage(ass_tank_png);
    texTank = LoadTextureFromImage(Tank);
    SetTextureFilter(texTank, TEXTURE_FILTER_BILINEAR);
    UnloadImage(Tank);

    character0.lastFire = GetTime();
    character0.ground_position = 0;
    character0.ground_velocity = 0;
    character0.fire_frequency = 0.2;
    character0.ground_max_speed = 250.0;

    arr_init(projectile_pool);
}

void CharacterUpdate() {
    float deltaTime = GetFrameTime();
    double gameTime = GetTime();

    float vel = 0;
    if (IsKeyDown(KEY_A)) {
        vel -= deltaTime*character0.ground_max_speed;
    }
    if (IsKeyDown(KEY_D)) {
        vel += deltaTime*character0.ground_max_speed;
    }
    character0.ground_velocity = vel;
    character0.ground_position += vel;

    int first_inactive = -1;
    for (int i = 0; i < projectile_pool.size; i++) {
        ProjectileState *projectile = &arr_get(ProjectileState, projectile_pool, i);
        if (projectile->active) {
            if (projectile->lifetime_max >= (gameTime - projectile->birth_time)) {
                projectile->position = Vector2Add(projectile->position,Vector2Scale(projectile->velocity,deltaTime));
            } else {
                if (first_inactive < 0) first_inactive = i;
                projectile->active = false;
            }
        } else {
            if (first_inactive < 0) first_inactive = i;
        }

    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && gameTime - character0.lastFire >= character0.fire_frequency) {
        character0.lastFire = gameTime;

        ProjectileState new_proj = {
            .position = Vector2FromToAtDistance(GetCharacterPositionWithOffset((Vector2){0,-10}),GetMousePosition(),25),
            .lifetime_max = 1.0,
            .birth_time = gameTime,
            .radius_v = 12,
            .radius_h = 8,
            .active = true,
        };

        Vector2 new_velocity = Vector2DirectionScaled(new_proj.position,GetMousePosition(),350);
        new_proj.velocity = new_velocity;//Vector2Add(new_velocity,(Vector2){vel/deltaTime,0});

        if (first_inactive >= 0) {
            arr_set(projectile_pool, first_inactive, new_proj);
        } else {
            arr_append(projectile_pool, new_proj);
        }
    }
}

void CharacterDraw() {
    DrawTextureEx(texTank,
        (Vector2){w.refW/2-texTank.width/2.0+character0.ground_position,
            w.refH-50 -texTank.height/2.0},
            0.0, 1.0, WHITE);

    for (int i = 0; i < projectile_pool.size; i++) {
        ProjectileState *projectile = &arr_get(ProjectileState, projectile_pool, i);
        if (projectile->active) {
            //DrawEllipse(projectile->position.x, projectile->position.y, projectile->radius_h, projectile->radius_v, RED);
            DrawRectanglePro((Rectangle){projectile->position.x,projectile->position.y,projectile->radius_h,projectile->radius_v},
                (Vector2){projectile->radius_h/2.0,projectile->radius_v/2.0},
                RAD2DEG*Vector2Angle((Vector2){0,1},Vector2Normalize(projectile->velocity)), RED);
        }

    }
}

void CharacterUnload() {
    UnloadTexture(texTank);
    arr_clear(projectile_pool);
}

Vector2 GetCharacterPosition() {
    return (Vector2){w.refW/2+character0.ground_position, w.refH-50};
}

Vector2 GetCharacterPositionWithOffset(Vector2 offset) {
    return Vector2Add(GetCharacterPosition(),offset) ;
}