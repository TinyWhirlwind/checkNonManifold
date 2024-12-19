#ifndef _GLSLPROGRAMOBJECT_
#define _GLSLPROGRAMOBJECT_

#include <windows.h>
#include <gl/glew.h>
#include <string>
#include <vector>
#include "Common/Point3.h"

/* GLSL程序类 */
class GLSLProgramObject
{
public:

	void bind();     //  启用shader

	void unbind();   //  退出shader

	virtual void attachVertexShader();

	virtual void attachFragmentShader();

	void setUniform(std::string name, GLfloat* val, int count);

	void setTextureUnit(std::string texname, int texunit);

	void bindTexture(GLenum target, std::string texname, GLuint texid, int texunit);

	void bindTexture2D(std::string texname, GLuint texid, int texunit);

	void bindTexture3D(std::string texname, GLuint texid, int texunit);

	void bindTextureRECT(std::string texname, GLuint texid, int texunit);

protected:

	void link();

	void destroy();

	GLuint CompileGLSLShader(GLenum target, const char* shader);

	inline GLuint getProgId() { return _progId; }

public:

	GLSLProgramObject();

	virtual ~GLSLProgramObject();

protected:
	bool _init;
	std::vector<GLuint>		_vertexShaders;
	std::vector<GLuint>		_fragmentShaders;
	GLuint					_progId;
};
#endif