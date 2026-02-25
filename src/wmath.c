#include "wmath.h"
#include "raymath.h"
#include <math.h>

#ifndef Max
#define Max(a, b) (((a) > (b)) ? (a) : (b))
#endif

// Helper to check if a point is inside a rotated ellipse
static bool IsPointInRotatedEllipse(Vector2 point, Vector2 ellipsePos, float rh, float rv, float rot) {
    // Translate point to origin relative to ellipse
    float dx = point.x - ellipsePos.x;
    float dy = point.y - ellipsePos.y;

    // Rotate point back by -rot
    float cosRot = cosf(-rot);
    float sinRot = sinf(-rot);

    float rx = dx * cosRot - dy * sinRot;
    float ry = dx * sinRot + dy * cosRot;

    // Standard ellipse equation: (x/a)^2 + (y/b)^2 <= 1
    return (rx * rx) / (rh * rh) + (ry * ry) / (rv * rv) <= 1.0f;
}

// Approximation for rotated ellipse-ellipse collision using sampling
// A more accurate method would involve solving a quartic equation or using GJK/SAT.
// Given the context of a game, sampling or a simplified bounding box + point checks might suffice if not performance critical.
// Here we use a slightly better approach: check if any point on the boundary of one ellipse is inside the other.
bool CheckCollisionRotatedEllipses(Vector2 pos1, float rh1, float rv1, float rot1,
                                   Vector2 pos2, float rh2, float rv2, float rot2) {
    // 1. Broad phase: distance check
    float maxDist = Max(rh1, rv1) + Max(rh2, rv2);
    if (Vector2Distance(pos1, pos2) > maxDist) return false;

    // 2. Sample points on ellipse 1 and check if they are in ellipse 2
    const int samples = 8;
    for (int i = 0; i < samples; i++) {
        float angle = (float)i * (2.0f * PI / (float)samples);
        // Point on unrotated ellipse 1
        float px = rh1 * cosf(angle);
        float py = rv1 * sinf(angle);

        // Rotate point by rot1
        float s = sinf(rot1);
        float c = cosf(rot1);
        Vector2 p;
        p.x = pos1.x + px * c - py * s;
        p.y = pos1.y + px * s + py * c;

        if (IsPointInRotatedEllipse(p, pos2, rh2, rv2, rot2)) return true;
    }

    // 3. Sample points on ellipse 2 and check if they are in ellipse 1
    for (int i = 0; i < samples; i++) {
        float angle = (float)i * (2.0f * PI / (float)samples);
        float px = rh2 * cosf(angle);
        float py = rv2 * sinf(angle);

        float s = sinf(rot2);
        float c = cosf(rot2);
        Vector2 p;
        p.x = pos2.x + px * c - py * s;
        p.y = pos2.y + px * s + py * c;

        if (IsPointInRotatedEllipse(p, pos1, rh1, rv1, rot1)) return true;
    }

    // 4. Check centers
    if (IsPointInRotatedEllipse(pos1, pos2, rh2, rv2, rot2)) return true;
    if (IsPointInRotatedEllipse(pos2, pos1, rh1, rv1, rot1)) return true;

    return false;
}

bool CheckCollisionRotatedEllipseCircle(Vector2 ellipsePos, float rh, float rv, float rot,
                                        Vector2 circlePos, float circleRadius) {
    // Transform circle center to ellipse's local coordinate system
    float dx = circlePos.x - ellipsePos.x;
    float dy = circlePos.y - ellipsePos.y;

    float cosRot = cosf(-rot);
    float sinRot = sinf(-rot);

    float localX = dx * cosRot - dy * sinRot;
    float localY = dx * sinRot + dy * cosRot;

    // Now we need to find the distance from the point (localX, localY) to the axis-aligned ellipse (rh, rv)
    // If that distance is <= circleRadius, they collide.
    
    // A simple approximation: check if the circle center is within an expanded ellipse
    // or if the closest point on the ellipse is within the circle.
    
    // Find closest point on axis-aligned ellipse to (localX, localY)
    // This is still non-trivial. Let's use sampling for now as well for consistency.
    
    if (IsPointInRotatedEllipse(circlePos, ellipsePos, rh + circleRadius, rv + circleRadius, rot)) return true;
    
    // More accurate: find point on ellipse closest to circle center
    const int samples = 12;
    for (int i = 0; i < samples; i++) {
        float angle = (float)i * (2.0f * PI / (float)samples);
        float px = rh * cosf(angle);
        float py = rv * sinf(angle);
        
        float s = sinf(rot);
        float c = cosf(rot);
        Vector2 p;
        p.x = ellipsePos.x + px * c - py * s;
        p.y = ellipsePos.y + px * s + py * c;
        
        if (CheckCollisionPointCircle(p, circlePos, circleRadius)) return true;
    }

    return IsPointInRotatedEllipse(circlePos, ellipsePos, rh, rv, rot);
}
