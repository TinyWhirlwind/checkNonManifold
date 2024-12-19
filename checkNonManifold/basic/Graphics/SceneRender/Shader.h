#ifndef Shader_H
#define Shader_H

#include <string>
#include <windows.h>
#include <GL/glew.h>

#ifdef _WIN32
#define WINDOW_PLATFORM
#endif

/* GLSL着色器基类 */
class Shader
{
public:
	Shader() : m_Program(0), m_Name("Shader") {}
	Shader(std::string shaderName) :m_Program(0), m_Name(shaderName) {}

	~Shader() {}

public:
	void init();
	void attachByFile(int type, const char *filename);
	void attach(int type, const char* str);
	void link();
	void info();
	void use() const;
	void end() const;

	GLuint GetProgram() const;
	

private:
	char *textFileRead(const char *fn);

	GLuint m_Program;   //Shaderprogram
	std::string m_Name;   //Shaderclass name
};

#endif

