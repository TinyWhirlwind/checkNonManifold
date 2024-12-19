#include "SceneCircleObj.h"
#include "sn3DCircleRenderer.h"
#include "MathTools.h"
#include <QtCore/QDebug>

sn3DCore::sn3DCircleModel::sn3DCircleModel(Point3f c, Point3f n, float r, Color4f color /*= Color4f::Green*/)
{
	m_circleC = c;
	m_circleN = n;
	m_circleR = r;
	m_circleColor = color;
	GenerateCircleEdge();
	m_renderer.reset(new sn3DCircleRenderer(this));
}

bool sn3DCore::sn3DCircleModel::GenerateCircleEdge()
{
	m_circleEdgePts.clear();
	m_circleEdgePts.reserve(180);
	
	Quaternionf quat = MathTools::MakeRotate(Point3f(0.f,0.f,1.f), m_circleN);
	Matrix44f rot; quat.ToMatrix(rot);
	Matrix44f trans; trans.SetTranslate(m_circleC - Point3f(0.f, 0.f, 0.f));

	Matrix44f mxTransform;
	mxTransform = trans * rot;

	int div = 20;
	float angle;
	Point3f p;
	for (int i = 0; i < div; ++i)
	{
		angle = (2.f*M_PI / (div*1.f)) * i;
		p.x = m_circleR * cos(angle);
		p.y = m_circleR * sin(angle);
		p.z = 0.f;
		p = mxTransform * p;
		m_circleEdgePts.push_back(p);
	}
	return (m_circleEdgePts.size()>0);
}

void sn3DCore::sn3DCircleModel::Render()
{
	if (GetRenderer())
		GetRenderer()->Draw();
}

void sn3DCore::sn3DCircleModel::UpdateAll()
{
	UpdateDisplay();
}

void sn3DCore::sn3DCircleModel::UpdateDisplay()
{
	if (GetRenderer())
		GetRenderer()->Update();
}
