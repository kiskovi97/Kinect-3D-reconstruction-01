#pragma once
#include <list>
#include "Camera.h"
#include <iostream>
#include <fstream>


typedef unsigned short uint16;
typedef unsigned int uint32;
class DepthProcesser
{
private:
	std::list<vec3> pontok;
	MyCamera cam;
	MyCamera nezo;
	bool elore = true;
	bool file = true;
public:
	DepthProcesser();
	~DepthProcesser();

	void WriteOut();

	uint32 Convert(uint16 depth);
	void Process(uint16* depthfield, uint32* RGBfield, const int m_depthWidth, const int m_depthHeight);
};

