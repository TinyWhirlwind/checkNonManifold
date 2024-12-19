#ifndef _GLDrawFont_
#define _GLDrawFont_

#include <Windows.h>

/* GL字体类 
*	\note
*		文字显示使用
*/
class GLDrawFont
{
public:
	GLDrawFont()
	{
		bBuilded = false;
	}
	~GLDrawFont()
	{
		KillFont();
	}

	void BuildFont(HDC hDC);  // 创建位图字体					
	void KillFont(void);	  // 删除显示列表						
	void GLPrint(const char *str, float x, float y);	// 自定义GL输出字体函数
	void GLPrint(const char *str, float x, float y, float z);
	bool IsBuild() { return bBuilded; }

protected:
	unsigned int m_FontList;
	bool	bBuilded;

};

#endif
