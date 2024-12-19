#include "sn3DSphereRenderer.h"

#include <gl/glew.h>
#include "SceneSphereObj.h"
#include "glShape.h"

sn3DCore::sn3DSphereRenderer::sn3DSphereRenderer(sn3DSphereModel* model) : sn3DGeometryRenderer(model)
{
}

void sn3DCore::sn3DSphereRenderer::Draw()
{
	sn3DGeometryRenderer::Draw();
}

void sn3DCore::sn3DSphereRenderer::Update()
{
	sn3DGeometryRenderer::Update();
}

void sn3DCore::sn3DSphereRenderer::DrawFill()
{
	sn3DSphereModel* model = (sn3DSphereModel*)m_model;

	glShape::DrawSolidSphere(model->m_center, model->m_radius, model->m_color);
}

