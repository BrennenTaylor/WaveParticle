#pragma once

#include <stdlib.h>

#include "Farlor.h"
#include "Math/Math.h"

using namespace std;
using namespace Farlor;

void TestRandomCrap()
{
    Vector2 valOne{1, 2};
    Vector2 valTwo{10, 2};
    valTwo *= 5;

    valTwo = valTwo + valOne;
    cout << valOne + valTwo << endl;

    Vector3 vecThree{1, 2, 3};
    cout << vecThree << endl;

    Vector4 vecFour{1, 2, 3, 4};
    cout << vecFour << endl;

    FastVector fastVec1{vecThree};
    FastVector fastVec2{vecFour};

    FastVector result = fastVec1 + fastVec2;

    cout << "Size of float: " << sizeof(float) << endl;
    cout << "Size of Vector2: " << sizeof(Vector2) << endl;
    cout << "Size of Vector3: " << sizeof(Vector3) << endl;
    cout << "Size of Vector4: " << sizeof(Vector4) << endl;
    cout << "Size of FastVector: " << sizeof(FastVector) << endl;
    cout << "Size of FastMatrix: " << sizeof(FastMatrix) << endl;

    cout << "Size of Point: " << sizeof(Point) << endl;

    FastMatrix tempMatrix{1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
    Matrix4x4 printMatrix(tempMatrix);
    cout << "Original: " << printMatrix << endl;
    FastMatrix transposedMatrix = tempMatrix.Transposed();
    Matrix4x4 transposedPrint(transposedMatrix);
    cout << "Transposed: " << transposedPrint << endl;


    char quit = cin.get();

    if(quit == 'q')
    {
        exit(1);
    }
}
