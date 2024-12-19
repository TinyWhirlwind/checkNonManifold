#ifndef _PHONGSHADER_
#define _PHONGSHADER_

#include <string>
#include <windows.h>
#include <gl/glew.h>

#include "GLSLProgramObject.h"

/* Phonge着色器程序 */
class PhongShader : public GLSLProgramObject
{
public:
	PhongShader();
	virtual ~PhongShader();

	virtual void attachVertexShader();
	virtual void attachFragmentShader();

private:
	std::string  m_vs;
	std::string  m_fs;
};
#endif