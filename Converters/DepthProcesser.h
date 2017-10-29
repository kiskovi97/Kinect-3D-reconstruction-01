#pragma once
#include <list>
#include "Camera.h"

typedef unsigned short uint16;
typedef unsigned int uint32;
class DepthProcesser
{
private:
	std::list<vec3> pontok;
	MyCamera cam;
	bool elore = true;
public:
	DepthProcesser();
	~DepthProcesser();

	uint32 Convert(uint16 depth);
	void Process(uint16* depthfield, uint32* RGBfield, int m_depthWidth, int m_depthHeight);
};

