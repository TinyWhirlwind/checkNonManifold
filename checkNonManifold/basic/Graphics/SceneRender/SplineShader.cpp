//#include"stdafx.h"
#include "SplineShader.h"

#include <gl/glew.h>

SplineShader::SplineShader()
{
	//vert shader
	char vs[] =
		"void main(void)\
  {\
	gl_Position = ftransform();\n\
	gl_FrontColor = gl_Color;\n\
  }\0";

	// fragment shader
	char fs[] =
		"uniform sampler2D DepthTex;\n\
  uniform float screenWidth;\n\
  uniform float screenHeight;\n\
  void main(void)\n\
  {\n\
   gl_FragColor = gl_Color;\n\
   vec2 texCoords = vec2(gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight);\n\
   if(abs(gl_FragCoord.z-texture2D(DepthTex, texCoords).x)>0.00006)\n\
   discard;\n\
  }\0";

	//0.00006

	m_vs = vs;
	m_fs = fs;
}
SplineShader::~SplineShader()
{
}
void SplineShader::attachVertexShader()
{
	GLuint shaderId = CompileGLSLShader(GL_VERTEX_SHADER, m_vs.c_str());

	_vertexShaders.push_back(shaderId);
}

void SplineShader::attachFragmentShader()
{
	GLuint shaderId = CompileGLSLShader(GL_FRAGMENT_SHADER, m_fs.c_str());

	_fragmentShaders.push_back(shaderId);
}