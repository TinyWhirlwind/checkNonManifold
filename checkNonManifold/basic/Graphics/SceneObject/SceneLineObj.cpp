#include "SceneLineObj.h"
#include "sn3DLineRenderer.h"

sn3DCore::sn3DLineModel::sn3DLineModel(Point3f start, Point3f end, float lineWidth, Color4f color /*= Color4f::Green*/)
{
	m_start = start;
	m_end = end;
	m_lineWidth = lineWidth;
	m_lineColor = color;

	m_renderer.reset(new sn3DLineRenderer(this));
}

void sn3DCore::sn3DLineModel::Render()
{
	if (GetRenderer())
		GetRenderer()->Draw();
}

void sn3DCore::sn3DLineModel::UpdateAll()
{
	UpdateDisplay();
}

void sn3DCore::sn3DLineModel::UpdateDisplay()
{
	if (GetRenderer())
		GetRenderer()->Update();
}
