#pragma once
#include "raylib.h"

typedef struct {
    float ground_position;
    float ground_velocity;
    float fire_frequency;
    double lastFire;
    float ground_max_speed;
} CharacterState;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    double birth_time;
    float radius_v;
    float radius_h;
    float lifetime_max;
    bool active;
} ProjectileState;

static CharacterState character0;

void CharacterInitialize();
void CharacterUpdate();
void CharacterDraw();
void CharacterUnload();

Vector2 GetCharacterPosition();
Vector2 GetCharacterPositionWithOffset();