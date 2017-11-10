#include "DepthProcesser.h"
#include <vector>

typedef unsigned short uint16;
typedef unsigned int uint32;

DepthProcesser::DepthProcesser()
{
	cam.position = vec3(0, 0, 0);
	nezo.position = vec3(0, 0, 0);
}


DepthProcesser::~DepthProcesser()
{
}

void DepthProcesser::WriteOut() {
	std::ofstream myfile;
	myfile.open("example.obj");
	myfile << "# Scanner_file" << std::endl;
	myfile << "o ScanObject" << std::endl;
	for (vec3 pont : pontok) {
		myfile << "v " << pont.x<<" "<<pont.y<<" "<<pont.z << std::endl;
	}
	for (vec3 pont : pontok) {
		myfile << "vn " << "-1.0000 0.00000 0.0000" << std::endl;
	}
	
	myfile << "usemtl None" << std::endl;
	myfile << "s off" << std::endl;
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
		
		if (pont.z<1000) pontok.push_back(pont);
		//pontok.push_back(irany*depthfield[i]+cam.position);
		RGBfield[i] = 0x000000;//Convert(depthfield[i]);
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
			// RGBfield[y*m_depthWidth + x] = Convert((pont-nezo.position).Length());
		}
	}
	for (int i = 0; i < m_depthWidth * m_depthHeight; ++i)
	{
		
		RGBfield[i] = Convert(DepthMap[i]);
	}
	
	if (!pontok.empty() && file) {
		WriteOut();
		std::cout << "Written Out" << std::endl;
		file = false;
	}
	pontok.clear();
}

