#pragma once
#include <vector>
#include "NuiKinectFusionCameraPoseFinder.h"
#include "Vec.h"

class ICP {
	bool elso;
	std::vector<vec3> model, object;
	double getDistance(vec3, vec3);
public:
	ICP() {
		elso = true;
	}
	Matrix4 getMatrice4(std::vector<vec3>&);
};