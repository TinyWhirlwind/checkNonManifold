#include"GLSLProgramObject.h"
#include <gl/glew.h>

GLSLProgramObject::GLSLProgramObject() :_progId(0)
{
	glewInit();
	_init = false;
}

GLSLProgramObject::~GLSLProgramObject()
{
	destroy();
}

void GLSLProgramObject::destroy()
{
	for (unsigned i = 0; i < _vertexShaders.size(); i++)
	{
		glDeleteShader(_vertexShaders[i]);
	}
	for (unsigned i = 0; i < _fragmentShaders.size(); i++)
	{
		glDeleteShader(_fragmentShaders[i]);
	}
	if (_progId != 0)
	{
		glDeleteProgram(_progId);
	}
}
GLuint GLSLProgramObject::CompileGLSLShader(GLenum target, const char* shader)
{
	GLuint object;

	object = glCreateShader(target);

	if (!object) return object;

	glShaderSource(object, 1, &shader, NULL);

	glCompileShader(object);

	// check if shader compiled
	GLint compiled = 0;
	glGetShaderiv(object, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		glDeleteShader(object);
		return 0;
	}

	return object;
}
void GLSLProgramObject::attachVertexShader()
{

}

void GLSLProgramObject::attachFragmentShader()
{

}
void GLSLProgramObject::link()
{
	_progId = glCreateProgram();

	for (unsigned i = 0; i < _vertexShaders.size(); i++)
	{
		glAttachShader(_progId, _vertexShaders[i]);
	}

	for (unsigned i = 0; i < _fragmentShaders.size(); i++)
	{
		glAttachShader(_progId, _fragmentShaders[i]);
	}

	glLinkProgram(_progId);

	GLint success = 0;
	glGetProgramiv(_progId, GL_LINK_STATUS, &success);

	if (!success) {
		char temp[1024];
		glGetProgramInfoLog(_progId, 1024, NULL, temp);
		printf("Failed to link program:\n%s\n", temp);
		exit(1);
	}
}

void GLSLProgramObject::bind()
{
	if (!_init)
	{
		_init = true;
		attachVertexShader();
		attachFragmentShader();
		link();
	}

	glUseProgram(_progId);
}

void GLSLProgramObject::unbind()
{
	glUseProgram(0);
}

void GLSLProgramObject::setUniform(std::string name, GLfloat* val, int count)
{
	GLint id = glGetUniformLocation(_progId, name.c_str());
	if (id == -1)
	{
		return;
	}
	switch (count)
	{
	case 1:
		glUniform1fv(id, 1, val);
		break;
	case 2:
		glUniform2fv(id, 1, val);
		break;
	case 3:
		glUniform3fv(id, 1, val);
		break;
	case 4:
		glUniform4fv(id, 1, val);
		break;
	}
}

void GLSLProgramObject::setTextureUnit(std::string texname, int texunit)
{
	GLint linked;
	glGetProgramiv(_progId, GL_LINK_STATUS, &linked);
	if (linked != GL_TRUE)
	{
		exit(1);
	}
	GLint id = glGetUniformLocation(_progId, texname.c_str());
	if (id == -1)
	{
		return;
	}
	glUniform1i(id, texunit);
}

void GLSLProgramObject::bindTexture(GLenum target, std::string texname, GLuint texid, int texunit)
{
	glActiveTexture(GL_TEXTURE0 + texunit);
	glBindTexture(target, texid);
	setTextureUnit(texname, texunit);
	glActiveTexture(GL_TEXTURE0);
}

void GLSLProgramObject::bindTexture2D(std::string texname, GLuint texid, int texunit)
{
	bindTexture(GL_TEXTURE_2D, texname, texid, texunit);
}
void GLSLProgramObject::bindTexture3D(std::string texname, GLuint texid, int texunit)
{
	bindTexture(GL_TEXTURE_3D, texname, texid, texunit);
}
void GLSLProgramObject::bindTextureRECT(std::string texname, GLuint texid, int texunit)
{
	bindTexture(GL_TEXTURE_2D, texname, texid, texunit);
}
