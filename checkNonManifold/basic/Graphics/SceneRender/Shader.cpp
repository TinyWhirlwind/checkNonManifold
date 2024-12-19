#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

#include <QtCore/QDebug>
#include <QTextCodec>
//read the Shadercode
char * Shader::textFileRead(const char *fn) {  //read the shader code
	FILE *fp;
	char *content = NULL;
	int count = 0;

	if (fn != NULL) {

		fopen_s(&fp, fn, "rt");
		if (!fp) {

#ifdef WINDOW_PLATFORM
			//	MessageBox(NULL, "Can not open the shader file", "Error",  MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
			std::cerr << "Can not open the shader file:" << fn << std::endl;

#endif
			return NULL;
		}
		if (fp != NULL) {
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
		else
			std::cout << "Fail to open the shader file" << std::endl;
	}
	return content;
}



void Shader::init()
{
	m_Program = glCreateProgram();
}

void Shader::attachByFile(int type, const char *filename)
{
	//方法一流读取
	/*std::string shaderCode;
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{

		shaderFile.open(filename);
		std::stringstream shaderStream;

		shaderStream << shaderFile.rdbuf();

		shaderFile.close();

		shaderCode = shaderStream.str();;
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
		return;
	}
	const char* czShaderCode = shaderCode.c_str();*/
	
	//方法二读取文件
	/*QTextCodec *code = QTextCodec::codecForName("GB2312");
	std::string changPath = code->fromUnicode(filename).data();
	FILE *f = fopen(changPath.data(), "rb");
	if (!f)
	{
		QString qsMsg("读取文件二进制失败");
		return;
	}
	fseek(f, 0, SEEK_END);
	long sizeold = ftell(f);
	long size = ftell(f) + 1;
	char *czShaderCode = new char[size];
	fseek(f, 0L, SEEK_SET);
	fread(czShaderCode, sizeof(char), size, f);
	if (f)fclose(f);
	czShaderCode[sizeold] = '\0';*/
	//方法三
	char* czShaderCode = textFileRead(filename);
	qDebug("%s", czShaderCode);
	attach(type, czShaderCode);
	//delete[]czShaderCode;
}

void Shader::attach(int type, const char* str) //连接不同种类Shader
{
	GLuint handle = glCreateShader(type);
	glShaderSource(handle, 1, (const GLchar**)(&str), 0);

	glCompileShader(handle);

	GLint compileSuccess = 0;
	GLchar compilerSpew[256];

	glGetShaderiv(handle, GL_COMPILE_STATUS, &compileSuccess);

	if (!compileSuccess)
	{
		glGetShaderInfoLog(handle, sizeof(compilerSpew), 0, compilerSpew);
		qDebug("ycx Shader Compile Failure: %s", compilerSpew);

		while (1);;
	}
	glAttachShader(m_Program, handle);
	glDeleteShader(handle);
}

void Shader::link()
{
	glLinkProgram(m_Program);

	GLint linkSuccess;
	GLchar compilerSpew[256];
	glGetProgramiv(m_Program, GL_LINK_STATUS, &linkSuccess); //输出连接信息
	if (!linkSuccess)
	{
		glGetProgramInfoLog(m_Program, sizeof(compilerSpew), 0, compilerSpew);
		//printf("ShaderLinker:\n%s\nlinkSuccess=%d\n",compilerSpew,linkSuccess);
		qDebug("ycx Shader Link Failure: %s", compilerSpew);
		while (1);;
	}

	//std::cout << m_Name << " linked successful" << std::endl;
	//std::cout << "--------------------------------------------------------------------------------" << std::endl;
}

void Shader::info()
{
	std::cout << "------------------------------" << m_Name << " Interface-------------------------"
		<< std::endl;

	GLint outputs = 0;
	glGetProgramInterfaceiv(m_Program, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &outputs);
	static const GLenum props[] = { GL_TYPE, GL_LOCATION };
	GLint params[2];
	GLchar name[64];
	const char *type_name;

	if (outputs > 0)
		std::cout << "***Input***" << std::endl;

	for (int i = 0; i != outputs; ++i)
	{
		glGetProgramResourceName(m_Program, GL_PROGRAM_INPUT, i, sizeof(name), NULL, name);
		glGetProgramResourceiv(m_Program, GL_PROGRAM_INPUT, i, 2, props, 2, NULL, params);
		type_name = name;
		//std::cout << "Index " << i << std::endl;
		std::cout << "(" << type_name << ")" << " locatoin: " << params[1] << std::endl;
	}

	glGetProgramInterfaceiv(m_Program, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &outputs);
	if (outputs > 0)
		std::cout << "***Onput***" << std::endl;

	for (int i = 0; i != outputs; ++i)
	{
		glGetProgramResourceName(m_Program, GL_PROGRAM_OUTPUT, i, sizeof(name), NULL, name);
		glGetProgramResourceiv(m_Program, GL_PROGRAM_OUTPUT, i, 2, props, 2, NULL, params);

		type_name = name;
		//std::cout << "Index " << i << std::endl;
		std::cout << "(" << type_name << ")" << " locatoin: " << params[1] << std::endl;
	}

	glGetProgramInterfaceiv(m_Program, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &outputs);
	if (outputs > 0)
		std::cout << "***Uniform Block***" << std::endl;

	for (int i = 0; i != outputs; ++i)
	{
		glGetProgramResourceName(m_Program, GL_UNIFORM_BLOCK, i, sizeof(name), NULL, name);
		glGetProgramResourceiv(m_Program, GL_UNIFORM_BLOCK, i, 2, props, 2, NULL, params);

		type_name = name;
		//std::cout << "Index " << i << std::endl;
		std::cout << "(" << type_name << ")" << " locatoin: " << params[1] << std::endl;
	}

	glGetProgramInterfaceiv(m_Program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &outputs);
	if (outputs > 0)
		std::cout << "***Uniform***" << std::endl;
	if (outputs > 100)
		return;
	for (int i = 0; i != outputs; ++i)
	{
		glGetProgramResourceName(m_Program, GL_UNIFORM, i, sizeof(name), NULL, name);
		glGetProgramResourceiv(m_Program, GL_UNIFORM, i, 2, props, 2, NULL, params);

		type_name = name;
		//std::cout << "Index " << i << std::endl;
		std::cout << "(" << type_name << ")" << " locatoin: " << params[1] << std::endl;
	}
	std::cout << "--------------------------------------------------------------------------------" << std::endl;
}

void Shader::use() const
{
	glUseProgram(m_Program);
}
void Shader::end() const
{
	glUseProgram(0);
}

GLuint Shader::GetProgram() const
{
	return m_Program;
}

