#include "game_character.h"

#include <string.h>
#include <math.h>

#include "generated_assets.h"
#include "game_globals.h"
#include "raymath.h"
#include "wutils.h"
#include "tank_bullet_atlas.h"
#include "wmath.h"

int first_player_bullet_inactive = -1;

Texture2D texTank;
Texture2D texProjectileAtlas;
Texture2D texFragBombAtlas;
WList player_projectile_pool;
WList enemy_bombs_pool; 

WArray PlayerBulletCollisions;
WArray EnemyBombCollisions;

#define groundHeight\
    (w.refH-50)

// Helper functions for loops
static float deltaTime_loop;
static double gameTime_loop;
static Vector2 playerPos_loop;
static float playerRadius_loop;

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
    character0.fire_frequency = 0.2f;
    character0.ground_max_speed = 250.0f;
    character0.ground_extents = 375;
    character0.bullet_damage_tier = 0;

    list_init(player_projectile_pool, ProjectileState);
    list_init(enemy_bombs_pool, ProjectileState);
    arr_init(PlayerBulletCollisions);
    arr_init(EnemyBombCollisions);
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
    character0.ground_position =
        Clamp(character0.ground_position,-character0.ground_extents,character0.ground_extents);

    deltaTime_loop = deltaTime;
    gameTime_loop = gameTime;
    {
        for (size_t _i = 0; _i < (player_projectile_pool).items.size; _i++) {
            ProjectileStateInList _node = arr_get(ProjectileStateInList, (player_projectile_pool).items, _i);
            if (_i != (player_projectile_pool).starting_index && _node.prev == NULL_OFFSET && _node.next == NULL_OFFSET) continue;
            ProjectileState projectile = _node.item;
            if (projectile.active) {
                if (projectile.lifetime_max >= (gameTime_loop - projectile.birth_time)) {
                    projectile.position = Vector2Add(projectile.position,Vector2Scale(projectile.velocity,deltaTime_loop));
                    ProjectileStateInList updated = {projectile, _node.prev, _node.next};
                    arr_set(player_projectile_pool.items, _i, updated);
                } else {
                    projectile.active = false;
                    ProjectileStateInList updated = {projectile, _node.prev, _node.next};
                    arr_set(player_projectile_pool.items, _i, updated);
                }
            }
        }
    }

    if (IsKeyPressed(KEY_P)) {
        character0.bullet_damage_tier++;
    }
    if (IsKeyPressed(KEY_O)) {
        character0.bullet_damage_tier--;
    }

    character0.bullet_damage_tier = (char)Clamp(character0.bullet_damage_tier,0,4);

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && gameTime - character0.lastFire >= character0.fire_frequency) {
        character0.lastFire = gameTime;

        SpawnPlayerBullet(0, character0.bullet_damage_tier);
    }

    TestFallingBombs();
    CheckCollisions();
}

void CheckCollisions() {
    arr_empty(PlayerBulletCollisions);
    arr_empty(EnemyBombCollisions);

    for (size_t _i = 0; _i < (player_projectile_pool).items.size; _i++) {
        ProjectileStateInList _node = arr_get(ProjectileStateInList, (player_projectile_pool).items, _i);
        if (_i != (player_projectile_pool).starting_index && _node.prev == NULL_OFFSET && _node.next == NULL_OFFSET) continue;
        ProjectileState projectile = _node.item;
        if (projectile.active) {
            for (size_t _j = 0; _j < (enemy_bombs_pool).items.size; _j++) {
                ProjectileStateInList _node_j = arr_get(ProjectileStateInList, (enemy_bombs_pool).items, _j);
                if (_j != (enemy_bombs_pool).starting_index && _node_j.prev == NULL_OFFSET && _node_j.next == NULL_OFFSET) continue;
                ProjectileState bomb = _node_j.item;
                if (bomb.active) {
                    if (CheckCollisionRotatedEllipses(
                            projectile.position,
                            projectile.radius_h,
                            projectile.radius_v,
                            ProjectileRotation(&projectile),
                            bomb.position, bomb.radius_h, bomb.radius_v, ProjectileRotation(&bomb))) {

                        ProjectileCollision hit = {
                            .entityTarget = 0,
                            .impactPoint = Vector2Lerp(projectile.position, bomb.position, 0.5f),
                            .collisionType = ToEnemy
                        };
                        arr_append(PlayerBulletCollisions, hit);
                        bomb.active = false;
                        ProjectileStateInList updated = {bomb, _node_j.prev, _node_j.next};
                        arr_set(enemy_bombs_pool.items, _j, updated);
                    }
                }
            }
        }
    }

    playerPos_loop = GetCharacterPosition();
    playerRadius_loop = 20.0f;

    for (size_t _i = 0; _i < (enemy_bombs_pool).items.size; _i++) {
        ProjectileStateInList _node = arr_get(ProjectileStateInList, (enemy_bombs_pool).items, _i);
        if (_i != (enemy_bombs_pool).starting_index && _node.prev == NULL_OFFSET && _node.next == NULL_OFFSET) continue;
        ProjectileState bomb = _node.item;
        if (bomb.active) {
            if (CheckCollisionRotatedEllipseCircle(
                    bomb.position, bomb.radius_h, bomb.radius_v, ProjectileRotation(&bomb),
                    playerPos_loop, playerRadius_loop)) {
                
                ProjectileCollision hit = {
                    .entityTarget = 0,
                    .impactPoint = playerPos_loop,
                    .collisionType = ToPlayer
                };
                arr_append(EnemyBombCollisions, hit);
                bomb.active = false;
                ProjectileStateInList updated = {bomb, _node.prev, _node.next};
                arr_set(enemy_bombs_pool.items, _i, updated);
            }
        }
    }
}

void SpawnPlayerBullet(float AngleOffset, char bulletLevel) {
    ProjectileState new_proj = {
        .position = Vector2FromToAtDistance(GetCharacterPositionWithOffset((Vector2){0,-10}),
            GetMousePositionInFrame(),25),
        .lifetime_max = 1.0f,
        .birth_time = GetTime(),
        .radius_v = 20,
        .radius_h = 15,
        .active = true,
        .ornament = bulletLevel
    };

    Vector2 new_velocity = Vector2Rotate(Vector2DirectionScaled(new_proj.position
        ,GetMousePositionInFrame(),350), AngleOffset);
    new_proj.velocity = new_velocity;

    list_insert_last(&player_projectile_pool, &new_proj);
}

void TestFallingBombs() {
    float deltaTime = GetFrameTime();
    double gameTime = GetTime();

    deltaTime_loop = deltaTime;
    gameTime_loop = gameTime;
    {
        for (size_t _i = 0; _i < (enemy_bombs_pool).items.size; _i++) {
            ProjectileStateInList _node = arr_get(ProjectileStateInList, (enemy_bombs_pool).items, _i);
            if (_i != (enemy_bombs_pool).starting_index && _node.prev == NULL_OFFSET && _node.next == NULL_OFFSET) continue;
            ProjectileState bomb = _node.item;
            if (bomb.active) {
                if ((bomb.lifetime_max >= (gameTime_loop - bomb.birth_time)) && bomb.position.y < groundHeight) {
                    bomb.velocity = Vector2Add(bomb.velocity, (Vector2){0,bomb.v_acceleration*deltaTime_loop});
                    bomb.position = Vector2Add(bomb.position,Vector2Scale(bomb.velocity,deltaTime_loop));
                    ProjectileStateInList updated = {bomb, _node.prev, _node.next};
                    arr_set(enemy_bombs_pool.items, _i, updated);
                } else {
                    bomb.active = false;
                    ProjectileStateInList updated = {bomb, _node.prev, _node.next};
                    arr_set(enemy_bombs_pool.items, _i, updated);
                }
            }
        }
    }

    static double last_bomb_test_time = 0;
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && IsKeyDown(KEY_Q) && gameTime - last_bomb_test_time >= 0.2) {
        last_bomb_test_time = gameTime;

        ProjectileState new_proj = {
            .position = GetMousePositionInFrame(),
            .lifetime_max = 2.0f,
            .birth_time = gameTime,
            .v_acceleration = 150,
            .radius_v = 20,
            .radius_h = 15,
            .active = true,
            .ornament = 0
        };

        Vector2 new_velocity = ((Vector2){random_sign*100,5});
        new_proj.velocity = new_velocity;

        list_insert_last(&enemy_bombs_pool, &new_proj);
    }
}

void CharacterDraw() {
    DrawTextureEx(texTank,
        (Vector2){w.refW/2-texTank.width/2.0f+character0.ground_position,
            groundHeight -texTank.height/2.0f},
            0.0f, 1.0f, WHITE);

    {
        for (size_t _i = 0; _i < (player_projectile_pool).items.size; _i++) {
            ProjectileStateInList _node = arr_get(ProjectileStateInList, (player_projectile_pool).items, _i);
            if (_i != (player_projectile_pool).starting_index && _node.prev == NULL_OFFSET && _node.next == NULL_OFFSET) continue;
            ProjectileState projectile = _node.item;
            if (projectile.active) {
                DrawRectanglePro(
                    (Rectangle){
                        projectile.position.x,
                        projectile.position.y,
                        projectile.radius_v,
                        projectile.radius_v},
                    (Vector2){
                        projectile.radius_v/2.0f,
                        projectile.radius_v/2.0f},
                    RAD2DEG*Vector2Angle((Vector2){0,1},Vector2Normalize(projectile.velocity)),
                    DARKGRAY);

                DrawRectanglePro(
                    (Rectangle){
                        projectile.position.x,
                        projectile.position.y,
                        projectile.radius_h,
                        projectile.radius_v},
                    (Vector2){
                        projectile.radius_h/2.0f,
                        projectile.radius_v/2.0f},
                    RAD2DEG*Vector2Angle((Vector2){0,1},Vector2Normalize(projectile.velocity)),
                    BLACK);

                rtpAtlasSprite texBullet = BulletAtlas[(int)projectile.ornament];
                const Rectangle rectSrc = { (float)texBullet.positionX, (float)texBullet.positionY, (float)texBullet.sourceWidth, (float)texBullet.sourceHeight};
                Rectangle destRec = {
                    projectile.position.x,
                    projectile.position.y,
                    (float)texBullet.sourceWidth,
                    (float)texBullet.sourceHeight
                };
                const float angle = RAD2DEG*Vector2Angle((Vector2){0,1},Vector2Normalize(projectile.velocity));
                DrawTexturePro(texProjectileAtlas, rectSrc, destRec, (Vector2){texBullet.sourceWidth/2.0f,texBullet.sourceHeight/2.0f}, angle, WHITE);
            }
        }
    }

    {
        for (size_t _i = 0; _i < (enemy_bombs_pool).items.size; _i++) {
            ProjectileStateInList _node = arr_get(ProjectileStateInList, (enemy_bombs_pool).items, _i);
            if (_i != (enemy_bombs_pool).starting_index && _node.prev == NULL_OFFSET && _node.next == NULL_OFFSET) continue;
            ProjectileState bomb = _node.item;
            if (bomb.active) {
                Texture2D texBomb = texFragBombAtlas;
                Vector2 v = bomb.velocity;
                float len = Vector2Length(v);
                if (len < 0.0001f) v = (Vector2){ 1.0f, 0.0f };
                else v = Vector2Scale(v, 1.0f/len);

                float actual_angle = atan2f(v.y, v.x);
                bool flipX = false;
                if (fabsf(actual_angle) > PI*0.5f) {
                    flipX = true;
                    actual_angle = PI - actual_angle;
                }
                actual_angle = Clamp(actual_angle, 0.0f, PI*0.5f);
                const float step = (PI*0.5f) / 9.0f;
                int spriteIndex = (int)lroundf(actual_angle / step);
                spriteIndex = (int)Clamp((float)spriteIndex, 0.0f, 9.0f);
                float sprite_angle_rad = spriteIndex * step;
                float extra_rot_deg = 0;
                if (flipX) {
                    extra_rot_deg = RAD2DEG * (-actual_angle + sprite_angle_rad);
                } else {
                    extra_rot_deg = RAD2DEG * ( actual_angle - sprite_angle_rad);
                }

                Rectangle rectSrc = TrimmedFromBombAtlas(spriteIndex, texBomb);
                if (flipX) {
                    rectSrc.width *= -1.0f;
                }
                Rectangle destRec = {
                    bomb.position.x,
                    bomb.position.y,
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
}

void CharacterUnload() {
    UnloadTexture(texTank);
    arr_clear(player_projectile_pool.items);
    arr_clear(enemy_bombs_pool.items);
    arr_clear(PlayerBulletCollisions);
    arr_clear(EnemyBombCollisions);
}

Vector2 GetCharacterPosition() {
    return (Vector2){w.refW/2.0f+character0.ground_position, (float)groundHeight};
}

Vector2 GetCharacterPositionWithOffset(Vector2 offset) {
    return Vector2Add(GetCharacterPosition(),offset) ;
}

int CharacterGetBulletDamageTier(void) {
    return (int)character0.bullet_damage_tier;
}
