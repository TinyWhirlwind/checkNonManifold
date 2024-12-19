#ifndef _COLORBAR_
#define _COLORBAR_

#include <string>

#include "Common/Color4.h"
#include "SceneRender/SceneGL3D.h"
//#include <gl\gl.h>
#include <gl/glew.h>

/* 颜色标尺绘制 */
class ColorBar
{
public:
	ColorBar(sn3DCore::SceneGL3D* pScene);
	~ColorBar();

	/// 设置标尺大小值
	void SetMaxMin(float vMax, int vMin);

	/// 设置颜色标尺
	void SetColor(std::vector<sn3DCore::Color4f>& vecColor);

	/// 绘制
	void Draw(int locx, int locy, int w, int h);

private:
	void drawString(std::string str);

private:
	sn3DCore::SceneGL3D* m_scene_;

	std::vector<sn3DCore::Color4f> m_color_;

	int m_segs_;	//刻度段数
	float m_v_max_;	//刻度最大值
	float m_v_min_;	//刻度最小值

	GLuint m_lists;
};
#endif