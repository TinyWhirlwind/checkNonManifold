#include "sn3DGeometryRenderer.h"

#include <gl/glew.h>

#include "SceneGeometryObj.h"

sn3DCore::sn3DGeometryRenderer::sn3DGeometryRenderer(sn3DGeometryModel* model)
{
	m_model = model;
}

void sn3DCore::sn3DGeometryRenderer::Draw()
{
	switch (m_renderMethod)
	{
	case RMGeneral:
	case RMBufferObject:
	case RMShader:
		DrawGeneral();
		break;
	case RMDisplayList:
		if (glIsList(m_list.list))
			glCallList(m_list.list);
		break;
	}
}

void sn3DCore::sn3DGeometryRenderer::Update()
{
	switch (m_renderMethod)
	{
	case RMGeneral:
	case RMBufferObject:
	case RMShader:
		break;
	case RMDisplayList:
		BuildList();
		break;
	}
}

void sn3DCore::sn3DGeometryRenderer::BuildPointList()
{
	NewList();
	glNewList(m_list.list, GL_COMPILE);

	DrawPoints();

	glEndList();
}
void sn3DCore::sn3DGeometryRenderer::BuildFaceList()
{
	NewList();
	glNewList(m_list.list, GL_COMPILE);

	DrawFill();

	glEndList();

}

