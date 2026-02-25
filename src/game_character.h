#pragma once
#include "raylib.h"
#include "raymath.h"

typedef struct {
    double lastFire;
    float ground_position;
    float ground_velocity;
    float fire_frequency;
    float ground_extents;
    float ground_max_speed;
    char bullet_damage_tier;
} CharacterState;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    double birth_time;
    float v_acceleration;
    float radius_v;
    float radius_h;
    float lifetime_max;
    bool active;
    char ornament;
} ProjectileState;

typedef enum {
    ToPlayer,
    ToEnemy
} CollisionType;

typedef struct {
    size_t entityTarget;
    Vector2 impactPoint;
    CollisionType collisionType;
} ProjectileCollision;

static CharacterState character0;

int CharacterGetBulletDamageTier(void);

void CharacterInitialize();
void CharacterUpdate();
void CharacterDraw();
void CharacterUnload();
void TestFallingBombs();
void SpawnPlayerBullet(float angleOffset, char bulletLevel);
void SpawnEnemyBomb();
void CheckCollisions();

Vector2 GetCharacterPosition();
Vector2 GetCharacterPositionWithOffset();

inline float ProjectileSpeed(ProjectileState *projectile) {
   return Vector2Length(projectile->velocity);
}

inline float ProjectileRotation(ProjectileState *projectile) {
    return Vector2Angle((Vector2){0,1}, projectile->velocity);
}