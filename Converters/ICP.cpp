#include <vector>
#include "ICP.h"
#include "NuiKinectFusionCameraPoseFinder.h"




Matrix4 ICP::getMatrice4(std::vector<vec3>& new_pointcloud) {
	Matrix4 m;
	m.M11 = 1; m.M12 = 0; m.M13 = 0; m.M14 = 0;
	m.M21 = 0; m.M22 = 1; m.M23 = 0; m.M24 = 0;
	m.M31 = 0; m.M32 = 0; m.M33 = 1; m.M34 = 0;
	m.M41 = 0; m.M42 = 0; m.M43 = 0; m.M44 = 1;
	if (elso) {
		model = new_pointcloud;
		elso = false;
		return m;
	}

	double local_min = 100.1;
	
	vec3 dir(0, 0, 0);
	for (vec3  object_point : new_pointcloud) {
		int index = 0;
		int localtionofmin = 0;
		for (vec3  model_point :  model) {
			if (getDistance(model_point, object_point) < local_min) {
				localtionofmin = index;
			}
			index++;
		}
		dir = dir + (object_point - model.at(index));
	}
	dir = dir / vec3(new_pointcloud.size(), new_pointcloud.size(), new_pointcloud.size());
	m.M41 = dir.x; m.M42 = dir.y; m.M43 = dir.z;
	return m;
}


double ICP::getDistance(vec3 p1, vec3 p2)
{
	double x = p2.x - p1.x; //calculating number to square in next step
	double y = p2.y - p1.y;
	double z = p2.z - p1.z;
	double dist;

	dist = pow(x, 2) + pow(y, 2) + pow(z, 2);       //calculating Euclidean distance
	dist = sqrt(dist);

	return dist;
}
