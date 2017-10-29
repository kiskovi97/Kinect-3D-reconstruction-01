#include "DepthProcesser.h"


typedef unsigned short uint16;
typedef unsigned int uint32;

DepthProcesser::DepthProcesser()
{

}


DepthProcesser::~DepthProcesser()
{
}

uint32 DepthProcesser::Convert(uint16 depth) {
	unsigned short max = 9000;
	unsigned short temp = (1.0f-depth*1.0f/max*1.0f)*256;
	
	return temp;
}
void DepthProcesser::Process(uint16* depthfield, uint32* RGBfield, int m_depthWidth, int m_depthHeight) {
	cam.position = vec3(0, 0, 0);
	int m_Depth = m_depthHeight / (2 * cos(0.3752)); // 21.5fok
	cam.forward = vec3(0, 0,  m_Depth*0.1f);
	cam.right = vec3(m_depthWidth*0.05f, 0, 0);
	cam.up = vec3(0, m_depthHeight*0.05f, 0);

	for (int i = 0; i < m_depthWidth * m_depthHeight; ++i)
	{
		int x = i%m_depthWidth;
		int y = (i - x) / m_depthWidth;
		vec3 temp_pont = cam.Pixel(x - m_depthWidth / 2, y - m_depthHeight / 2, m_depthWidth, m_depthHeight);
		vec3 irany = temp_pont - cam.position;
		irany=irany.normalize();
		pontok.push_back(irany*depthfield[i]+cam.position);

		RGBfield[i] = 0x000000;//Convert(depthfield[i]);
	}

	cam.position = cam.position + vec3(300, 0, 0);
	for (vec3 pont : pontok) {
		vec3 irany = pont - cam.position;
		vec3 pixel = cam.position + irany*(dota(cam.forward, cam.forward) / dota(cam.forward, irany));
		float _x = dota(pixel - cam.position, cam.right) / (cam.right.Length()*cam.right.Length());
		float _y = dota(pixel - cam.position, cam.up) / (cam.up.Length()*cam.up.Length());
		int x = ((_x+2.0f) / 2.0f)*m_depthWidth;
		int y = ((1.0f-_y+1.0f) / 2.0f)*m_depthHeight;
		if (x >= m_depthWidth || y >= m_depthHeight || x < 0 || y < 0){
			//printf("Hus");
		} else RGBfield[y*m_depthWidth + x] = Convert((pont-cam.position).Length());
	}
	

	pontok.clear();
}

