//#include"stdafx.h"
#include "InfoBar.h"

#include <windows.h>
#include <gl\gl.h>

InfoBar::InfoBar()
{
	m_maxv = 2.0;
	m_minv = 0.0;
	m_values[0] = Color4f(1.0, 0.0, 0.0, 1.0);

	m_values[1] = Color4f(0.0, 1.0, 0.0, 1.0);

	m_values[2] = Color4f(0.0, 0.0, 1.0, 1.0);

	m_values[3] = Color4f(1.0, 1.0, 0.0, 1.0);

	m_values[4] = Color4f(0.0, 0.0, 0.0, 1.0);
	m_vNum = 0;
	m_tNum = 0;
}

void InfoBar::GLRender(SceneGL3D * painter, int locx, int locy, int w, int h)
{
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
	glViewport(locx, locy, w, h);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-3, 3.3, -2, 2, -2, 2);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_DEPTH_TEST);
	glColor4f(0.0, 1.0, 0.0, 1.0);
	glLineWidth(2.5);

	Point3f minp, maxp;
	minp = Point3f(-2.0, -1.5, 0.0);
	maxp = Point3f(3.3, 1.7, 0.0);
	Color4f color;

	glBegin(GL_LINES);
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glVertex3f(maxp[0], maxp[1], minp[2]);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glVertex3f(minp[0], maxp[1], minp[2]);
	glEnd();

	glColor4f(0.0, 0.0, 0.0, 1.0);
	char str[1024];

	Point3f p;
	p[0] = -1.8;
	p[1] = -1.0;
	p[2] = 0.0;

	sprintf(str, "顶点数: %d", m_vNum);
	glRasterPos3f(p[0], p[1], p[2]);
	drawCNString(painter, str);

	p[0] = -1.8;
	p[1] = 0.5;
	p[2] = 0.0;

	sprintf(str, "三角形数: %d", m_tNum);
	glRasterPos3f(p[0], p[1], p[2]);
	drawCNString(painter, str);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glPopAttrib();
}

void InfoBar::SetMaxMin(float maxv, int minv)
{
	m_maxv = maxv;
	m_minv = minv;
}

void InfoBar::SetInfo(int v, int t)
{
	m_vNum = v;
	m_tNum = t;
}

void InfoBar::SetColor(Color4f values[])
{
	for (int i = 0; i < 7; i++)
	{
		m_values[i] = values[i];
	}
}

void InfoBar::drawString(SceneGL3D * scene, std::string str)
{
	GLuint lists;

	LPCWSTR faceName = L"Arial";
	HFONT hFont = CreateFont(13, 0, 0, 0, FW_LIGHT, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, faceName);
	HFONT hOldFont = (HFONT)SelectObject(scene->m_device.m_hDC, hFont);
	lists = glGenLists(256);
	DeleteObject(hOldFont);
	glGetError();
	wglUseFontBitmaps(scene->m_device.m_hDC, 0, 255, lists);
	glListBase(lists);
	glCallLists(str.size(), GL_UNSIGNED_BYTE, str.c_str());
	glDeleteLists(lists, 256);
}

void InfoBar::drawCNString(SceneGL3D * scene, const char* str)
{
	HFONT hFont = CreateFontA(14, 0, 0, 0, FW_NORMAL, 0, 0, 0,
		CHINESEBIG5_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "新宋体");
	HFONT hOldFont = (HFONT)SelectObject(scene->m_device.m_hDC, hFont);
	DeleteObject(hOldFont);
	int len, i;
	wchar_t* wstring;
	GLuint list = glGenLists(1);

	// 计算字符的个数
	// 如果是双字节字符的（比如中文字符），两个字节才算一个字符
	// 否则一个字节算一个字符
	len = 0;
	for (i = 0; str[i] != '\0'; ++i)
	{
		if (IsDBCSLeadByte(str[i]))
			++i;
		++len;
	}

	// 将混合字符转化为宽字符
	wstring = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstring, len);
	wstring[len] = L'\0';

	// 逐个输出字符
	for (i = 0; i < len; ++i)
	{
		wglUseFontBitmapsW(scene->m_device.m_hDC, wstring[i], 1, list);
		glCallList(list);
	}

	// 回收所有临时资源
	free(wstring);
	glDeleteLists(list, 1);
}