#ifndef _GreyShader_
#define _GreyShader_

#include <string>
#include "GLSLProgramObject.h"

class GreyShader : public GLSLProgramObject
{
public:
	GreyShader();
	virtual ~GreyShader();

	virtual void attachVertexShader();
	virtual void attachFragmentShader();

private:
	std::string  m_vs;
	std::string  m_fs;
};
#endif