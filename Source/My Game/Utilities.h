#pragma once

#include "ComponentIncludes.h"
#include <time.h>

//Note that this has no class. It's intentional so that it can be called anywhere

float DegreesToRadians(float degrees);

//Given two vectors, find the distance between both
float EuclideanDistanceVectors(Vector2 v1, Vector2 v2);

//Given a vector and a scalar, randomly add or subtract the scalar from either the x or y of the Vector.
Vector2 RandVector(Vector2 v1, float randScalar);