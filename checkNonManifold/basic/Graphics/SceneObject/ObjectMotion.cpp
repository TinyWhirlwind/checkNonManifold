
#include"ObjectMotion.h"

#include <QtCore/QDebug>
#pragma warning (disable : 4244)

using namespace sn3DCore;
ObjectMotion::ObjectMotion()
{
	for (int i = 0; i < 16; i++)
	{
		m_matGL[i] = 0;
	}
	m_matGL[0] = m_matGL[5] = m_matGL[10] = m_matGL[15] = 1.0;

	m_scale = 1.0;
	m_trans = Point3f(0.0, 0.0, 0.0);
	m_rotCenter = Point3f(0.0, 0.0, 0.0);
	m_rot.SetIdentity();
}

void ObjectMotion::SetIdentity()
{
	for (int i = 0; i < 16; i++)
		m_matGL[i] = 0.0;
	m_matGL[0] = m_matGL[5] = m_matGL[10] = m_matGL[15] = 1.0;

}

void ObjectMotion::SetTransformGL(float  * matrix)
{
	for (int i = 0; i < 16; i++)
	{
		m_matGL[i] = matrix[i];
	}
}

void ObjectMotion::GetTransformGL(float  * matrix)
{
	for (int i = 0; i < 16; i++)
	{
		matrix[i] = m_matGL[i];
	}
}

void ObjectMotion::SetTransform(Point3f & rotCenter, Quaternionf & rot, Point3f & trans)
{
	this->m_rot = rot;
	this->m_trans = trans;
	this->m_rotCenter = rotCenter;

	float matGL[16];	//OpengGL变换矩阵（列优先）

	Matrix44f matRot;	//Quat矩阵(行优先)
	rot.ToMatrix(matRot);

	Matrix44f matPrev; matPrev.SetTranslate(-rotCenter);
	Matrix44f matPost; matPost.SetTranslate(rotCenter);

	// 旋转矩阵伴随旋转中心
	matRot = matPost * matRot * matPrev;

	Matrix44f matTrans;
	matTrans.SetTranslate(trans);

	Matrix44f result;
	result = matTrans * matRot;	//先旋转再平移

	//for (int i = 0; i < 4; ++i)
	//	qDebug("ycx SetRT mat[%d]: [%f %f %f %f]", i, result[i][0], result[i][1], result[i][2], result[i][3]);

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; ++j)
			matGL[i * 4 + j] = result[j][i];

	SetTransformGL(matGL);
}

void ObjectMotion::GetTransform(Point3f & rotCenter, sn3DCore::Quaternionf & rot, Point3f & trans)
{
	rot = this->m_rot;
	trans = this->m_trans;
	rotCenter = this->m_rotCenter;
}

void ObjectMotion::SetRT(Quaternionf & rot, Point3f &  t)
{
	m_rot = rot;
	m_trans = t;
	
#if 0
	Matrix44f mat;
	rot.ToMatrix(mat);

	float tmat[16];
	float matGL[16]; // 总体变换
	for (int i = 0; i < 16; i++)
		tmat[i] = 0;
	tmat[0] = tmat[5] = tmat[10] = tmat[15] = 1.0;

	tmat[3] = t[0];
	tmat[7] = t[1];
	tmat[11] = t[2];

	float result[16];

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			float t = 0.0;
			for (int k = 0; k < 4; k++)
				t += tmat[i * 4 + k] * mat[k][j];
			result[i * 4 + j] = t;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matGL[i * 4 + j] = result[j * 4 + i];
		}
	}
#else

	float matGL[16];	//OpengGL变换矩阵（列优先）

	Matrix44f matRot;	//Quat矩阵(行优先)
	rot.ToMatrix(matRot);

	Matrix44f matTrans; 
	matTrans.SetTranslate(t);

	Matrix44f result; 
	result = matTrans * matRot;	//先旋转再平移

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; ++j)
			matGL[i * 4 + j] = result[j][i];
#endif
	SetTransformGL(matGL);
}

void ObjectMotion::GetRT(Quaternionf & rot, Point3f &  t)
{
	rot = m_rot;
	t = m_trans;
}
