#include "DepthProcesser.h"


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
void DepthProcesser::Process(uint16* depthfield, uint32* RGBfield, int m_depthWidth, int m_depthHeight) {
	
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

	if (elore) nezo.position = nezo.position + vec3(5, 0, 0);
	else nezo.position = nezo.position - vec3(5, 0, 0);
	if (nezo.position.x > 400) elore = false;
	if (nezo.position.x < -400) elore = true;
	
	for (vec3 pont : pontok) {
		vec3 irany = pont - nezo.position;
		vec3 pixel = nezo.position + irany*(dota(nezo.forward, nezo.forward) / dota(nezo.forward, irany));
		float _x = dota(pixel - nezo.position, nezo.right) / (nezo.right.Length()*nezo.right.Length());
		float _y = dota(pixel - nezo.position, nezo.up) / (nezo.up.Length()*nezo.up.Length());
		int x = ((_x+1.0f) / 2.0f)*m_depthWidth;
		int y = ((1.0f-_y) / 2.0f)*m_depthHeight;
		if (x >= m_depthWidth || y >= m_depthHeight || x < 0 || y < 0){
			//printf("Hus");
		} else RGBfield[y*m_depthWidth + x] = Convert((pont-nezo.position).Length());
	}
	

	pontok.clear();
}

