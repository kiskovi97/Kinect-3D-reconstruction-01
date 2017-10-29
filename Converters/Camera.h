#pragma once
#include "Vec.h"
class MyCamera
{
public:
	vec3 position;
	vec3 forward;
	vec3 up;
	vec3 right;
	MyCamera();
	~MyCamera();
	vec3 Pixel(int x, int y, int width, int height);

};

