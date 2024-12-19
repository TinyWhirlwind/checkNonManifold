#ifndef _CAMERA_
#define _CAMERA_

/*************** sn3DCore::Camera ******************
*--------------------------------------------------*
* 该类实现了当前场景的显示范围控制功能,主要包括:   *
*  1. 视景体 投影矩阵设置,支持透视、正交两种方式   *
*  2. 旋转、平移、缩放，模型视点矩阵设置           *
*  3. 场景坐标变换                                 *
*  4. 场景变换初始化                               *
***************************************************/


#include "Common/point3.h"
#include "Common/plane3.h"
#include "Common/line3.h"
#include "Common/matrix44.h"
#include "Common/matrix33.h"

namespace sn3DCore
{
	class Camera
	{
	public:

		void GetView();
		void SetView();


		//======场景坐标变换
		Point3f Project(const Point3f &p) const;
		Point3f UnProject(const Point3f &p) const;
		Point3f ViewPoint() const;


		Plane3f ViewPlaneFromModel(const Point3f &p); //返回通过点P切垂直于视轴的平面
		Line3f ViewLineFromModel(const Point3f &p);   //返回通过点P的视线
		Line3f ViewLineFromWindow(const Point3f &p);  //返回通过点P和观察者的线
		Point3f NormDevCoordToWindowCoord(const Point3f &p) const;//将归一化设备坐标转化为窗口坐标
		Point3f WindowCoordToNormDevCoord(const Point3f &p) const;//将窗口坐标转化为归一化设备坐标

	public:

		int viewport[4];		//左下角，宽，高
		float ratio;			//视景体高度
		float fov;				//视角

		float clipRatioFar;		//远近裁剪比率
		float clipRatioNear;	//近裁剪比率
		float nearPlane;		//近裁剪平面
		float farPlane;			//远裁剪平面

		//======Camera参数
		Point3f m_Eye;
		Point3f m_Up;
		Point3f m_Target;

		Matrix44f proj;//投影矩阵
		Matrix44f model;//模型视图矩阵
		Matrix44f matrix;//变换矩阵
		Matrix44f inverse;//变换矩阵的逆  

	};
}

#endif
