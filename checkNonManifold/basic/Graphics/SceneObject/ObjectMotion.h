#ifndef _OBJECT_MOTION_
#define _OBJECT_MOTION_

#include <stdio.h>
#include "Common/quaternion.h"
#include "Common/Point3.h"

/* 场景对象变换 */
class ObjectMotion
{
public: 
	/// 设置RT变换(带旋转中心)
	void SetTransform(Point3f & rotCenter,sn3DCore::Quaternionf & rot,Point3f & trans);
	void GetTransform(Point3f & rotCenter,sn3DCore::Quaternionf & rot,Point3f & trans);

	/// 设置RT
	void SetRT(sn3DCore::Quaternionf & rot, Point3f &  trans);
	void GetRT(sn3DCore::Quaternionf & rot, Point3f &  trans);

	void SetTransformGL(float * mat);       //设置变换矩阵
	void GetTransformGL(float * matrix);	//获取变换矩阵
	void SetIdentity();

public:
	ObjectMotion();
	~ObjectMotion()	{}

public:
	float  m_matGL[16];			//刚体变换矩阵(列优先p*mat)
	
	Point3f m_trans;			//平移
	float m_scale;				//缩放
	Point3f m_rotCenter;		//旋转中心
	sn3DCore::Quaternion<float> m_rot; //旋转

};

#endif