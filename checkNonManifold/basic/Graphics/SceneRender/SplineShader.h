#ifndef _SplineShader_
#define _SplineShader_

#include "SceneRender/GLSLProgramObject.h"

/* Spline着色器程序 */
class SplineShader : public GLSLProgramObject
{
public:
	SplineShader();
	virtual ~SplineShader();

	virtual void attachVertexShader();
	virtual void attachFragmentShader();

public:
	std::string  m_vs;
	std::string  m_fs;
};
#endif