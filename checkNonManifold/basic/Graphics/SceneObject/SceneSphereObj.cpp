#include "SceneSphereObj.h"

#include <gl/glew.h>

#include "sn3DSphereRenderer.h"

sn3DCore::sn3DSphereModel::sn3DSphereModel(Point3f c, float r, Color4f clr /*= Color4f::Red*/)
{
	m_center = c;
	m_radius = r;
	m_color = clr;
	m_renderer.reset(new sn3DSphereRenderer(this));
}

void sn3DCore::sn3DSphereModel::Render()
{
	if (GetRenderer())
		GetRenderer()->Draw();
}

void sn3DCore::sn3DSphereModel::UpdateAll()
{
	UpdateDisplay();
}

void sn3DCore::sn3DSphereModel::UpdateDisplay()
{
	if (GetRenderer())
		GetRenderer()->Update();
}

