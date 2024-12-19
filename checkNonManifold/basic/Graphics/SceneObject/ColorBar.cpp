//#include"stdafx.h"
#include "ColorBar.h"
#include <windows.h>

ColorBar::ColorBar(SceneGL3D* pScene)
{
	m_scene_ = pScene;

	m_v_max_ = 2.0;
	m_v_min_ = 0.0;

	m_color_.resize(3);
	m_color_[0] = Color4f(1.0, 1.0, 0.0, 1.0);
	m_color_[1] = Color4f(1.0, 0.5, 0.0, 1.0);
	m_color_[2] = Color4f(1.0, 0.0, 0.0, 1.0);
	m_segs_ = m_color_.size() - 1;
	m_lists = 0;

	LPCWSTR faceName = L"Arial";
	HFONT hFont = CreateFont(13, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, faceName);
	HFONT hOldFont = (HFONT)SelectObject(m_scene_->m_device.m_hDC, hFont);

	if (m_lists <= 0)
	{
		m_lists = glGenLists(256);
	}
	DeleteObject(hOldFont);
	glGetError();
	wglUseFontBitmaps(m_scene_->m_device.m_hDC, 0, 255, m_lists);
}

ColorBar::~ColorBar()
{
	if (m_lists > 0)
	{
		glDeleteLists(m_lists, 256);
	}
}

void ColorBar::Draw(int locx, int locy, int w, int h)
{
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
	glViewport(locx, locy, w, h);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-3, 3, -2, 2, -2, 2);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_DEPTH_TEST);

	glColor4f(0.0, 1.0, 0.0, 1.0);
	glLineWidth(1.0);

	Point3f minp, maxp;
	minp = Point3f(1.0, -1.5, 0.0);
	maxp = Point3f(2.0, 2.0, 0.0);

#if 0
	// 颜色渐变条
	int num = 10;
	float step = (maxp[1] - minp[1]) / (num * 7);
	float seg = step*num;
	Color4f color;
	
	glBegin(GL_QUADS);
	for (int i = 0; i < num / 2; i++)
	{
		color.lerp(m_color_[1], m_color_[0], (float)i / ((float)num / 2));
		glColor4f(color[0], color[1], color[2], 1.0);
		glVertex3f(minp[0], minp[1] + step*i, minp[2]);
		glVertex3f(maxp[0], minp[1] + step*i, minp[2]);

		color.lerp(m_color_[1], m_color_[0], (float)(i + 1) / ((float)num / 2));
		glColor4f(color[0], color[1], color[2], 1.0);
		glVertex3f(maxp[0], minp[1] + step*(i + 1), minp[2]);
		glVertex3f(minp[0], minp[1] + step*(i + 1), minp[2]);
	}
	glEnd();

	glBegin(GL_QUADS);
	for (int it = 6; it > 0; it--)
		for (int i = 0; i < num; i++)
		{
			color.lerp(m_color_[it + 1], m_color_[it], (float)i / ((float)num));
			glColor4f(color[0], color[1], color[2], 1.0);
			glVertex3f(minp[0], minp[1] + ((6 - it) + 0.5)*seg + step*i, minp[2]);
			glVertex3f(maxp[0], minp[1] + ((6 - it) + 0.5)*seg + step*i, minp[2]);
			color.lerp(m_color_[it + 1], m_color_[it], (float)(i + 1) / ((float)num));
			glColor4f(color[0], color[1], color[2], 1.0);
			glVertex3f(maxp[0], minp[1] + ((6 - it) + 0.5)*seg + step*(i + 1), minp[2]);
			glVertex3f(minp[0], minp[1] + ((6 - it) + 0.5)*seg + step*(i + 1), minp[2]);
		}
	glEnd();

	
	glBegin(GL_QUADS);
	for (int i = 0; i < num / 2; i++)
	{
		color.lerp(m_color_[1], m_color_[0], (float)i / ((float)num / 2));
		glColor4f(color[0], color[1], color[2], 1.0);
		glVertex3f(minp[0], minp[1] + 6.5*seg + step*i, minp[2]);
		glVertex3f(maxp[0], minp[1] + 6.5*seg + step*i, minp[2]);

		color.lerp(m_color_[1], m_color_[0], (float)(i + 1) / ((float)num / 2));
		glColor4f(color[0], color[1], color[2], 1.0);
		glVertex3f(maxp[0], minp[1] + 6.5*seg + step*(i + 1), minp[2]);
		glVertex3f(minp[0], minp[1] + 6.5*seg + step*(i + 1), minp[2]);
	}
	glEnd();
#else
	float step = (maxp[1] - minp[1]) / (1.f * m_segs_);
	Color4f color;
	glBegin(GL_QUADS);
	for (int i = 0; i < m_segs_; ++i)
	{
		color = m_color_[i];
		glColor4f(color[0], color[1], color[2], 1.0);
		glVertex3f(minp[0], minp[1] + step*i, minp[2]);
		glVertex3f(maxp[0], minp[1] + step*i, minp[2]);

		color = m_color_[i + 1];
		glColor4f(color[0], color[1], color[2], 1.0);
		glVertex3f(maxp[0], minp[1] + step*(i + 1), minp[2]);
		glVertex3f(minp[0], minp[1] + step*(i + 1), minp[2]);
	}
	glEnd();
#endif
	

	glColor4f(0.0, 0.0, 0.0, 1.0);
	glLineWidth(0.5);

	// 绘制颜色条外框
	glBegin(GL_LINES);
	glVertex3f(minp[0], minp[1], minp[2]);
	glVertex3f(maxp[0], minp[1], minp[2]);

	glVertex3f(maxp[0], minp[1], minp[2]);
	glVertex3f(maxp[0], maxp[1], minp[2]);

	glVertex3f(maxp[0], maxp[1], minp[2]);
	glVertex3f(minp[0], maxp[1], minp[2]);

	glVertex3f(minp[0], maxp[1], minp[2]);
	glVertex3f(minp[0], minp[1], minp[2]);
	glEnd();

	// 绘制刻度值和刻度线
	step = (maxp[1] - minp[1]) / (1.f*m_segs_);		//每段显示Y位置
	float vStep = (m_v_max_ - m_v_min_) / m_segs_;	//每段刻度值
	for (int i = 0; i < m_segs_+1; i++)
	{
		glBegin(GL_LINES);
		glVertex3f(minp[0], minp[1] + i*step, minp[2]);
		glVertex3f(minp[0] - 0.4f, minp[1] + i*step, minp[2]);
		glEnd();

		char str[1024];
		Point3f p;
		p[0] = minp[0] - 3.8f;
		p[1] = minp[1] + i*step;
		p[2] = minp[2];

		sprintf(str, "%.2f", m_v_min_ + i*vStep);
		glRasterPos3f(p[0], p[1], p[2]);
		drawString(str);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glPopAttrib();
}
void ColorBar::SetMaxMin(float maxv, int minv)
{
	m_v_max_ = maxv;
	m_v_min_ = minv;
}

void ColorBar::SetColor(std::vector<Color4f>& vecColor)
{
	m_color_.resize(vecColor.size());
	for (int i = 0; i < vecColor.size(); ++i)
		m_color_[i] = vecColor[i];

	m_segs_ = vecColor.size() - 1;
}

void ColorBar::drawString(std::string str)
{
	glListBase(m_lists);
	glCallLists(str.size(), GL_UNSIGNED_BYTE, str.c_str());	
}