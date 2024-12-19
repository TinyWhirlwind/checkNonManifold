//#include"stdafx.h"
#include "NormalColorShader.h"

#include <stdio.h>
#include <vector>
#include <gl/glew.h>
#include "Common/Point3.h"

NormalColorShader::NormalColorShader()
{
	char shade_vertex[] =
		"varying vec3 normal;\
  void main()\
  {\
	normal = gl_NormalMatrix * gl_Normal;\
	gl_Position = ftransform();\
  }";

	char shade_fragment[] =
		"varying vec3 normal;\
  void main()\
  {\
   vec3 color = vec3(normalize(normal));\
   color = color * 0.6 + 0.4;\
   gl_FragColor = vec4(color, 1.0);\
  }";

	m_shade_vertex = shade_vertex;
	m_shade_fragment = shade_fragment;

}
NormalColorShader::~NormalColorShader()
{

}
void NormalColorShader::attachVertexShader()
{
	GLuint shaderId = CompileGLSLShader(GL_VERTEX_SHADER, m_shade_vertex.c_str());

	_vertexShaders.push_back(shaderId);
}

void NormalColorShader::attachFragmentShader()
{
	GLuint shaderId = CompileGLSLShader(GL_FRAGMENT_SHADER, m_shade_fragment.c_str());

	_fragmentShaders.push_back(shaderId);
}