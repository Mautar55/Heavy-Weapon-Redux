#include "game_character.h"

#include <string.h>

#include "generated_assets.h"
#include "game_globals.h"
#include "raymath.h"
#include "wutils.h"
#include "tank_bullet_atlas.h"

const float ENEMY_BOMB_SPEED = 150.0f;
int first_player_bullet_inactive = -1;

Texture2D texTank;
Texture2D texProjectileAtlas;
Texture2D texFragBombAtlas;
WArray player_projectile_pool;
WArray enemy_bombs_pool; // pending implement arr_init_after or something

void CharacterInitialize() {
    Image Tank = LoadImage(ass_tank_png);
    texTank = LoadTextureFromImage(Tank);
    SetTextureFilter(texTank, TEXTURE_FILTER_BILINEAR);
    UnloadImage(Tank);

    Image ProjectileAtlas = LoadImage(ass_projectiles_tank_bullet_atlas_png);
    texProjectileAtlas = LoadTextureFromImage(ProjectileAtlas);
    SetTextureFilter(texProjectileAtlas, TEXTURE_FILTER_BILINEAR);
    UnloadImage(ProjectileAtlas);

    Image FragBombAtlas = LoadImage(ass_projectiles_fragbomb_png);
    texFragBombAtlas = LoadTextureFromImage(FragBombAtlas);
    SetTextureFilter(texFragBombAtlas, TEXTURE_FILTER_BILINEAR);
    UnloadImage(FragBombAtlas);

    character0.lastFire = GetTime();
    character0.ground_position = 0;
    character0.ground_velocity = 0;
    character0.fire_frequency = 0.2;
    character0.ground_max_speed = 250.0;
    character0.ground_extents = 375;
    character0.bullet_damage_tier = 0;

    arr_init(player_projectile_pool);
    arr_init(enemy_bombs_pool);
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

    int first_player_bullet_inactive = -1;
    for (int i = 0; i < player_projectile_pool.size; i++) {
        ProjectileState *projectile = &arr_get(ProjectileState, player_projectile_pool, i);
        if (projectile->active) {
            if (projectile->lifetime_max >= (gameTime - projectile->birth_time)) {
                projectile->position = Vector2Add(projectile->position,Vector2Scale(projectile->velocity,deltaTime));
            } else {
                if (first_player_bullet_inactive < 0) first_player_bullet_inactive = i;
                projectile->active = false;
            }
        } else {
            if (first_player_bullet_inactive < 0) first_player_bullet_inactive = i;
        }
    }

    if (IsKeyPressed(KEY_P)) {
        character0.bullet_damage_tier++;
    }
    if (IsKeyPressed(KEY_O)) {
        character0.bullet_damage_tier--;
    }

    character0.bullet_damage_tier = Clamp(character0.bullet_damage_tier,0,4);

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && gameTime - character0.lastFire >= character0.fire_frequency) {
        character0.lastFire = gameTime;

        SpawnPlayerBullet(0, character0.bullet_damage_tier);
    }

    TestFallingBombs();
}

void SpawnPlayerBullet(float AngleOffset, char bulletLevel) {
    ProjectileState new_proj = {
        .position = Vector2FromToAtDistance(GetCharacterPositionWithOffset((Vector2){0,-10}),GetMousePositionInFrame(),25),
        .lifetime_max = 1.0,
        .birth_time = GetTime(),
        .radius_v = 20,
        .radius_h = 15,
        .active = true,
        .ornament = bulletLevel
    };

    Vector2 new_velocity = Vector2Rotate(Vector2DirectionScaled(new_proj.position,GetMousePositionInFrame(),350), AngleOffset);
    new_proj.velocity = new_velocity;//Vector2Add(new_velocity,(Vector2){vel/deltaTime,0});

    if (first_player_bullet_inactive >= 0) {
        arr_set(player_projectile_pool, first_player_bullet_inactive, new_proj);
    } else {
        arr_append(player_projectile_pool, new_proj);
    }
}

void TestFallingBombs() {
    float deltaTime = GetFrameTime();
    double gameTime = GetTime();

    int first_inactive = -1;
    for (int i = 0; i < enemy_bombs_pool.size; i++) {
        ProjectileState *bomb = &arr_get(ProjectileState, enemy_bombs_pool, i);
        if (bomb->active) {
            if (bomb->lifetime_max >= (gameTime - bomb->birth_time)) {
                bomb->position = Vector2Add(bomb->position,Vector2Scale(bomb->velocity,deltaTime));
            } else {
                if (first_inactive < 0) first_inactive = i;
                bomb->active = false;
            }
        } else {
            if (first_inactive < 0) first_inactive = i;
        }
    }

    static double last_bomb_test_time = 0;
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && IsKeyDown(KEY_Q) && gameTime - last_bomb_test_time >= 0.2) {
        last_bomb_test_time = gameTime;

        ProjectileState new_proj = {
            .position = GetMousePositionInFrame(),
            .lifetime_max = 2.0,
            .birth_time = gameTime,
            .radius_v = 20,
            .radius_h = 15,
            .active = true,
            .ornament = 0
        };

        Vector2 new_velocity = Vector2Rotate((Vector2){0,ENEMY_BOMB_SPEED}, random_half_angle-(3.14159/2));
        new_proj.velocity = new_velocity;//Vector2Add(new_velocity,(Vector2){vel/deltaTime,0});

        if (first_inactive >= 0) {
            arr_set(enemy_bombs_pool, first_inactive, new_proj);
        } else {
            arr_append(enemy_bombs_pool, new_proj);
        }
    }
}

void CharacterDraw() {
    DrawTextureEx(texTank,
        (Vector2){w.refW/2-texTank.width/2.0+character0.ground_position,
            w.refH-50 -texTank.height/2.0},
            0.0, 1.0, WHITE);

    // DRAWING PLAYER PROJECTILES

    for (int i = 0; i < player_projectile_pool.size; i++) {
        ProjectileState *projectile = &arr_get(ProjectileState, player_projectile_pool, i);
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

            rtpAtlasSprite texBullet = BulletAtlas[projectile->ornament];

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

    // DRAWING ENEMY BOMBS

    for (int i = 0; i < enemy_bombs_pool.size; i++) {
        ProjectileState *bomb = &arr_get(ProjectileState, enemy_bombs_pool, i);
        if (bomb->active) {
            //DrawEllipse(projectile->position.x, projectile->position.y, projectile->radius_h, projectile->radius_v, RED);

            //DrawRectanglePro(
            //    (Rectangle){
            //        projectile->position.x,
            //        projectile->position.y,
            //        projectile->radius_v,
            //        projectile->radius_v},
            //    (Vector2){
            //        projectile->radius_v/2.0,
            //        projectile->radius_v/2.0},
            //    RAD2DEG*Vector2Angle((Vector2){0,1},Vector2Normalize(projectile->velocity)),
            //    DARKGRAY);
            //DrawRectanglePro(
            //    (Rectangle){
            //        projectile->position.x,
            //        projectile->position.y,
            //        projectile->radius_h,
            //        projectile->radius_v},
            //    (Vector2){
            //        projectile->radius_h/2.0,
            //        projectile->radius_v/2.0},
            //    RAD2DEG*Vector2Angle((Vector2){0,1},Vector2Normalize(projectile->velocity)),
            //    BLACK);

            Texture2D texBomb = texFragBombAtlas; //TrimmedFromBombAtlas(5, texFragBombAtlas);

                // Atlas has 10 frames covering 0..90 degrees (right -> down).
                Vector2 v = bomb->velocity;
                float len = Vector2Length(v);
                if (len < 0.0001f) v = (Vector2){ 1.0f, 0.0f };
                else v = Vector2Scale(v, 1.0f/len);

                float actual_angle = atan2f(v.y, v.x);              // radians, 0=right, +pi/2=down (raylib screen coords)
                bool flipX = false;

                // Mirror left-facing directions into the right-facing atlas range and remember to flip the sprite.
                if (fabsf(actual_angle) > PI*0.5f) {
                    flipX = true;
                    actual_angle = PI - actual_angle;               // maps e.g. 135° -> 45°
                }

                // Clamp to the atlas-supported range (right->down).
                actual_angle = Clamp(actual_angle, 0.0f, PI*0.5f);

                const float step = (PI*0.5f) / 9.0f;                // 10 frames -> 9 intervals
                int spriteIndex = (int)lroundf(actual_angle / step); // nearest frame
                spriteIndex = (int)Clamp((float)spriteIndex, 0.0f, 9.0f);

                float sprite_angle_rad = spriteIndex * step;        // angle that the chosen sprite frame represents
                float extra_rot_deg = RAD2DEG * (actual_angle - sprite_angle_rad);

                Rectangle rectSrc = TrimmedFromBombAtlas(spriteIndex, texBomb);

                // Flip horizontally if needed (raylib: negative source width).
                if (flipX) {
                    rectSrc.width *= -1.0f;
                }

                // Destination rectangle (screen rectangle where drawing part of texture)
                Rectangle destRec = {
                    bomb->position.x,
                    bomb->position.y,
                    fabsf(rectSrc.width),
                    fabsf(rectSrc.height)
                };

                DrawTexturePro(texBomb, rectSrc, destRec,
                    (Vector2){destRec.width/2.0f, destRec.height/2.0f},
                    extra_rot_deg,
                    WHITE);
        }
    }
}

void CharacterUnload() {
    UnloadTexture(texTank);
    arr_clear(player_projectile_pool);
}

Vector2 GetCharacterPosition() {
    return (Vector2){w.refW/2+character0.ground_position, w.refH-50};
}

Vector2 GetCharacterPositionWithOffset(Vector2 offset) {
    return Vector2Add(GetCharacterPosition(),offset) ;
}

int CharacterGetBulletDamageTier(void) {
    return (int)character0.bullet_damage_tier;
}