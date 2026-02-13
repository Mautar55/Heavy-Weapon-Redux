#pragma once
#include "raylib.h"

typedef struct {
    float ground_position;
    float ground_velocity;
} CharacterState;

static CharacterState main_character_state;

void CharacterInitialize();
void CharacterCheckKeysPressed();
void CharacterDraw();
void CharacterUnload();