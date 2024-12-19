//#include"stdafx.h"
#include "GreyShader.h"

#include <stdio.h>
#include <vector>
#include <gl/glew.h>

#include"Common/Point3.h"


GreyShader::GreyShader()
{

	GLchar  vs[] = "void main()\n\
                   {\n\
				    gl_Position = ftransform();\n\
					gl_TexCoord[0] = gl_MultiTexCoord0; \n\
                   }\n\0";

	GLchar  fs[] = "uniform sampler2D imageTexture; \n\
		             void main(){\n\
					 gl_FragColor.rgba = texture2D(imageTexture,gl_TexCoord[0].st).rrra;\n\
                     }\n\0";
	m_vs = vs;
	m_fs = fs;
}
GreyShader::~GreyShader()
{
}
void GreyShader::attachVertexShader()
{
	GLuint shaderId = CompileGLSLShader(GL_VERTEX_SHADER, m_vs.c_str());

	_vertexShaders.push_back(shaderId);
}

void GreyShader::attachFragmentShader()
{
	GLuint shaderId = CompileGLSLShader(GL_FRAGMENT_SHADER, m_fs.c_str());

	_fragmentShaders.push_back(shaderId);
}
