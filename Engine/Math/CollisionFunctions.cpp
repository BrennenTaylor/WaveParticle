#include "CollisionFunctions.h"

namespace Farlor
{
    float Sign(Vector2 p1, Vector2 p2, Vector2 p3)
    {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    }
    
    bool PointInTriangle(Vector2 p, Vector2 a, Vector2 b, Vector2 c)
    {
        bool b1, b2, b3;
        b1 = Sign(p, a, b) < 0.0f;
        b2 = Sign(p, b, c) < 0.0f;
        b3 = Sign(p, c, a) < 0.0f;

        return ((b1 == b2) && (b2 == b3));
    }

    float DistancePointToLine(Vector2 start, Vector2 end, Vector2 p)
    {
        Vector2 norm = end - start;
        Vector2 pointStart = start - p;
        Vector2 c = norm * (pointStart.Dot(norm) / norm.Dot(norm));
        Vector2 d = pointStart - c;
        return sqrt(d.Dot(d));
    }

    float TriangleArea(Vector2 a, Vector2 b, Vector2 c)
    {
        float area = 0.0f;
        area = 0.5f * abs((a.x - c.x)*(b.y - a.y) - (a.x - b.x)*(c.y-a.y));
        return area;
    }
}