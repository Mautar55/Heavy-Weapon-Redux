#ifndef WMATH_H
#define WMATH_H

#include "raylib.h"
#include <stdbool.h>

// Check collision between two rotated ellipses
bool CheckCollisionRotatedEllipses(Vector2 pos1, float rh1, float rv1, float rot1,
                                   Vector2 pos2, float rh2, float rv2, float rot2);

// Check collision between a rotated ellipse and a circle
bool CheckCollisionRotatedEllipseCircle(Vector2 ellipsePos, float rh, float rv, float rot,
                                        Vector2 circlePos, float circleRadius);

#endif // WMATH_H
