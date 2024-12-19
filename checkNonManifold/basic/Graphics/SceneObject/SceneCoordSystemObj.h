#ifndef _COORDINATESYSTEM_
#define _COORDINATESYSTEM_

#include "Common/Point3.h"
#include "SceneObject/SceneObject.h"

/* 场景坐标系绘制 */
namespace sn3DCore
{
	class CoordinateSystem : public SceneObject
	{
	public:
		void Render();

	private:
		void DrawCoordinateSystem();

		void Cone(int slices, float lenght, float width); //绘制圆锥,x方向

		void Cylinder(int slices, float lenght, float width); //绘制圆柱,x方向

		void Arrow(float body_lengtht, float body_width, float head_lenght, float head_width, int body_slice = 50, int head_slice = 50);//绘制箭头,x方向

		void MoveArrow(float scale, int body_slice = 50, int head_slice = 50);

		void RotateArrow(float scale, int body_slice = 50, int head_slice = 50);

		void DrawX();
		void DrawY();
		void DrawZ();

	};
}
#endif