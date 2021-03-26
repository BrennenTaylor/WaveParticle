#pragma once

#include <FMath/FMath.h>

namespace Farlor
{
    float Sign(Vector2 p1, Vector2 p2, Vector2 p3);
    bool PointInTriangle(Vector2 p, Vector2 a, Vector2 b, Vector2 c);

    float DistancePointToLine(Vector2 start, Vector2 end, Vector2 p);

    float TriangleArea(Vector2 a, Vector2 b, Vector2 c);
}