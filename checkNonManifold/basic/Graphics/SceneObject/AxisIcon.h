#ifndef _AXISICON_
#define _AXISICON_

/*******************************
 AxisIcon

 坐标轴绘制

********************************/

#include "Common/quaternion.h"

using namespace sn3DCore;

class AxisIcon
{
public:
   void GLRender(int locx,int locy,int w,int h,float * mx);
   void Cone(int slices,float lenght,float width); //绘制圆锥,x方向  
   void Cylinder(int slices,float lenght,float width); //绘制圆柱,x方向
   void Arrow(float body_lengtht,float body_width,float head_lenght,float head_width,int body_slice=50,int head_slice=50);//绘制箭头,x方向
   void MoveArrow(float scale,int body_slice=50,int head_slice=50);
   void RotateArrow(float scale,int body_slice=50,int head_slice=50);
   void SetRotate(Quaternionf ro);
   AxisIcon();

private:
	void DrawX();
	void DrawY();
	void DrawZ();
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float position[4];
	float ambientFancyBack[4];
	float diffuseFancyBack[4];
	float specularFancyBack[4];
	float ambientFancyFront[4];
	float diffuseFancyFront[4];
	float specularFancyFront[4];
	Quaternionf m_rotate;
};

#endif