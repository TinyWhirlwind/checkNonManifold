#include "sn3DPointsRenderer.h"

#include <windows.h>
#include <gl/glew.h>

#include "SceneRender/Shader.h"
#include "GeoData/Face.h"
#include "GeoData/PointsData.h"
#include "SceneParam.h"

#ifndef RENDER_SCALE 
#define RENDER_SCALE 200000
#endif

namespace sn3DCore
{
	Shader shader;
	void  sn3DPointsRenderer::CompileRangeShader()
	{
		g_rangeShader = 0xffffffff;
		glewInit();
		if (glIsShader(g_rangeShader))
			return;

		char  vs[] = "#version 430 core \n\
                  layout(location = 0) in vec3 vPosition;\n\
                  layout(location = 1) in vec3 vNormal;\n\
				  out vec4 PositionW; // Position in world space.\n\
                  out vec4 NormalW;  // Surface normal in world space.\n\
                  // Model, View, Projection matrix.\n\
                  uniform mat4 ProjectionMatrix;\n\
                  uniform mat4 ModelMatrix;\n\
	              void main()\n\
                  {\n\
				   gl_Position = ProjectionMatrix * ModelMatrix * vec4(vPosition, 1);\n\
                   PositionW = ModelMatrix * vec4(vPosition, 1); \n\
                   NormalW = ModelMatrix * vec4(vNormal, 0);\n\
				  }\n\0";

		char  fs[] = "#version 430 core \n\
                  layout (location = 0) out vec4 fragColor;\n\
				  in vec4 PositionW; // Position in world space.\n\
                  in vec4 NormalW; // Surface normal in world space.\n\
				  uniform vec4 RangeColor; //\n\
				  uniform vec4 EyePosW;   // Eye position in world space.\n\
                  uniform vec4 LightPosW; // Light's position in world space.\n\
                  uniform vec4 LightColor; // Light's diffuse and specular contribution.\n\
                  uniform vec4 MaterialEmissive;\n\
                  uniform vec4 MaterialDiffuse;\n\
                  uniform vec4 MaterialSpecular;\n\
                  uniform float MaterialShininess;\n\
                  uniform vec4 Ambient; // Global ambient contribution\n\
	              void main()\n\
                  {\n\
					fragColor = vec4(1.0,0.0,0.0,1.0);\n\
				  }\n\0";

		shader.init();
		shader.attach(GL_VERTEX_SHADER, vs);
		shader.attach(GL_FRAGMENT_SHADER, fs);
		shader.link();
		g_rangeShader = shader.GetProgram();
	}

	sn3DPointsRenderer::sn3DPointsRenderer()
	{
		m_data = 0;
		m_bufferInit = false;
	}

	sn3DPointsRenderer::~sn3DPointsRenderer()
	{

	}

	void sn3DPointsRenderer::Draw()
	{
		boost::shared_ptr<sn3DPointsData> data = boost::dynamic_pointer_cast<sn3DPointsData>(m_data);
		size_t vn = data->vn;

		if (m_renderMethod == RMDisplayList)
		{
			glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_POINT_BIT);

			glPointSize(0.5);
			glEnable(GL_POINT_SMOOTH);

			if ((int)data->m_vertexColor.size() == vn)
			{
				glDisable(GL_LIGHTING);
				glDisable(GL_COLOR_MATERIAL);
			}
			else if ((int)data->m_vertexNormal.size() == vn)
			{
				glEnable(GL_LIGHTING);
				glEnable(GL_COLOR_MATERIAL);
				glColor4f(0.235f, 0.5137f, 0.8706f, 1.0f);
			}
			else
			{
				glDisable(GL_LIGHTING);
				glDisable(GL_COLOR_MATERIAL);
				glColor4f(0.0f, 0.69f, 0.957f, 1.0f);
			}
			if (glIsList(m_list.list))
			{
				glCallList(m_list.list);
			}
			glPopAttrib();
		}

		else if (m_renderMethod == RMBufferObject || m_renderMethod == RMShader)
		{
			glPushMatrix();

			Color4f frontColor;
			Color4f bkColor = Color4f(0.4706f, 0.4706f, 0.4706f, 1.0f);

			float m_MvMatrix[16];
			float m_ProjMatrix[16];
			float lightPosition[4]; // 碌脝鹿芒脭脷脢脫碌茫脦禄脰脙

			float white[4] = { 1.0f,1.0f,1.0f,1.0f};
			float black[4] = { 0.0f,0.0f,0.0f,1.0f };
			float gray[4] = { 0.02f,0.02f,0.02f,1.0f };
			float ambient[4] = { 0.02f,0.02f,0.02f,1.0f };
			float rangeColor[4] = { 0.0f,0.69f,0.957f,1.0f };

			glGetFloatv(GL_MODELVIEW_MATRIX, m_MvMatrix);
			glGetFloatv(GL_PROJECTION_MATRIX, m_ProjMatrix);

			lightPosition[0] = -m_MvMatrix[12];
			lightPosition[1] = -m_MvMatrix[13];
			lightPosition[2] = -m_MvMatrix[14];
			lightPosition[3] = m_MvMatrix[15];

			int g_uniformMVP = glGetUniformLocation(g_rangeShader, "ProjectionMatrix");
			int g_uniformModelMatrix = glGetUniformLocation(g_rangeShader, "ModelMatrix");

			// Light properties.n 
			int g_uniformLightPosW = glGetUniformLocation(g_rangeShader, "LightPosW");
			int g_uniformLightColor = glGetUniformLocation(g_rangeShader, "LightColor");
			int g_uniformEyePosW = glGetUniformLocation(g_rangeShader, "EyePosW");

			// Global ambient.
			int g_uniformAmbient = glGetUniformLocation(g_rangeShader, "Ambient");

			// Material properties.
			int g_uniformMaterialEmissive = glGetUniformLocation(g_rangeShader, "MaterialEmissive");
			int g_uniformMaterialDiffuse = glGetUniformLocation(g_rangeShader, "MaterialDiffuse");
			int g_uniformMaterialSpecular = glGetUniformLocation(g_rangeShader, "MaterialSpecular");
			int g_uniformMaterialShininess = glGetUniformLocation(g_rangeShader, "MaterialShininess");

			int g_uniformRangeColor = glGetUniformLocation(g_rangeShader, "RangeColor");

			glUseProgram(g_rangeShader);

			glUniformMatrix4fv(g_uniformMVP, 1, GL_FALSE, m_ProjMatrix);
			glUniformMatrix4fv(g_uniformModelMatrix, 1, GL_FALSE, m_MvMatrix);

			glBindVertexArray(m_vertexArray); 
			glDrawArrays(GL_POINTS, 0, (int)(vn / 10)); 
			glUseProgram(0);
			glBindVertexArray(0);
			glPopMatrix();
		}
	}

	void sn3DPointsRenderer::Update()
	{
		boost::shared_ptr<sn3DPointsData> data = boost::dynamic_pointer_cast<sn3DPointsData>(m_data);
		size_t vn = data->vn;

		if (m_renderMethod == RMDisplayList)
		{
			BuildList();
		}
		else if (m_renderMethod == RMBufferObject)
		{
			CompileRangeShader();

			float * coord = new float[vn * 3];
			float * normal = new float[vn * 3];

			for (int i = 0; i < vn; i++)
			{
				Vertex * v = data->V(i);
				Point3f p = v->P();
				Point3f n = Point3f(0.0, 0.0, 0.0);

				coord[i * 3] = p[0];
				coord[i * 3 + 1] = p[1];
				coord[i * 3 + 2] = p[2];

				if (v->IsS()) // selected
				{
					normal[i * 3] = 1.0;
					normal[i * 3 + 1] = 1.0;
					normal[i * 3 + 2] = 1.0;
				}
				else
				{
					normal[i * 3] = n[0];
					normal[i * 3 + 1] = n[1];
					normal[i * 3 + 2] = n[2];
				}
			}

			if (!glIsVertexArray(m_vertexArray))
				glGenVertexArrays(1, &m_vertexArray);
			glBindVertexArray(m_vertexArray);

			if (!glIsBuffer(m_vertexBuffer))
				glGenBuffers(1, &m_vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vn * 3, coord, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

			if (!glIsBuffer(m_normalBuffer))
				glGenBuffers(1, &m_normalBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vn * 3, normal, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			delete coord;
			delete normal;
		}
	}
	void sn3DPointsRenderer::BuildList()
	{
		NewList();
		glNewList(m_list.list, GL_COMPILE);
		size_t vertexN = (int)m_data->m_vertices.size();

		glBegin(GL_POINTS);
		for (size_t i = 0; i < vertexN; i++)
		{
			if (m_data->m_vertices[i].IsS())
				continue;
			if ((int)m_data->m_vertexNormal.size() == vertexN)
				glNormal3f(m_data->m_vertices[i].N().X(), m_data->m_vertices[i].N().Y(), m_data->m_vertices[i].N().Z());
			if ((int)m_data->m_vertexColor.size() == vertexN)
				glColor4f(m_data->m_vertexColor[i][0], m_data->m_vertexColor[i][1], m_data->m_vertexColor[i][2], m_data->m_vertexColor[i][3]);
			glVertex3f(m_data->m_vertices[i].m_coord.X(), m_data->m_vertices[i].m_coord.Y(), m_data->m_vertices[i].m_coord.Z());
		}
		glEnd();
		glEndList();
	}

	void sn3DPointsRenderer::BuildBuffer()
	{
	}

	void sn3DPointsRenderer::UpdateBuffer()
	{
	}

	void sn3DPointsRenderer::ClearBuffer()
	{
	}

	void sn3DPointsRenderer::SetGeometry(boost::shared_ptr<sn3DGeometryData> data)
	{
		Clear();
		m_data = data;
	}
}