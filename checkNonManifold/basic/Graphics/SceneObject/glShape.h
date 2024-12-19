#pragma once
#include "Common/point3.h"
#include "Common/color4.h"

/* 各类形状绘制 */
class glShape
{
public:
	/// 绘制球
	static void DrawSolidSphere(Point3f center, float radius, sn3DCore::Color4f color);

	/// 半透明平面
	static void  DrawTransparentPlane(Point3f center, Point3f normal,
		                              Point3f frontDir, float size, sn3DCore::Color4f color = sn3DCore::Color4f(0.5, 0.5, 0.9, 0.25));

	/// 圆平面
	static void  DrawPlaneCircle(Point3f center,Point3f normal, float radius);

	/// 旋转的外圈圆
	static void  DrawLineCircle(Point3f center, Point3f normal, float radius, float lineWidth = 2.5f);

	/// 圆柱+箭头
	static void  DrawCylinderArrow(Point3f center, Point3f normal, float len, float radius, float circleRatio=2.4, 
		                 float lenRatio=0.7,int body_slice=25, int head_slice=25);

	/// 旋转箭头
	static void  DrawRotateArrow(Point3f center, Point3f upDir, Point3f frontDir,
		float r, float halfAngle, int body_slice = 25, int head_slice = 15);

	/// 圆柱 + 指定大小的箭头
	static void  DrawCylinderArrowWithFixedSize(Point3f pos, Point3f dir, float cylinderLen, float cylinderRadius,
		float coneLen, float coneRadius,
		int body_slice=25, int head_slice=25);

	/// 直箭头
	static void DrawStraightArrow(Point3f pos, Point3f dir, float scale, int body_slice, int head_slice);

	static void GetRotationMatrix(Point3f up0, Point3f front0, Point3f up1, Point3f front1, float * mat);

	static int	m_straightArrowList; // 显示列表
	static int	m_rotateArrowList;

	static float m_cylinderRadius;

};
