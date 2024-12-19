#include "sn3DCurveRenderer.h"

#include <gl/glew.h>

#include "SceneCurveObj.h"

using namespace sn3DCore;
sn3DCurveRenderer::sn3DCurveRenderer(sn3DCurveModel* model) : sn3DGeometryRenderer(model)
{
}

void sn3DCore::sn3DCurveRenderer::Draw()
{
	sn3DGeometryRenderer::Draw();
}

void sn3DCore::sn3DCurveRenderer::Update()
{
	sn3DGeometryRenderer::Update();
}

void sn3DCore::sn3DCurveRenderer::DrawFill()
{
	sn3DCurveModel* model = (sn3DCurveModel*)m_model;
	if (model->m_curve.empty())
		return;

	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
	
	glDisable(GL_LIGHTING);
	glColor4fv(&(model->m_color[0]));
	glLineWidth(model->m_linewidth);
	
	glBegin(GL_LINES);
	
	int size = model->m_curve.size();
	int i=0;
	for (i = 0; i < model->m_curve.size()-1; ++i)
	{
		glVertex3fv(&(model->m_curve[i][0]));
		glVertex3fv(&(model->m_curve[i+1][0]));
	}
	if (model->m_closed)
	{
		glVertex3fv(&(model->m_curve[i][0]));
		glVertex3fv(&(model->m_curve[(i + 1) % size][0]));
	}
	
	glEnd();
	
	glPopAttrib();
}

