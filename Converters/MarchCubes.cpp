#include "MarchCubes.h"



MarchCubes::MarchCubes()
{
}


MarchCubes::~MarchCubes()
{
}

std::vector<int> MarchCubes::Haromszogek(int i) {
	std::vector<int> ki;
	for (int j = 0; j < 16; j++) {
		if (triTable[i][j] != -1) ki.push_back(triTable[i][j]);
	}
	return ki;
}