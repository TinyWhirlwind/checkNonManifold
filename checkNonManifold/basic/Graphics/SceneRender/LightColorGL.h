#ifndef _LightColorGL_
#define _LightColorGL_

#include <vector>
#include "Common/Color4.h"

/////////////////////////////////////////////////////////////////////////////
//
//	光照颜色设定 ：LightColorGL
//
//	1 光照模型。
//	2 颜色设置。	
//	3 材质设置。
//
//===========================================================================
//
/////////////////////////////////////////////////////////////////////////////

using namespace sn3DCore;

class MaterialGL
{
public:

	float ambient[4];
	float diffuse[4];
	float specular[4];
};

class LightGL
{
public:
	float ambient[4];
	float diffuse[4];
	float specular[4];

	LightGL()
	{
		ambient[0] = 0.2f; ambient[1] = 0.2f; ambient[2] = 0.2f; ambient[3] = 1.0f;
		diffuse[0] = 0.8f; diffuse[1] = 0.8f; diffuse[2] = 0.8f; diffuse[3] = 1.0f;
		specular[0] = 1.0f; specular[1] = 1.0f; specular[2] = 1.0f; specular[3] = 1.0f;
	}
};

class LightColorGL
{
public:

	static void IndexColor(int index);

	static void GreyColor();

	static void RedColor();

	static void FancyLight();

	LightColorGL();

	static void initMaterial();

	enum LightingModel { LIGHTDOUBLE, LIGHTFANCY };

	class ColorSetting
	{
	public:

		Color4b ColorBottom;
		Color4b ColorTop;
		Color4b Color;

		ColorSetting()
		{
			ColorBottom = Color4b(64, 64, 128, 1);
			ColorTop = Color4b(0, 0, 0, 1);
			Color = Color4b(128, 16, 16, 1);
		};
	};

	static std::vector<MaterialGL> m_material;             //  模型材质集
};
#endif