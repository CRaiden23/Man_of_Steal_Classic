#include "Utilities.h"
#include "ComponentIncludes.h"

float DegreesToRadians(float degrees)
{
	return degrees * (XM_PIDIV2 / 180);
}

float EuclideanDistanceVectors(Vector2 v1, Vector2 v2)
{
	float interior = (pow(v1.x - v2.x, 2) + (pow(v1.y - v2.y, 2)));
	return sqrt(interior);
}

Vector2 RandVector(Vector2 v1, float randScalar)
{
	int random = rand() % 4;
	float rndx = (rand() % int(randScalar + randScalar)) - randScalar;
	float rndy = (rand() % int(randScalar + randScalar)) - randScalar;

	v1.x += rndx;
	v1.y += rndy;

	return v1;
}
