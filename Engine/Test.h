#pragma once

#include <stdlib.h>

#include "Farlor.h"
#include "Math/Math.h"
#include "Math/CollisionFunctions.h"

using namespace std;
using namespace Farlor;

void TestRandomCrap()
{
    Vector2 a{0.0f, 0.0f};
    Vector2 b{1.0f, 0.0f};
    Vector2 c{0.0f, 1.0f};
    Vector2 p{1.3f, 1.3f};

    cout << "Point in triangle: " << PointInTriangle(p, a, b, c) << endl;

    char quit = cin.get();

    if(quit == 'q')
    {
        exit(1);
    }
}
