#include "Camera.h"
vec3 MyCamera::Pixel(int x, int y, int width, int height) {
	float tempx = 2.0f * x / width - 1.0f;
	float tempy = (2.0f * y / height - 1.0f)*(-1.0f);
	return up*tempy + right*tempx + forward;
}

MyCamera::MyCamera()
{
	position = vec3(0, 0, 0);
	forward = vec3(0, 0, 1.0f);
	up = vec3(0, 1.0f, 0);
	right = vec3(1.0f, 0, 0);
}

MyCamera::~MyCamera()
{
}