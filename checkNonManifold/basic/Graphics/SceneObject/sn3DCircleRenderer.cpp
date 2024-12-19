#include "sn3DCircleRenderer.h"

#include <gl/glew.h>

#include "SceneCircleObj.h"

using namespace sn3DCore;
sn3DCircleRenderer::sn3DCircleRenderer(sn3DCircleModel* model) : sn3DGeometryRenderer(model)
{
}

void sn3DCore::sn3DCircleRenderer::Draw()
{
	sn3DGeometryRenderer::Draw();
}

void sn3DCore::sn3DCircleRenderer::Update()
{
	sn3DGeometryRenderer::Update();
}

void sn3DCore::sn3DCircleRenderer::DrawFill()
{
	sn3DCircleModel* model = (sn3DCircleModel*)m_model;

	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);

	glColor4fv(&(model->m_circleColor[0]));
	
	glBegin(GL_TRIANGLE_FAN);
	glVertex3fv(&model->m_circleC[0]);
	int size = model->m_circleEdgePts.size();
	for (int i = 0; i <= size; ++i)
	{
		glVertex3fv(&model->m_circleEdgePts[(i%size)][0]);
	}
	glEnd();

	glPopAttrib();
}

