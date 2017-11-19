#include "DepthProcesser.h"
#include <vector>
#include <icpPointToPoint.h>


typedef unsigned short uint16;
typedef unsigned int uint32;

DepthProcesser::DepthProcesser()
{
	cam.position = vec3(0, 0, 0);
	nezo.position = vec3(0, 0, 0);
	array3D.resize(WIDTH);
	for (int i = 0; i < WIDTH; ++i) {
		array3D[i].resize(HEIGHT);

		for (int j = 0; j < HEIGHT; ++j) {
			array3D[i][j].resize(DEPTH);
			for (int k = 0; k < DEPTH; ++k)
				array3D[i][j][k] = false;
		}
			
	}
}


DepthProcesser::~DepthProcesser()
{
	delete[] Elozoframe;
	delete[] Mostaniframe;
}
// ez megy vegig minden kockan
void DepthProcesser::MarchingCubes() {

	for (int i = 0; i < (WIDTH-1); i++)
		for (int j = 0; j < (HEIGHT-1); j++)
			for (int k = 0; k < (DEPTH-1); k++)
			{
				MCube(i,j,k);
			}

}

//Ez a fuggveny generalja le egy darab kockara a haromszogeket
void DepthProcesser::MCube(int x, int y, int z) {
	bool A = array3D[x][y][z];
	bool B = array3D[x+1][y][z];
	bool C = array3D[x+1][y][z+1];
	bool D = array3D[x][y][z+1];
	bool E = array3D[x][y+1][z];
	bool F = array3D[x+1][y+1][z];
	bool G = array3D[x+1][y+1][z+1];
	bool H = array3D[x][y+1][z+1];

	std::vector<int> triangles = mcb.Haromszogek(A + B * 2 + C * 4 + D * 8 + E * 16 + F * 32 + G * 64 + H * 128);
	if (triangles.size()>0)
	for (int i = 0; i < (triangles.size()); i+=3)
	{
		std::vector<vec3> har;
		har.push_back(vec3(x, y, z) + eltolas[triangles[i]]);
		har.push_back(vec3(x, y, z) + eltolas[triangles[i+1]]);
		har.push_back(vec3(x, y, z) + eltolas[triangles[i+2]]);
		vec3 a = triangles[i] - triangles[i + 1];
		vec3 b = triangles[i+1] - triangles[i + 2];
		vec3 norm;
		norm.x = a.y*b.z - a.z*b.y;
		norm.y = a.z * b.x - a.x*b.z;
		norm.z = a.x*b.y - a.y*b.x;
		har.push_back(norm.normalize());
		haromszogek.push_back(har);
	}
	
}

// fileba kiirasa a meglevo haromszogeknek
void DepthProcesser::WriteOut() {
	std::ofstream myfile;
	myfile.open("example.obj");
	myfile << "# Scanner_file" << std::endl;
	myfile << "mtllib untitled.mtl" << std::endl;
	myfile << "o ScanObject" << std::endl;

	for (std::vector<vec3> har : haromszogek)
	{
		myfile << "v " << har[0].x << " " << har[0].y << " " << har[0].z << std::endl;
		myfile << "v " << har[1].x << " " << har[1].y << " " << har[1].z << std::endl;
		myfile << "v " << har[2].x << " " << har[2].y << " " << har[2].z << std::endl;
	}
	/*
	for (std::vector<vec3> har : haromszogek)
	{
		myfile << "vn " << har[3].x << " " << har[3].y << " " << har[3].z << std::endl;
		myfile << "vn " << har[3].x << " " << har[3].y << " " << har[3].z << std::endl;
		myfile << "vn " << har[3].x << " " << har[3].y << " " << har[3].z << std::endl;
	}
	*/
	myfile << "usemtl Material01" << std::endl;
	myfile << "s 1" << std::endl;
	int i = 0;
	for (std::vector<vec3> har : haromszogek)
	{
		myfile << "f " << i*3 +1 << "//" << i+1 << " "<< i * 3 +2<< "//" << i + 1 <<" "<< i * 3 +3 << "//" << i + 1 << std::endl;
		i++;
	}

	myfile.close();
}

// egy adott melysegben levo pixel szinenek kiszamitasa
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
	
	// kamera kalibralasa
	float m_Depth = m_depthWidth / (2.0f*tanf(0.497419f)); // 21.5fok
	cam.forward = vec3(0, 0,  m_Depth*1.0f);
	cam.right = vec3(m_depthWidth*0.5f, 0, 0);
	cam.up = vec3(0, m_depthHeight*0.5f, 0);
	


	// a kinect depth mapja alapjan kiszamolt pontok
	for (int i = 0; i < m_depthWidth * m_depthHeight; ++i)
	{
		// pixel helyenek, a pont iranyanak es helyenek kiszamitasa kiszamitasa
		int x = i % m_depthWidth;
		int y = (i - x) / m_depthWidth;
		vec3 temp_pont = cam.Pixel(x - (m_depthWidth / 2), y - (m_depthHeight / 2), m_depthWidth, m_depthHeight);
		vec3 irany = temp_pont - cam.position;
		float my = depthfield[i]*1.0f * ((y - (m_depthHeight / 2))*(-1.0f) / (2.0f * m_Depth));
		float mx = depthfield[i]*1.0f * ((x - (m_depthWidth / 2))*(1.0f) / (2.0f * m_Depth));
		irany=irany.normalize();
		vec3 pont = cam.forward.normalize()*(depthfield[i] / 3.0f) + cam.right.normalize()*mx + cam.up.normalize()*my + cam.position;

		// pontok helyenek kerekitese egy adott racsra
		int XFaktor = (WIDTH_MAX / WIDTH) * 2;
		int YFaktor = (HEIGHT_MAX / HEIGHT) * 2;
		int ZFaktor = (DEPTH_MAX / DEPTH) ;
		pont.x = (int)pont.x - (int)pont.x % XFaktor;
		pont.y = (int)pont.y - (int)pont.y % YFaktor;
		pont.z = (int)pont.z - (int)pont.z % ZFaktor;
		
		// ha a pont az adott racson belul van bevinni a rendszerbe
		if (pont.z < DEPTH_MAX) {
			int x = pont.x / XFaktor + WIDTH/2;
			int y = pont.y / YFaktor + HEIGHT/2;
			int z = pont.z / ZFaktor;
			if (!array3D[x][y][z]) pontok.push_back(pont);
			array3D[x][y][z] = true;
		}
		RGBfield[i] = 0x000000;
	}


	// a megjeleniteshez hasznalt depthmap
	std::vector<uint16> DepthMap(m_depthWidth*m_depthHeight);
	// pontok alapjan egy kimeneti depth map kiszamitasa
	
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
	
	
	// Frame-ek kiírása
	{
		delete[] Elozoframe;
		Elozoframe = Mostaniframe;
		Mostaniframe = new double[3 * pontok.size()];
		for (int i = 0; i < pontok.size(); i++)
		{
			Mostaniframe[i * 3] = pontok[i].x;
			Mostaniframe[i * 3 + 1] = pontok[i].x;
			Mostaniframe[i * 3 + 2] = pontok[i].x;
		}

		MyIcp();
		// kimeneti kep kiszinezese
	}

	for (int i = 0; i < m_depthWidth * m_depthHeight; ++i)
		RGBfield[i] = Convert(DepthMap[i]);



	// ha vannak pontok es ez az elso kep akkor kiszamitjuk es kiirjuk fileba
	// Elso lekerdezes
	if (!pontok.empty() && elso_frame) {

		Mostaniframe = new double[3 * pontok.size()];
		Elozoframe = new double[3];
		for (int i = 0; i < pontok.size(); i++)
		{
			Mostaniframe[i * 3] = pontok[i].x;
			Mostaniframe[i * 3 + 1] = pontok[i].x;
			Mostaniframe[i * 3 + 2] = pontok[i].x;
		}
		/* Marching Cubes */
		//MarchingCubes();
		//std::cout << "The Math has benn done" << std::endl;
		/* kiírás obj-be */
		//WriteOut();
		//std::cout << "Written Out" << std::endl;
		elso_frame = false;
	}

	// az adatok torlese egyenlore
	pontok.clear();
	for (int i = 0; i < HEIGHT; ++i)
		for (int j = 0; j < WIDTH; ++j) 
			for (int k = 0; k < DEPTH; ++k)
				array3D[i][j][k] = false;

}

void DepthProcesser::MyIcp() {

}
