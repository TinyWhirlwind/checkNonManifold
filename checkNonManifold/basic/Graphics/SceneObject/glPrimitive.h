#ifndef GL_PRIMITIVE_H
#define GL_PRIMITIVE_H

/* 
包含了若干场景中物体的绘制方法 */

namespace sn3DCore
{
   class glPrimitive
   {
	public:
		void Cone(int slices,float lenght,float width); //绘制圆锥,x方向
		void Cylinder(int slices,float lenght,float width); //绘制圆柱,x方向
		void Arrow(float body_lengtht,float body_width,float head_lenght,float head_width,int body_slice=50,int head_slice=50);//绘制箭头,x方向
		void MoveArrow(float scale,int body_slice=50,int head_slice=50);
		void RotateArrow(float scale,int body_slice=50,int head_slice=50);

	public: 
		void DrawX();
		void DrawY();
		void DrawZ();
	};
}

#endif