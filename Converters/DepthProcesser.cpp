#include "DepthProcesser.h"
#include <vector>

typedef unsigned short uint16;
typedef unsigned int uint32;

DepthProcesser::DepthProcesser()
{
	cam.position = vec3(0, 0, 0);
	nezo.position = vec3(0, 0, 0);
	array3D.resize(HEIGHT);
	for (int i = 0; i < HEIGHT; ++i) {
		array3D[i].resize(WIDTH);

		for (int j = 0; j < WIDTH; ++j) {
			array3D[i][j].resize(DEPTH);
			for (int k = 0; k < DEPTH; ++k)
				array3D[i][j][k] = false;
		}
			
	}
}


DepthProcesser::~DepthProcesser()
{
}

void DepthProcesser::MarchingCubes() {

	for (int i = 0; i < 199; i++)
		for (int j = 0; j < 199; j++)
			for (int k = 0; k < 99; k++)
			{
				MCube(i,j,k);
			}

}

void DepthProcesser::MCube(int x, int y, int z) {
	bool A = array3D[x][y][z];
	bool B = array3D[x+1][y][z];
	bool C = array3D[x+1][y][z+1];
	bool D = array3D[x][y][z+1];
	bool E = array3D[x][y+1][z];
	bool F = array3D[x+1][y+1][z];
	bool G = array3D[x+1][y+1][z+1];
	bool H = array3D[x][y+1][z+1];
	/*if (A && !B && !C && !D && !E && !F && !G && !H)
	{
		std::vector<vec3> har;
		har.push_back(vec3(x + 0.5, y, z));
		har.push_back(vec3(x, y + 0.5, z));
		har.push_back(vec3(x, y, z + 0.5));
		har.push_back(vec3(1, 1, 1).normalize());
		haromszogek.push_back(har);
	}
	if (A && B && !C && !D && !E && !F && !G && !H)
	{
		std::vector<vec3> har, har2;
		har.push_back(vec3(x + 0.5, y, z));
		har.push_back(vec3(x+1, y , z + 0.5));
		har.push_back(vec3(x+1, y + 0.5, z + 0.5));
		har.push_back(vec3(0, 1, 1).normalize());
		haromszogek.push_back(har);
		har2.push_back(vec3(x + 0.5, y, z));
		har2.push_back(vec3(x , y + 0.5, z ));
		har2.push_back(vec3(x + 1, y + 0.5, z + 0.5));
		har2.push_back(vec3(0, 1, 1).normalize());
		haromszogek.push_back(har2);
	}
	if (A && B && C && D && !E && !F && !G && !H)
	{
		std::vector<vec3> har, har2;
		har.push_back(vec3(x , y + 0.5, z));
		har.push_back(vec3(x + 1, y + 0.5, z));
		har.push_back(vec3(x + 1, y + 0.5, z + 1));
		har.push_back(vec3(0, 1, 0).normalize());
		haromszogek.push_back(har);
		har2.push_back(vec3(x , y + 0.5, z));
		har2.push_back(vec3(x, y + 0.5, z + 1));
		har2.push_back(vec3(x + 1, y + 0.5, z + 1));
		har2.push_back(vec3(0, 1, 0).normalize());
		haromszogek.push_back(har2);
	}*/

	std::vector<int> triangles = mcb.Haromszogek(A + B * 2 + C * 4 + D * 8 + E * 16 + F * 32 + G * 64 + H * 128);
	if (triangles.size()>0)
	for (int i = 0; i < (triangles.size()); i+=3)
	{
		std::vector<vec3> har;
		har.push_back(vec3(x, y, z) + eltolas[triangles[i]]);
		har.push_back(vec3(x, y, z) + eltolas[triangles[i+1]]);
		har.push_back(vec3(x, y, z) + eltolas[triangles[i+2]]);
		har.push_back(vec3(0, 1, 0).normalize());
		haromszogek.push_back(har);
	}
	
}

void DepthProcesser::WriteOut() {
	std::ofstream myfile;
	myfile.open("example.obj");
	myfile << "# Scanner_file" << std::endl;
	myfile << "o ScanObject" << std::endl;

	for (std::vector<vec3> har : haromszogek)
	{
		myfile << "v " << har[0].x << " " << har[0].y << " " << har[0].z << std::endl;
		myfile << "v " << har[1].x << " " << har[1].y << " " << har[1].z << std::endl;
		myfile << "v " << har[2].x << " " << har[2].y << " " << har[2].z << std::endl;
	}

	for (std::vector<vec3> har : haromszogek)
	{
		myfile << "vn " << har[3].x << " " << har[3].y << " " << har[3].z << std::endl;
		myfile << "vn " << har[3].x << " " << har[3].y << " " << har[3].z << std::endl;
		myfile << "vn " << har[3].x << " " << har[3].y << " " << har[3].z << std::endl;
	}

	/*for (vec3 pont : pontok) {
		myfile << "v " << pont.x<<" "<<pont.y<<" "<<pont.z << std::endl;
	}
	for (vec3 pont : pontok) {
		myfile << "vn " << "-1.0000 0.00000 0.0000" << std::endl;
	}*/
	
	myfile << "usemtl None" << std::endl;
	myfile << "s off" << std::endl;
	int i = 0;
	for (std::vector<vec3> har : haromszogek)
	{
		myfile << "f " << i*3 +1 << "//" << i+1 << " "<< i * 3 +2<< "//" << i + 1 <<" "<< i * 3 +3 << "//" << i + 1 << std::endl;
		i++;
	}

	myfile.close();
}

uint32 DepthProcesser::Convert(uint16 depth) {
	uint16 max = 8000;
	uint32 temp;
	if (depth > (uint16)3000) {
		temp = (int)0x000001 + (int)0x000001 * (int)(128 * (1 - ((depth - 3000.0f) / 6200.0f)));
	}
	else {
		temp = (int)0x010000 + (int)0x010000 * (int)(256 * (1 - (depth* 1.0f / 3000.0f)));

		//temp = (1.0f - depth / (uint16)1500) * (int)0xFF0000;
	}

	return temp;
}
void DepthProcesser::Process(uint16* depthfield, uint32* RGBfield,const int m_depthWidth,const int m_depthHeight) {
	float m_Depth = m_depthWidth / (2.0f*tanf(0.497419f)); // 21.5fok
	cam.forward = vec3(0, 0,  m_Depth*1.0f);
	cam.right = vec3(m_depthWidth*0.5f, 0, 0);
	cam.up = vec3(0, m_depthHeight*0.5f, 0);
	std::vector<uint16> DepthMap(m_depthWidth*m_depthHeight);



	for (int i = 0; i < m_depthWidth * m_depthHeight; ++i)
	{
		int x = i % m_depthWidth;
		int y = (i - x) / m_depthWidth;
		vec3 temp_pont = cam.Pixel(x - (m_depthWidth / 2), y - (m_depthHeight / 2), m_depthWidth, m_depthHeight);
		vec3 irany = temp_pont - cam.position;
		float my = depthfield[i]*1.0f * ((y - (m_depthHeight / 2))*(-1.0f) / (2.0f * m_Depth));
		float mx = depthfield[i]*1.0f * ((x - (m_depthWidth / 2))*(1.0f) / (2.0f * m_Depth));

		irany=irany.normalize();
		vec3 pont = cam.forward.normalize()*(depthfield[i] / 3.0f) + cam.right.normalize()*mx + cam.up.normalize()*my + cam.position;
		pont.x = (int)pont.x - (int)pont.x % 10;
		pont.y = (int)pont.y - (int)pont.y % 10;
		pont.z = (int)pont.z - (int)pont.z % 10;
		
		if (pont.z < 1000) {
			int x = pont.x / 10 + 100;
			int y = pont.y / 10 + 100;
			int z = pont.z / 10;
			if (!array3D[x][y][z]) pontok.push_back(pont);
			array3D[x][y][z] = true;
			
		}
		
		
		RGBfield[i] = 0x000000;
	}
	
	for (vec3 pont : pontok) {
		vec3 irany = pont - nezo.position;
		vec3 pixel = nezo.position + irany*(dota(nezo.forward, nezo.forward) / dota(nezo.forward, irany));
		float _x = dota(pixel - nezo.position, nezo.right) / (nezo.right.Length()*nezo.right.Length());
		float _y = dota(pixel - nezo.position, nezo.up) / (nezo.up.Length()*nezo.up.Length());
		int x = ((_x+0.25f) * 1.0f)*m_depthWidth;
		int y = ((0.25f-_y) * 1.0f)*m_depthHeight;
		if (x >= m_depthWidth || y >= m_depthHeight || x < 0 || y < 0){
			
		}
		else {
			if (DepthMap[y*m_depthWidth + x]==0  || DepthMap[y*m_depthWidth + x] >  (pont - nezo.position).Length())
			DepthMap[y*m_depthWidth + x] = (pont - nezo.position).Length(); 
		}
	}
	for (int i = 0; i < m_depthWidth * m_depthHeight; ++i)
	{
		
		RGBfield[i] = Convert(DepthMap[i]);
	}
	
	if (!pontok.empty() && file) {
		MarchingCubes();
		WriteOut();
		std::cout << "Written Out" << std::endl;
		file = false;
	}
	pontok.clear();

	for (int i = 0; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH; ++j) 
			for (int k = 0; k < DEPTH; ++k)
				array3D[i][j][k] = false;

}

