#ifndef _NormalColorShader_
#define _NormalColorShader_

#include <gl/glew.h>

#include"GLSLProgramObject.h"

class NormalColorShader : public GLSLProgramObject
{
public:
	NormalColorShader();
	virtual ~NormalColorShader();

	virtual void attachVertexShader();
	virtual void attachFragmentShader();

public:
	std::string m_shade_vertex;
	std::string m_shade_fragment;
};
#endif