#pragma once
#include "raylib.h"

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
    float radius_v;
    float radius_h;
    float lifetime_max;
    bool active;
    char ornament;
} ProjectileState;


static CharacterState character0;

int CharacterGetBulletDamageTier(void);

void CharacterInitialize();
void CharacterUpdate();
void CharacterDraw();
void CharacterUnload();
void TestFallingBombs();

Vector2 GetCharacterPosition();
Vector2 GetCharacterPositionWithOffset();