#include "SceneCurveObj.h"

#include <gl/glew.h>

#include "sn3DCurveRenderer.h"

sn3DCore::sn3DCurveModel::sn3DCurveModel(const std::vector<Point3f>& curve, bool bClosed, float fLineWidth, Color4f clr /*= Color4f::Red*/)
{
	m_curve.resize(curve.size());
	std::copy(curve.begin(), curve.end(), m_curve.begin());

	m_closed = bClosed;
	m_linewidth = fLineWidth;
	m_color = clr;
	m_renderer.reset(new sn3DCurveRenderer(this));
}

void sn3DCore::sn3DCurveModel::setCurve(const std::vector<Point3f>& curve, bool bClosed)
{
	m_curve.resize(curve.size());
	std::copy(curve.begin(), curve.end(), m_curve.begin());

	m_closed = bClosed;
}

void sn3DCore::sn3DCurveModel::setCurveColor(Color4f clr)
{
	m_color = clr;
}

void sn3DCore::sn3DCurveModel::Render()
{
	if (GetRenderer())
		GetRenderer()->Draw();
}

void sn3DCore::sn3DCurveModel::UpdateAll()
{
	UpdateDisplay();
}

void sn3DCore::sn3DCurveModel::UpdateDisplay()
{
	if (GetRenderer())
		GetRenderer()->Update();
}

