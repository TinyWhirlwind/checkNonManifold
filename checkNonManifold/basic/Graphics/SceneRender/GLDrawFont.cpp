#include "GLDrawFont.h"

#include <gl/gl.h>

void GLDrawFont::BuildFont(HDC hDC)						// 创建位图字体
{
	if (bBuilded)
		return;
	HFONT hFont = CreateFont(24, 0, 0, 0, FW_THIN/*FW_BOLD*/, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, L"Arial");

	// Now that we have created a new font, we need to select that font into our global HDC.
	HFONT hFontOld = (HFONT)SelectObject(hDC, hFont);

	m_FontList = glGenLists(256);
	BOOL ret = wglUseFontBitmaps(hDC, 0, 255, m_FontList); // 批量生产显示字符的显示列表
	SelectObject(hDC, hFontOld);					// 选择原来的字体
	DeleteObject(hFont);
	bBuilded = true;

}
void GLDrawFont::KillFont(GLvoid)							// 删除显示列表
{
	if (bBuilded)
		glDeleteLists(m_FontList, 256);					//删除96个显示列表
	bBuilded = false;
}
void GLDrawFont::GLPrint(const char *str, float x, float y)			// 自定义GL输出字体函数
{
	if (!bBuilded)
		return;
	glRasterPos2f(x, y);
	glListBase(m_FontList);
	glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);
}
void GLDrawFont::GLPrint(const char *str, float x, float y, float z)			// 自定义GL输出字体函数
{
	if (!bBuilded)
		return;
	glRasterPos3f(x, y, z);
	glListBase(m_FontList);
	glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);
}