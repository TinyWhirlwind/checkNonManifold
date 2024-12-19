#include "sn3DLineRenderer.h"

#include <gl/glew.h>

#include "SceneLineObj.h"

using namespace sn3DCore;
sn3DLineRenderer::sn3DLineRenderer(sn3DLineModel* model) : sn3DGeometryRenderer(model)
{
}

void sn3DCore::sn3DLineRenderer::Draw()
{
	sn3DGeometryRenderer::Draw();
}

void sn3DCore::sn3DLineRenderer::Update()
{
	sn3DGeometryRenderer::Update();
}

void sn3DCore::sn3DLineRenderer::DrawFill()
{
	sn3DLineModel* model = (sn3DLineModel*)m_model;

	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
	
	glDisable(GL_LIGHTING);
	glColor4fv(&(model->m_lineColor[0]));
	glLineWidth(model->m_lineWidth);
	
	glBegin(GL_LINES);
	glVertex3fv(&(model->m_start[0]));
	glVertex3fv(&(model->m_end[0]));
	glEnd();
	
	glPopAttrib();
}

