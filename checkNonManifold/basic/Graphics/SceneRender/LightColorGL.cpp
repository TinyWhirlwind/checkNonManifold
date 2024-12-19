//#include"stdafx.h"
#include "LightColorGL.h"

#include <windows.h>
#include <gl\gl.h>

std::vector<MaterialGL> LightColorGL::m_material;

LightColorGL::LightColorGL()
{
	initMaterial();
}
void LightColorGL::IndexColor(int index)
{
	float colorv[4];	colorv[3] = 1.0;
	float colord[4];	colord[3] = 1.0;
	float colors[4];	colors[3] = 1.0;

	if (m_material.size() == 0)initMaterial();

	int size = m_material.size();
	for (int i = 0; i < 4; i++)
	{
		colorv[i] = m_material[index%size].ambient[i];
		colord[i] = m_material[index%size].diffuse[i];
		colors[i] = m_material[index%size].specular[i];
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT, colorv);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colord);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colors);

	colorv[0] = 0.5; colorv[1] = 0.5; colorv[2] = 0.5;
	colord[0] = 0.5; colord[1] = 0.5; colord[2] = 0.5;
	colors[0] = 0.0; colors[1] = 0.0; colors[2] = 0.0;

	glMaterialfv(GL_BACK, GL_AMBIENT, colorv);
	glMaterialfv(GL_BACK, GL_DIFFUSE, colord);
	glMaterialfv(GL_BACK, GL_SPECULAR, colors);
}

void LightColorGL::GreyColor()
{

	float colorv[4];	colorv[3] = 0.3;
	float colord[4];	colord[3] = 0.3;
	float colors[4];	colors[3] = 0.3;

	colorv[0] = 0.86f; colorv[1] = 0.86f; colorv[2] = 0.86f;
	colord[0] = 0.86f; colord[1] = 0.86f; colord[2] = 0.86f;
	colors[0] = 0.4f; colors[1] = 0.4f; colors[2] = 0.4f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, colorv);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colord);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colors);

	glMaterialfv(GL_BACK, GL_AMBIENT, colorv);
	glMaterialfv(GL_BACK, GL_DIFFUSE, colord);
	glMaterialfv(GL_BACK, GL_SPECULAR, colors);
}
void LightColorGL::RedColor()
{
	float colorv[4];	colorv[3] = 1.0;
	float colord[4];	colord[3] = 1.0;
	float colors[4];	colors[3] = 1.0;

	colorv[0] = 0.1f; colorv[1] = 0.0f; colorv[2] = 0.0f;
	colord[0] = 0.92f; colord[1] = 0.0f; colord[2] = 0.0f;
	colors[0] = 0.5f; colors[1] = 0.0f; colors[2] = 0.1f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, colorv);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, colord);
	glMaterialfv(GL_FRONT, GL_SPECULAR, colors);

	glMaterialfv(GL_BACK, GL_AMBIENT, colorv);
	glMaterialfv(GL_BACK, GL_DIFFUSE, colord);
	glMaterialfv(GL_BACK, GL_SPECULAR, colors);
}

void LightColorGL::FancyLight()
{
	glDisable(GL_LIGHTING);

	float ambientFancyFront[] = { 0.15f, 0.15f, 0.15f, 1.f };
	float diffuseFancyFront[] = { 0.68f, 0.68f, 0.68f, 1.f };
	float specularFancyFront[] = { 0.f, 0.f, 0.f, 1.f };
	//float specularFancyFront[] = { 1.0f, 1.0f, 1.0f, 1.f };
	float positionFront[] = { 0.f, 0.f, 100.f, 1.f };

	float ambientFancyBack[] = {0.f, 0.f, 0.f, 1.f};
	float diffuseFancyBack[] = {0.035f, 0.035f, 0.035f, 1.f};
	float specularFancyBack[] = {0.f, 0.f, 0.f, 1.f};
	float positionBack[] = { 0.f, 0.f, -100.f, 1.f };

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientFancyFront);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseFancyFront);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularFancyFront);
	glLightfv(GL_LIGHT0, GL_POSITION, positionFront);
	glEnable(GL_LIGHTING);
}
void LightColorGL::initMaterial()
{
	m_material.resize(10);

	m_material[0].diffuse[0] = 0.698039f;
	m_material[0].diffuse[1] = 0.443137f;
	m_material[0].diffuse[2] = 0.698039f;

	m_material[1].diffuse[0] = 0.3568627f;
	m_material[1].diffuse[1] = 0.5568627f;
	m_material[1].diffuse[2] = 0.5568627f;

	m_material[2].diffuse[0] = 0.5803922f;
	m_material[2].diffuse[1] = 0.70980392f;
	m_material[2].diffuse[2] = 0.45098039f;

	m_material[3].diffuse[0] = 0.917647f;
	m_material[3].diffuse[1] = 0.745098f;
	m_material[3].diffuse[2] = 0.572549f;

	m_material[4].diffuse[0] = 0.670588f;
	m_material[4].diffuse[1] = 0.517647f;
	m_material[4].diffuse[2] = 0.823529f;

	m_material[5].diffuse[0] = 0.360784f;
	m_material[5].diffuse[1] = 0.811765f;
	m_material[5].diffuse[2] = 0.360784f;

	m_material[6].diffuse[0] = 0.835294f;
	m_material[6].diffuse[1] = 0.835294f;
	m_material[6].diffuse[2] = 0.215686f;

	m_material[7].diffuse[0] = 0.2f;
	m_material[7].diffuse[1] = 0.7568627f;
	m_material[7].diffuse[2] = 0.7568627f;

	m_material[8].diffuse[0] = 0.188235f;
	m_material[8].diffuse[1] = 0.4470588f;
	m_material[8].diffuse[2] = 0.7058823f;

	m_material[9].diffuse[0] = 0.5313725f;
	m_material[9].diffuse[1] = 0.115686f;
	m_material[9].diffuse[2] = 0.5313725f;

	for (int i = 0; i < 10; i++)
	{
		m_material[i].ambient[3] = 1.0f;
		m_material[i].diffuse[3] = 1.0f;
		m_material[i].specular[3] = 1.0f;
	}

	for (int j = 0; j < 10; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			m_material[j].ambient[i] = 0.4f*m_material[j].diffuse[i];
			m_material[j].specular[i] = 0.4f*m_material[j].diffuse[i];
		}
	}
}