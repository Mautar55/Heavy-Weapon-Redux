#include "game_character.h"

#include <string.h>

#include "generated_assets.h"
#include "game_globals.h"
#include "raymath.h"
#include "wutils.h"
#include "tank_bullet_atlas.h"

Texture2D texTank;
Texture2D texProjectileAtlas;
WArray projectile_pool;

void CharacterInitialize() {
    Image Tank = LoadImage(ass_tank_png);
    texTank = LoadTextureFromImage(Tank);
    SetTextureFilter(texTank, TEXTURE_FILTER_BILINEAR);
    UnloadImage(Tank);

    Image ProjectileAtlas = LoadImage(ass_projectiles_tank_bullet_atlas_png);
    texProjectileAtlas = LoadTextureFromImage(ProjectileAtlas);
    SetTextureFilter(texProjectileAtlas, TEXTURE_FILTER_BILINEAR);
    UnloadImage(ProjectileAtlas);

    character0.lastFire = GetTime();
    character0.ground_position = 0;
    character0.ground_velocity = 0;
    character0.fire_frequency = 0.2;
    character0.ground_max_speed = 250.0;
    character0.ground_extents = 375;

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
    character0.ground_position = Clamp(character0.ground_position,-character0.ground_extents,character0.ground_extents);

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
            .position = Vector2FromToAtDistance(GetCharacterPositionWithOffset((Vector2){0,-10}),GetMousePositionInFrame(),25),
            .lifetime_max = 1.0,
            .birth_time = gameTime,
            .radius_v = 16,
            .radius_h = 10,
            .active = true,
        };

        Vector2 new_velocity = Vector2DirectionScaled(new_proj.position,GetMousePositionInFrame(),200);
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


    rtpAtlasSprite texBullet = BulletAtlas[TankBullet1];

    for (int i = 0; i < projectile_pool.size; i++) {
        ProjectileState *projectile = &arr_get(ProjectileState, projectile_pool, i);
        if (projectile->active) {
            //DrawEllipse(projectile->position.x, projectile->position.y, projectile->radius_h, projectile->radius_v, RED);

            DrawRectanglePro(
                (Rectangle){
                    projectile->position.x,
                    projectile->position.y,
                    projectile->radius_v,
                    projectile->radius_v},
                (Vector2){
                    projectile->radius_v/2.0,
                    projectile->radius_v/2.0},
                RAD2DEG*Vector2Angle((Vector2){0,1},Vector2Normalize(projectile->velocity)),
                DARKGRAY);

            DrawRectanglePro(
                (Rectangle){
                    projectile->position.x,
                    projectile->position.y,
                    projectile->radius_h,
                    projectile->radius_v},
                (Vector2){
                    projectile->radius_h/2.0,
                    projectile->radius_v/2.0},
                RAD2DEG*Vector2Angle((Vector2){0,1},Vector2Normalize(projectile->velocity)),
                BLACK);

            // part of the texture
            const Rectangle rectSrc = { texBullet.positionX, texBullet.positionY, texBullet.sourceWidth, texBullet.sourceHeight};

            // Destination rectangle (screen rectangle where drawing part of texture)
            Rectangle destRec = {
                projectile->position.x,
                projectile->position.y,
                texBullet.sourceWidth,
                texBullet.sourceHeight
            };

            const float angle = RAD2DEG*Vector2Angle((Vector2){0,1},Vector2Normalize(projectile->velocity));
            // Origin of the texture (rotation/scale point), it's relative to destination rectangle size
            //Vector2 origin = { (float)frameWidth, (float)frameHeight };

            DrawTexturePro(texProjectileAtlas, rectSrc, destRec, (Vector2){texBullet.sourceWidth/2.0,texBullet.sourceHeight/2.0}, angle, WHITE);
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