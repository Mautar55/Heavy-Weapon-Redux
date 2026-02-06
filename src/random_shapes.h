#pragma once
#include "raylib.h"

typedef struct {
    Vector3 position;
} ShapeData;

typedef struct {
    Vector3 position;
    Color color;
} ColoredShape;

void InitializeRandomShapes();
void AddRandomShape();
void RemoveLastShape();
void DrawRandomShapes2D();
void DrawRandomShapes3D();