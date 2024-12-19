#include "sn3DMeshRenderer.h"

#include <windows.h>
#include <gl/glew.h>
#include <QtCore/qdebug.h>
#include "Common/point3.h"
#include "GeoData/MeshData.h"
#include "SceneRender/Shader.h"
#include "QApplication.h"
#include "sn3DNewMeshRender.h"
using namespace sn3DCore;
sn3DMeshRenderer::sn3DMeshRenderer()
	:sn3DRenderer()
{
	m_bufferInit = false;
	m_ReUpdataReader = true;
	m_rangeShader = -1;

	m_color[0] = 0.2f;
	m_color[1] = 0.2f;
	m_color[2] = 0.2f;
	m_color[3] = 1.f;

	// 初始化模型变换矩阵(单位矩阵)
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (i == j)
				m_mat[i * 4 + j] = 1.f;
			else
				m_mat[i * 4 + j] = 0.f;
		}
	}
	
}

sn3DMeshRenderer::~sn3DMeshRenderer()
{

}

void sn3DMeshRenderer::Clear()
{
	sn3DRenderer::Clear();

	m_bufferInit = false;
	m_rangeShader = -1;
}

void sn3DMeshRenderer::Draw()
{
	Update();
	StartProperty();

	switch (m_renderMethod)
	{
	case RMGeneral:
		DrawGeneral();
		break;
	case RMDisplayList:
		if (glIsList(m_list.list))
			glCallList(m_list.list);
		else
			BuildList();
		break;
	case RMBufferObject:
		DrawBufferARB();
		break;
	case RMShader:
		DrawRangeData();
		break;
	}
	EndProperty();
}

void sn3DMeshRenderer::StartProperty()
{
	sn3DRenderer::StartProperty();

	// 模型矩阵
	float mvMatrix[16];
	float projMatrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mvMatrix);
	glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);

	Matrix44f mv, mat;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			mv[i][j] = mvMatrix[i * 4 + j];
			mat[i][j] = m_mat[i * 4 + j];
		}
	Matrix44f mvMat = mat * mv;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf(&mvMat[0][0]);	
}

void sn3DMeshRenderer::EndProperty()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	sn3DRenderer::EndProperty();
}

void sn3DMeshRenderer::DrawBufferARB()
{
	boost::shared_ptr<sn3DMeshData> data = boost::dynamic_pointer_cast<sn3DMeshData>(m_data);
	if (!data)
		return;

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vertexBuffer);         // for vertex coordinates
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexBuffer); // for indices

	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, 0);
	glNormalPointer(GL_FLOAT, 0, (GLvoid *)(sizeof(GLfloat)*data->vn * 3));

	glDrawElements(GL_TRIANGLES, data->fn * 3, GL_UNSIGNED_INT, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

void sn3DMeshRenderer::Update()
{
	if (!m_ReUpdataReader)return;
	switch (m_renderMethod)
	{
	case RMBufferObject:
		BuildBufferARB();
		break;
	case RMShader:
		BuildRangeData();
		break;
	}
	m_ReUpdataReader = false;
}

void sn3DMeshRenderer::DrawFill()
{
	if (!m_data)
		return;

	boost::shared_ptr<sn3DMeshData> mesh = boost::dynamic_pointer_cast<sn3DMeshData>(m_data);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	int faceN = (int)m_data->m_faces.size();
	if (!IsHasTex())
	{
		if(m_colorMode == CMPerMesh)
			glColor4fv(m_color);

		for (int i = 0; i < faceN; i++)
		{
			Face * f = &m_data->m_faces[i];
			if (m_data->m_faces[i].IsD())continue;
			if (m_data->m_faces[i].V(0)->IsD())continue;
			if (m_data->m_faces[i].V(1)->IsD())continue;
			if (m_data->m_faces[i].V(2)->IsD())continue;

			Point3f normal;
			int index;			// 顶点索引
			Point3f coord;		// 顶点坐标
			Color4f color;		// 顶点颜色

			if (SMFlat == m_shadeMode)
			{
				normal = f->N();
				glNormal3fv(&normal[0]);
			}
			if (CMPerFace == m_colorMode && !m_data->m_faceColor.empty())
			{
				color = m_data->m_faceColor[i];
				glColor3fv(&color[0]);
			}

			glBegin(GL_TRIANGLES);
			for (int j = 0; j < 3; ++j)
			{
				index = f->V(j)->Index();
				coord = f->V(j)->P();

				if (SMSmooth == m_shadeMode)
				{
					normal = f->V(j)->N();
					glNormal3fv(&normal[0]);
				}
				else if (SMNormalSplit == m_shadeMode)
				{
					normal = f->V(j)->N();
					glNormal3fv(&normal[0]);
				}

				if (CMPerVertex == m_colorMode && !m_data->m_vertexColor.empty())
				{
					color = m_data->m_vertexColor[index];

					glColor3fv(&color[0]);
				}

				glVertex3fv(&coord[0]);
			}
			glEnd();
		}
	}
	else
	{
		if(m_colorMode == CMPerMesh)
			glColor4fv(m_color);
		for (int i = 0; i < faceN; i++)
		{
			Face* f = &m_data->m_faces[i];
			if (m_data->m_faces[i].IsD())continue;
			if (m_data->m_faces[i].V(0)->IsD())continue;
			if (m_data->m_faces[i].V(1)->IsD())continue;
			if (m_data->m_faces[i].V(2)->IsD())continue;

			Point3f normal;
			Point3f point;
			Point2f tex;

			normal = f->N();
			glNormal3fv(&normal[0]);

			glBegin(GL_TRIANGLES);
			for (int j = 0; j < 3; ++j)
			{
				tex = f->TexCoord(j);
				point = f->V(j)->P();

				glTexCoord2fv(&tex[0]);
				glVertex3fv(&point[0]);
			}
			glEnd();
		}
	}
}

void sn3DMeshRenderer::DrawWire()
{
	if (!m_data)
		return;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glColor4fv(m_color);
	int faceN = (int)m_data->m_faces.size();
	for (int i = 0; i < faceN; i++)
	{
		Face * f = &m_data->m_faces[i];
		if (m_data->m_faces[i].IsD())continue;
		if (m_data->m_faces[i].V(0)->IsD())continue;
		if (m_data->m_faces[i].V(1)->IsD())continue;
		if (m_data->m_faces[i].V(2)->IsD())continue;
	
		glBegin(GL_TRIANGLES);
		for (int j = 0; j < 3; ++j)
		{
			glVertex3fv(&f->V(j)->P()[0]);
		}
		glEnd();
	}
}

void sn3DMeshRenderer::DrawPoints()
{
	if (!m_data)
		return;

	glPointSize(3.5f);
	glColor4fv(m_color);

	int vertexN = (int)m_data->m_vertices.size();
	glBegin(GL_POINTS);
	for (int i = 0; i < vertexN; i++)
	{
		if (m_data->m_vertices[i].IsD())continue;
		if (m_data->m_vertices[i].IsH())continue;

		if ((int)m_data->m_vertexNormal.size() != 0)
			glNormal3f(m_data->m_vertices[i].N().X(), m_data->m_vertices[i].N().Y(), m_data->m_vertices[i].N().Z());
		if ((int)m_data->m_vertexColor.size() != 0)
			glColor4f(m_data->m_vertexColor[i][0], m_data->m_vertexColor[i][1], m_data->m_vertexColor[i][2], m_data->m_vertexColor[i][3]);
		glVertex3f(m_data->m_vertices[i].m_coord.X(), m_data->m_vertices[i].m_coord.Y(), m_data->m_vertices[i].m_coord.Z());
	}
	glEnd();
}

void sn3DMeshRenderer::BuildPointList()
{
	NewList();
	glNewList(m_list.list, GL_COMPILE);
	DrawPoints();
	glEndList();
}

void sn3DMeshRenderer::BuildFaceList()
{
	NewList();
	glNewList(m_list.list, GL_COMPILE);
	DrawGeneral();
	glEndList();
}

void sn3DMeshRenderer::BuildBufferARB()
{
	glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
	glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
	glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
	glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
	glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
	glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
	glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
	glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");

	boost::shared_ptr<sn3DMeshData> data = boost::dynamic_pointer_cast<sn3DMeshData>(m_data);

	if (m_bufferInit)
	{
		glDeleteBuffersARB(1, &m_vertexBuffer);
		glDeleteBuffersARB(1, &m_indexBuffer);
	}

	glGenBuffersARB(1, &m_vertexBuffer);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vertexBuffer);

	std::vector<float> m_points;
	std::vector<float> m_normals;
	std::vector<unsigned int> m_indices;

	m_points.resize(data->vn * 3);
	m_normals.resize(data->vn * 3);
	m_indices.resize(data->fn * 3);

	for (int i = 0; i < (int)data->vn; i++)
	{
		m_points[i * 3] = data->V(i)->m_coord.X();
		m_points[i * 3 + 1] = data->V(i)->m_coord.Y();
		m_points[i * 3 + 2] = data->V(i)->m_coord.Z();
		m_normals[i * 3] = data->V(i)->N().X();
		m_normals[i * 3 + 1] = data->V(i)->N().Y();
		m_normals[i * 3 + 2] = data->V(i)->N().Z();
	}

	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(GLfloat)*data->vn * 3 * 2, 0, GL_DYNAMIC_DRAW_ARB);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(GLfloat)*data->vn * 3, &m_points[0]);
	glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(GLfloat)*data->vn * 3, sizeof(GLfloat)*data->vn * 3, &m_normals[0]);
	glGenBuffersARB(1, &m_indexBuffer);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_indexBuffer);

	for (int i = 0; i < (int)data->fn; i++)
	{
		m_indices[i * 3] = data->F(i)->V(0)->Index();
		m_indices[i * 3 + 1] = data->F(i)->V(1)->Index();
		m_indices[i * 3 + 2] = data->F(i)->V(2)->Index();
	}

	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(GLuint)*data->fn * 3, &m_indices[0], GL_DYNAMIC_DRAW_ARB);
	m_points.clear();
	m_normals.clear();
	m_indices.clear();
	m_bufferInit = true;
}

void sn3DMeshRenderer::UpdateBufferARB()
{
	if (!m_bufferInit)
		return;

	boost::shared_ptr<sn3DMeshData> data = boost::dynamic_pointer_cast<sn3DMeshData>(m_data);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vertexBuffer);
	float *ptr = (float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB);
	if (ptr)
	{
		int size = data->vn * 3;
		for (int i = 0; i < (int)data->vn; i++)
		{
			ptr[i * 3] = data->V(i)->m_coord.X();
			ptr[i * 3 + 1] = data->V(i)->m_coord.Y();
			ptr[i * 3 + 2] = data->V(i)->m_coord.Z();
			ptr[size + i * 3] = data->V(i)->N().X();
			ptr[size + i * 3 + 1] = data->V(i)->N().Y();
			ptr[size + i * 3 + 2] = data->V(i)->N().Z();
		}
		glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
	}
	m_ReUpdataReader = false; 
}

void sn3DMeshRenderer::ClearBufferARB()
{
	if (m_bufferInit)
	{
		glDeleteBuffersARB(1, &m_vertexBuffer);
		glDeleteBuffersARB(1, &m_indexBuffer);
		m_bufferInit = false;
	}
}

void sn3DMeshRenderer::SetGeometry(boost::shared_ptr<sn3DGeometryData> data)
{
	m_data = data;
	m_ReUpdataReader = true;
}

void sn3DMeshRenderer::SetTransform(float* mat)
{
	for (int i = 0; i < 16; i++)
	{
		m_mat[i] = mat[i];
	}
}

void sn3DMeshRenderer::SetColor(float* color)
{
	bool bUpdate = false;
	for (int i = 0; i < 4; ++i)
	{
		bUpdate = bUpdate || abs(m_color[i] - color[i]) >= 0.0001f;
	}

	if (bUpdate)
	{
		for (int i = 0; i < 4; ++i)
			m_color[i] = color[i];
		m_ReUpdataReader = true;
	}
}

void sn3DMeshRenderer::BuildRangeData()
{
	boost::shared_ptr<sn3DMeshData> data = boost::dynamic_pointer_cast<sn3DMeshData>(m_data);
	if (!data)
		return;
	if (m_bufferInit)
	{
		if (glIsVertexArray(m_vertexArray))
		{
			glDeleteVertexArrays(1, &m_vertexArray);
			m_vertexArray = 0xffffffff;
		}
		if (glIsBuffer(m_vertexBuffer))
		{
			glDeleteBuffers(1, &m_vertexBuffer);
			m_vertexBuffer = 0xffffffff;
		}
		if (glIsBuffer(m_indexBuffer))
		{
			glDeleteBuffers(1, &m_indexBuffer);
			m_indexBuffer = 0xffffffff;
		}
	}

	// 编译着色器
	if(!m_bufferInit)CompileRangeShader();
	//CompileRangeShader();

	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	int strip;	//GLfloat的strip

	if (SMFlat == m_shadeMode)
	{
		int vn = data->m_vertices.size();
		int fn = data->m_faces.size();

		if (vn == 0 || fn == 0)
		{
			return;
		}

		//strip = 3 * 2;
		strip = 3 * 3;
		vertices.resize(fn*3 * strip);

		for (int i = 0; i < fn; i++)
		{
			Face* f = data->F(i);
			for (int j = 0; j < 3; ++j)
			{
				Point3f p = f->V(j)->P();
				Point3f n = f->N();
				Color4f color(m_color[0], m_color[1], m_color[2], m_color[3]);
				if (f->V(j)->Index() < data->m_vertexColor.size())color = data->m_vertexColor[f->V(j)->Index()];

				vertices[(i*3+j) * strip] = p[0];
				vertices[(i*3+j) * strip + 1] = p[1];
				vertices[(i*3+j) * strip + 2] = p[2];

				vertices[(i*3+j) * strip + 3] = n[0];
				vertices[(i*3+j) * strip + 4] = n[1];
				vertices[(i*3+j) * strip + 5] = n[2];

				vertices[(i * 3 + j) * strip + 6] = color[0];
				vertices[(i * 3 + j) * strip + 7] = color[1];
				vertices[(i * 3 + j) * strip + 8] = color[2];
			}		
		}
		indices.resize(fn * 3);
		for (int i = 0; i < fn; i++)
		{
			for (int j = 0; j < 3; ++j)
			{
				indices[i * 3 + j] = i * 3 + j;
				indices[i * 3 + j] = i * 3 + j;
				indices[i * 3 + j] = i * 3 + j;
			}
		}
	}
	else if(SMNormalSplit == m_shadeMode)
	{
		int vn = data->m_vertices.size();
		int fn = data->m_faces.size();

		if (vn == 0 || fn == 0)
		{
			return;
		}

		//strip = 3 * 2;
		strip = 3 * 3;
		vertices.resize(fn * 3 * strip);

		for (int i = 0; i < fn; i++)
		{
			Face* f = data->F(i);
			for (int j = 0; j < 3; ++j)
			{
				Point3f p = f->V(j)->P();
				Point3f n = f->V(j)->N();
				Color4f color(m_color[0], m_color[1], m_color[2], m_color[3]);
				if (f->V(j)->Index() < data->m_vertexColor.size())color = data->m_vertexColor[f->V(j)->Index()];

				vertices[(i * 3 + j) * strip] = p[0];
				vertices[(i * 3 + j) * strip + 1] = p[1];
				vertices[(i * 3 + j) * strip + 2] = p[2];

				vertices[(i * 3 + j) * strip + 3] = n[0];
				vertices[(i * 3 + j) * strip + 4] = n[1];
				vertices[(i * 3 + j) * strip + 5] = n[2];

				vertices[(i * 3 + j) * strip + 6] = color[0];
				vertices[(i * 3 + j) * strip + 7] = color[1];
				vertices[(i * 3 + j) * strip + 8] = color[2];
			}
		}
		indices.resize(fn * 3);
		for (int i = 0; i < fn; i++)
		{
			for (int j = 0; j < 3; ++j)
			{
				indices[i * 3 + j] = i * 3 + j;
				indices[i * 3 + j] = i * 3 + j;
				indices[i * 3 + j] = i * 3 + j;
			}
		}
	}
	else //SMSmooth
	{
		int vn = data->m_vertices.size();
		int fn = data->m_faces.size();

		if (vn == 0 || fn == 0)
		{
			return;
		}

		//strip = 3 * 2;
		strip = 3 * 3;
		vertices.resize(vn * strip);

		for (int i = 0; i < vn; i++)
		{
			Point3f p = data->V(i)->P();
			Point3f n = data->V(i)->N();
			Color4f color(m_color[0], m_color[1], m_color[2], m_color[3]);
			if (data->V(i)->Index() < data->m_vertexColor.size())
				color = data->m_vertexColor[data->V(i)->Index()];

			vertices[i * strip] = p[0];
			vertices[i * strip + 1] = p[1];
			vertices[i * strip + 2] = p[2];

			vertices[i * strip + 3] = n[0];
			vertices[i * strip + 4] = n[1];
			vertices[i * strip + 5] = n[2];

			vertices[i * strip + 6] = color[0];
			vertices[i * strip + 7] = color[1];
			vertices[i * strip + 8] = color[2];
		}
		indices.resize(fn * 3);
		for (int i = 0; i < fn; i++)
		{
			int v0 = data->F(i)->V(0)->Index();
			int v1 = data->F(i)->V(1)->Index();
			int v2 = data->F(i)->V(2)->Index();

			indices[i * 3] = v0;
			indices[i * 3 + 1] = v1;
			indices[i * 3 + 2] = v2;
		}
	}
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	//================= Vertex Attributes ===============

	glGenVertexArrays(1, &m_vertexArray);
	glBindVertexArray(m_vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * strip, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * strip, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * strip, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

	m_bufferInit = true;
	m_ReUpdataReader = false;
}

void sn3DMeshRenderer::DrawRangeData()
{
	switch (m_drawMode)
	{
	case DMPoints:
		 DrawPoints(); 
		 break;
	case DMFill:
		 DrawRangeDataFill();
		 break;
	case DMWire:
		 DrawWire();   
		 break;
	case DMFillWire:
		 DrawRangeDataFillWire();
		 break;
	}
}

void sn3DMeshRenderer::DrawRangeDataFill()
{
	boost::shared_ptr<sn3DMeshData> data = boost::dynamic_pointer_cast<sn3DMeshData>(m_data);
	if (!data)
		return;

	if (m_bufferInit)
	{
		glUseProgram(m_rangeShader);

		glBindVertexArray(m_vertexArray);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		// 设置Uniform参数值
		SetRangeUniform();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		glDrawElements(GL_TRIANGLES, data->fn * 3, GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}
}

void sn3DMeshRenderer::DrawRangeDataFillWire()
{
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1);
	DrawFill();
	glDisable(GL_POLYGON_OFFSET_FILL);

	float oldColor[4];
	for (int i = 0; i < 4; ++i)
		oldColor[i] = m_color[i];

	glEnable(GL_COLOR_MATERIAL);
	m_color[0] = .3f;
	m_color[1] = .3f;
	m_color[2] = .3f;
	m_color[3] = 1.0;

	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	DrawWire();
	glPopAttrib();

	for (int i = 0; i < 4; ++i)
		m_color[i] = oldColor[i];

	glPopAttrib();
}

void sn3DMeshRenderer::CompileRangeShader()
{
	GLenum ret = glewInit();
	if (ret != GLEW_OK)
	{
		qDebug("ycx glewInit failure");
		return;
	}

	if (glIsShader(m_rangeShader))
		return;

	char  vsold[] = "#version 410\n\
layout(location = 0) in vec3 VertexPosition; \n\
layout(location = 1) in vec3 VertexNormal; \n\
layout(location = 2) in vec3 VertexColor; \n\
out vec4 LightIntensity;\n\
struct LightInfo {\n\
vec3 La;       // Ambient light intensity\n\
vec3 Ld;       // Diffuse light intensity\n\
vec3 Ls;       // Specular light intensity\n\
vec3 Position; \n\
};\n\
uniform LightInfo Light; \n\
struct MaterialInfo {\n\
vec3 Ka;            // Ambient reflectivity\n\
vec3 Kd;            // Diffuse reflectivity\n\
vec3 Ks;            // Specular reflectivity\n\
float Shininess;    // Specular shininess factor\n\
};\n\
uniform MaterialInfo Material;\n\
uniform MaterialInfo MaterialBack;\n\
uniform mat4 ModelViewMatrix;\n\
uniform mat3 NormalMatrix;\n\
uniform mat4 ProjectionMatrix;\n\
uniform float Alpha = 1.0f;\n\
//uniform mat4 MVP;\n\
void main()\n\
{\n\
vec3 n = normalize(NormalMatrix * VertexNormal);\n\
vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition, 1.0);\n\
vec3 s = normalize(Light.Position - vec3(eyeCoords));\n\
vec3 v = normalize(-eyeCoords.xyz);\n\
vec3 r = reflect(-s, n);\n\
//vec3 r1 = reflect(-s, (-1)*n);\n\
float sDotN = max(dot(s, n), 0.0);\n\
//float sDotN1 = max(dot(s, (-1)*n), 0.0);\n\
float sDotNV = dot(v, n);\n\
vec3 ambient = vec3(0.0);\n\
vec3 diffuse =  vec3(0.0);\n\
if (sDotNV > 0.0)\n\
{\n\
	//ambient = Light.La * Material.Ka; \n\
	//diffuse = Light.Ld * Material.Kd * sDotN; \n\
    ambient = Light.La * VertexColor; \n\
	diffuse = Light.Ld * VertexColor * sDotN; \n\
}\n\
//else\n\
{\n\
//	ambient = Light.La * MaterialBack.Ka;\n\
//	diffuse = Light.Ld * MaterialBack.Kd * sDotN1;\n\
}\n\
vec3 spec = vec3(0.0);\n\
if(sDotN>0.0)\n\
{\n\
	if (sDotNV > 0.0)\n\
		//spec = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess); \n\
        spec = Light.Ls * VertexColor * pow(max(dot(r, v), 0.0), Material.Shininess); \n\
   // else\n\
   //    spec = Light.Ls * MaterialBack.Ks * pow(max(dot(r1, v), 0.0), MaterialBack.Shininess); \n\
}\n\
LightIntensity = vec4(ambient + diffuse + spec, Alpha); \n\
gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(VertexPosition, 1.0); \n\
}\n\
\n\0";

	char  vs[] = "#version 410\n\
layout(location = 0) in vec3 VertexPosition; \n\
layout(location = 1) in vec3 VertexNormal; \n\
layout(location = 2) in vec3 VertexColor; \n\
out vec4 LightIntensity;\n\
struct LightInfo {\n\
vec3 La;       // Ambient light intensity\n\
vec3 Ld;       // Diffuse light intensity\n\
vec3 Ls;       // Specular light intensity\n\
vec3 Position; \n\
};\n\
uniform LightInfo Light; \n\
struct MaterialInfo {\n\
vec3 Ka;            // Ambient reflectivity\n\
vec3 Kd;            // Diffuse reflectivity\n\
vec3 Ks;            // Specular reflectivity\n\
float Shininess;    // Specular shininess factor\n\
};\n\
uniform MaterialInfo Material;\n\
uniform MaterialInfo MaterialBack;\n\
uniform mat4 ModelViewMatrix;\n\
uniform mat3 NormalMatrix;\n\
uniform mat4 ProjectionMatrix;\n\
uniform float Alpha = 1.0f;\n\
uniform bool EnableVColor = false;\n\
//uniform mat4 MVP;\n\
void main()\n\
{\n\
vec3 n = normalize(NormalMatrix * VertexNormal);\n\
vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition, 1.0);\n\
vec3 s = normalize(Light.Position - vec3(eyeCoords));\n\
vec3 v = normalize(-eyeCoords.xyz);\n\
vec3 r = reflect(-s, n);\n\
vec3 r1 = reflect(-s, (-1)*n);\n\
float sDotN = max(dot(s, n), 0.0);\n\
float sDotN1 = max(dot(s, (-1)*n), 0.0);\n\
float sDotNV = dot(v, n);\n\
vec3 ambient = vec3(0.0);\n\
vec3 diffuse =  vec3(0.0);\n\
vec3 Light_La = Light.La*1.8f;\n\
if (sDotNV > 0.0)\n\
{\n\
	if(!EnableVColor)\n\
	{\n\
		ambient = Light_La * Material.Ka; \n\
		diffuse = Light.Ld * Material.Kd * sDotN; \n\
	}\n\
	else\n\
	{\n\
		ambient = Light_La * VertexColor; \n\
		diffuse = Light.Ld * VertexColor * sDotN; \n\
	}\n\
}\n\
else\n\
{\n\
	if(!EnableVColor){\n\
		ambient = Light_La * MaterialBack.Ka; \n\
		diffuse = Light.Ld * MaterialBack.Kd * sDotN1; \n\
	}\n\
}\n\
vec3 spec = vec3(0.0);\n\
if(sDotN>0.0)\n\
{\n\
	if (sDotNV > 0.0)\n\
	{\n\
	    if(!EnableVColor)spec = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess); \n\
        else spec = Light.Ls * VertexColor * pow(max(dot(r, v), 0.0), Material.Shininess); \n\
    }\n\
	else\n\
		if(!EnableVColor)spec = Light.Ls * MaterialBack.Ks * pow(max(dot(r1, v), 0.0), MaterialBack.Shininess); \n\
}\n\
LightIntensity = vec4(ambient + diffuse + spec, Alpha); \n\
gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(VertexPosition, 1.0); \n\
}\n\
\n\0";

char  fs[] = "#version 410\n\
in vec4 LightIntensity; \n\
layout(location = 0) out vec4 FragColor; \n\
void main() \n\
{\n\
FragColor = LightIntensity; \n\
}\n\0";

	/*Shader m_shader;
	m_shader.init();
	m_shader.attach(GL_VERTEX_SHADER, vs);
	m_shader.attach(GL_FRAGMENT_SHADER, fs);
	m_shader.link();
	m_rangeShader = m_shader.GetProgram();*/
    m_rangeShader = ShadeAllFileLoadData::getInstance().GetShaderProgram(EN_SHADETYPE_COM);
}

void sn3DMeshRenderer::SetRangeUniform()
{
	float m_MvMatrix[16];
	float m_ProjMatrix[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, m_MvMatrix);
	glGetFloatv(GL_PROJECTION_MATRIX, m_ProjMatrix);

	Matrix44f mvMat, projMat, mat;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			mvMat[i][j] = m_MvMatrix[i * 4 + j];
			projMat[i][j] = m_ProjMatrix[i * 4 + j];

			//mat[i][j] = m_mat[i * 4 + j];
		}
	//mvMat = mat * mvMat;

	Matrix33f normalMat;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			normalMat[i][j] = mvMat[i][j];

	Matrix44f mvpMat = projMat * mvMat;

	Point3f lightDiffuse;
	Point3f lightSpecular;
	Point3f lightAmbient;
	float lightPos[3];
	
	glGetLightfv(GL_LIGHT0, GL_AMBIENT, &lightAmbient[0]);
	glGetLightfv(GL_LIGHT0, GL_DIFFUSE, &lightDiffuse[0]);
	glGetLightfv(GL_LIGHT0, GL_SPECULAR, &lightSpecular[0]);
	glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	Point3f materialDiffuse[2];
	Point3f materialSpecular[2];
	Point3f materialAmbient[2];
	//float shininess = 128.f;
	float shininess = 20.f;
	bool isEVcolor = true;
	//正反面
	for (int i = 0; i<2; i++)
	{
		materialAmbient[i] = m_color;
		materialDiffuse[i] = m_color;
		materialSpecular[i] = Point3f(0.0, 0.0, 0.0);
		//materialSpecular[i] = Point3f(0.5, 0.5, 0.5);
	}
	if (m_colorMode == CMMaterial && !m_material.empty())
	{
		/*std::map<MaterialFace, MaterialInfo>::iterator iter = m_material.find(MF_FRONT);
		if (iter == m_material.end())
			iter = m_material.find(MF_FRONT_AND_BACK);
		materialAmbient = Point3f(iter->second.ambient[0], iter->second.ambient[1], iter->second.ambient[2]);
		materialDiffuse = Point3f(iter->second.diffuse[0], iter->second.diffuse[1], iter->second.diffuse[2]);
		materialSpecular = Point3f(iter->second.specular[0], iter->second.specular[1], iter->second.specular[2]);*/
		std::map<MaterialFace, MaterialInfo>::iterator iter = m_material.find(MF_FRONT);
		std::map<MaterialFace, MaterialInfo>::iterator iterBack = m_material.find(MF_BACK);
		std::map<MaterialFace, MaterialInfo>::iterator iterOr = m_material.find(MF_FRONT_AND_BACK);
		if (iter == m_material.end() && iterBack == m_material.end() && iterOr != m_material.end())
		{
			//正反面
			for (int i = 0; i<2; i++)
			{
				materialAmbient[i] = Point3f(iterOr->second.ambient[0], iterOr->second.ambient[1], iterOr->second.ambient[2]);
				materialDiffuse[i] = Point3f(iterOr->second.diffuse[0], iterOr->second.diffuse[1], iterOr->second.diffuse[2]);
				materialSpecular[i] = Point3f(iterOr->second.specular[0], iterOr->second.specular[1], iterOr->second.specular[2]);
			}
		}
		else if (iter != m_material.end() && iterBack == m_material.end())
		{
			//正反面
			for (int i = 0; i<2; i++)
			{
				materialAmbient[i] = Point3f(iter->second.ambient[0], iter->second.ambient[1], iter->second.ambient[2]);
				materialDiffuse[i] = Point3f(iter->second.diffuse[0], iter->second.diffuse[1], iter->second.diffuse[2]);
				materialSpecular[i] = Point3f(iter->second.specular[0], iter->second.specular[1], iter->second.specular[2]);
			}
		}
		else if (iter == m_material.end() && iterBack != m_material.end())
		{
			//正反面
			for (int i = 0; i<2; i++)
			{
				materialAmbient[i] = Point3f(iterBack->second.ambient[0], iterBack->second.ambient[1], iterBack->second.ambient[2]);
				materialDiffuse[i] = Point3f(iterBack->second.diffuse[0], iterBack->second.diffuse[1], iterBack->second.diffuse[2]);
				materialSpecular[i] = Point3f(iterBack->second.specular[0], iterBack->second.specular[1], iterBack->second.specular[2]);
			}
		}
		else if (iter != m_material.end() && iterBack != m_material.end())
		{
			//正面
			materialAmbient[0] = Point3f(iter->second.ambient[0], iter->second.ambient[1], iter->second.ambient[2]);
			materialDiffuse[0] = Point3f(iter->second.diffuse[0], iter->second.diffuse[1], iter->second.diffuse[2]);
			materialSpecular[0] = Point3f(iter->second.specular[0], iter->second.specular[1], iter->second.specular[2]);
			//反面
			materialAmbient[1] = Point3f(iterBack->second.ambient[0], iterBack->second.ambient[1], iterBack->second.ambient[2]);
			materialDiffuse[1] = Point3f(iterBack->second.diffuse[0], iterBack->second.diffuse[1], iterBack->second.diffuse[2]);
			materialSpecular[1] = Point3f(iterBack->second.specular[0], iterBack->second.specular[1], iterBack->second.specular[2]);
		}
		isEVcolor = false;
	}
	
	
	int g_uniformLightDiffuse = glGetUniformLocation(m_rangeShader, "Light.Ld");
	int g_uniformLightAmbient = glGetUniformLocation(m_rangeShader, "Light.La");
	int g_uniformLightSpecular = glGetUniformLocation(m_rangeShader, "Light.Ls");
	int g_uniformLightPosition = glGetUniformLocation(m_rangeShader, "Light.Position");

	//正面材质
	int g_uniformMaterialDiffuse = glGetUniformLocation(m_rangeShader, "Material.Kd");
	int g_uniformMaterialAmbient = glGetUniformLocation(m_rangeShader, "Material.Ka");
	int g_uniformMaterialSpecular = glGetUniformLocation(m_rangeShader, "Material.Ks");
	int g_uniformMaterialShininess = glGetUniformLocation(m_rangeShader, "Material.Shininess");
	//背面材质
	int g_uniformMaterialBackDiffuse = glGetUniformLocation(m_rangeShader, "MaterialBack.Kd");
	int g_uniformMaterialBackAmbient = glGetUniformLocation(m_rangeShader, "MaterialBack.Ka");
	int g_uniformMaterialBackSpecular = glGetUniformLocation(m_rangeShader, "MaterialBack.Ks");
	int g_uniformMaterialBackShininess = glGetUniformLocation(m_rangeShader, "MaterialBack.Shininess");

	int g_uniformMVMat = glGetUniformLocation(m_rangeShader, "ModelViewMatrix");
	int g_uniformNormalMat = glGetUniformLocation(m_rangeShader, "NormalMatrix");
	int g_uniformProjMat = glGetUniformLocation(m_rangeShader, "ProjectionMatrix");

	//透明通道
	int g_uniformAlpha = glGetUniformLocation(m_rangeShader, "Alpha");
	//是否用顶点颜色
	int g_uniformIsEVcolor = glGetUniformLocation(m_rangeShader, "EnableVColor");

	//////////////////////////////////////////////////////////////
	glUniform3fv(g_uniformLightDiffuse, 1, &lightDiffuse[0]);
	glUniform3fv(g_uniformLightAmbient, 1, &lightAmbient[0]);
	glUniform3fv(g_uniformLightSpecular, 1, &lightSpecular[0]);
	glUniform3fv(g_uniformLightPosition, 1, &lightPos[0]);
	//正面
	glUniform3fv(g_uniformMaterialDiffuse, 1, &materialDiffuse[0][0]);
	glUniform3fv(g_uniformMaterialAmbient, 1, &materialAmbient[0][0]);
	glUniform3fv(g_uniformMaterialSpecular, 1, &materialSpecular[0][0]);
	glUniform1f(g_uniformMaterialShininess, shininess);
	//反面
	glUniform3fv(g_uniformMaterialBackDiffuse, 1, &materialDiffuse[1][0]);
	glUniform3fv(g_uniformMaterialBackAmbient, 1, &materialAmbient[1][0]);
	glUniform3fv(g_uniformMaterialBackSpecular, 1, &materialSpecular[1][0]);
	glUniform1f(g_uniformMaterialBackShininess, shininess);

	glUniformMatrix4fv(g_uniformMVMat, 1, GL_FALSE, &mvMat[0][0]);
	glUniformMatrix3fv(g_uniformNormalMat, 1, GL_FALSE, &normalMat[0][0]);
	glUniformMatrix4fv(g_uniformProjMat, 1, GL_FALSE, &projMat[0][0]);

	//透明通道
	glUniform1f(g_uniformAlpha, m_color[3]);
	//是否用颜色着色器
	glUniform1i(g_uniformIsEVcolor, (int)isEVcolor);
}

void sn3DMeshRenderer::RenderSelected()
{
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT |
		GL_DEPTH_BUFFER_BIT | GL_POINT_BIT | GL_LINE_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_COLOR_MATERIAL);
	glColor4f(0.7f, 0.0, 0.0, 0.5f);
	glPolygonOffset(-5.5, -5.5);

	//===========================
	std::vector<Face>::iterator fi;

	// 绘制被选中数据
	glBegin(GL_TRIANGLES);
	for (fi = m_data->m_faces.begin(); fi != m_data->m_faces.end(); ++fi)
	{
		if ((*fi).IsD())continue;
		if ((*fi).IsS() && !(*fi).IsM())
		{
			glVertex3f((*fi).P(0).X(), (*fi).P(0).Y(), (*fi).P(0).Z());
			glVertex3f((*fi).P(1).X(), (*fi).P(1).Y(), (*fi).P(1).Z());
			glVertex3f((*fi).P(2).X(), (*fi).P(2).Y(), (*fi).P(2).Z());
		}
	}
	glEnd();

	//绘制被修改数据
	glColor4f(0.0f, 1.0, 0.0, 0.5f);
	glBegin(GL_TRIANGLES);
	for (fi = m_data->m_faces.begin(); fi != m_data->m_faces.end(); ++fi)
	{
		if ((*fi).IsD())continue;
		if ((*fi).IsM())
		{
			glVertex3f((*fi).P(0).X(), (*fi).P(0).Y(), (*fi).P(0).Z());
			glVertex3f((*fi).P(1).X(), (*fi).P(1).Y(), (*fi).P(1).Z());
			glVertex3f((*fi).P(2).X(), (*fi).P(2).Y(), (*fi).P(2).Z());
		}
	}
	glEnd();
	glPopAttrib();

	return;
	//================================================
	{
		glPointSize(4.5);
		std::vector<Vertex>::iterator vi;
		glBegin(GL_POINTS);
		for (vi = m_data->m_vertices.begin(); vi != m_data->m_vertices.end(); ++vi)
		{
			if ((*vi).IsD())continue;
			if (!(*vi).IsM() && !(*vi).IsM1())continue;

			if ((*vi).IsM())
				glColor4f(1.0f, 0.0, 0.0, 1.0f);
			else
				glColor4f(0.0f, 0.0, 1.0, 1.0f);

			glVertex3f((*vi).m_coord.X(), (*vi).m_coord.Y(), (*vi).m_coord.Z());
		}
		glEnd();
	}
	glPopAttrib();
}
void sn3DMeshRenderer::RenderDecorate(int index)
{
	if (index > 1)return;
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_POINT_BIT);

	glEnable(GL_POLYGON_OFFSET_FILL);

	glDisable(GL_LIGHTING);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);

	glEnable(GL_CULL_FACE);

	glDepthMask(GL_FALSE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (index == 0)
		glColor4f(0.7f, 0.0, 0.0, 0.4f);
	else if (index == 1)
		glColor4f(0.196f, 0.488f, 0.69f, 0.4f);


	glPolygonOffset(-5.5, -5.5);

	glPointSize(5.5);

	std::vector<Vertex>::iterator vi;
	std::vector<Face>::iterator fi;

	if (m_data->m_faces.size() == 0)
	{
		glBegin(GL_POINTS);
		for (vi = m_data->m_vertices.begin(); vi != m_data->m_vertices.end(); ++vi)
		{
			if ((*vi).IsD())continue;
			if (!(*vi).IsS())continue;
			glVertex3f((*vi).m_coord.X(), (*vi).m_coord.Y(), (*vi).m_coord.Z());
		}
		glEnd();
	}
	else
	{
		glBegin(GL_TRIANGLES);
		for (fi = m_data->m_faces.begin(); fi != m_data->m_faces.end(); ++fi)
		{
			glVertex3f((*fi).P(0).X(), (*fi).P(0).Y(), (*fi).P(0).Z());
			glVertex3f((*fi).P(1).X(), (*fi).P(1).Y(), (*fi).P(1).Z());
			glVertex3f((*fi).P(2).X(), (*fi).P(2).Y(), (*fi).P(2).Z());
		}
		glEnd();
	}
	glPopAttrib();
}

void sn3DMeshRenderer::SetDrawMode(DrawMode dm)
{
	sn3DRenderer::SetDrawMode(dm);
	//Update();
	m_ReUpdataReader = true;
}

void sn3DMeshRenderer::SetRenderMethod(RenderMethod rm)
{
	sn3DRenderer::SetRenderMethod(rm);
	//Update();
	m_ReUpdataReader = true;
}

void sn3DMeshRenderer::SetShadeMode(ShadeMode sm)
{
	sn3DRenderer::SetShadeMode(sm);
	//Update();
	m_ReUpdataReader = true;
}

void sn3DMeshRenderer::SetColorMode(ColorMode cm)
{
	bool bUpdate = false;
	if (m_colorMode != cm)
	{
		bUpdate = true;
	}

	if (bUpdate)
	{
		sn3DRenderer::SetColorMode(cm);
		m_ReUpdataReader = true;
	}
}