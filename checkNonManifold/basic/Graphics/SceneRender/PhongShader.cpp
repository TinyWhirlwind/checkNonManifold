//#include"stdafx.h"
#include"PhongShader.h"

#include <stdio.h>
#include <vector>

#include <gl/glew.h>
#include <gl/glu.h>

#include "Common/Point3.h"
//#pragma comment (lib,"glew32.lib")

PhongShader::PhongShader()
{

	GLchar  vs[] = "varying vec3 normal;\n\
                   varying vec3 vpos;\n\
	               void main()\n\
                   {\n\
				    normal = normalize(gl_NormalMatrix * gl_Normal);\n\
	                vpos = vec3(gl_ModelViewMatrix * gl_Vertex);\n\
	                gl_Position = ftransform();\n\
                   }\n\0";

	GLchar  fs[] = "varying vec3 normal;\n\
                     varying vec3 vpos;\n\
                     void main(){\n\
					 vec3 n = normalize(normal);\n\
	                 vec4 diffuse = vec4(0.0);\n\
	                 vec4 specular = vec4(0.0);\n\
	                 vec4 mat_ambient = vec4(0.7, 0.7, 0.7, 1.0);\n\
	                 vec4 mat_diffuse = vec4(0.9, 0.9, 0.9, 1.0);\n\
	                 vec4 mat_specular = vec4(0.05, 0.05, 0.05, 1.0);\n\
	                 vec4 ambient = mat_ambient * gl_LightSource[0].ambient;\n\
	                 vec4 kd = mat_diffuse * gl_LightSource[0].diffuse;\n\
	                 vec4 ks = mat_specular * gl_LightSource[0].specular;\n\
	                 vec3 lightDir = normalize(gl_LightSource[0].position.xyz - vpos);\n\
	                 float NdotL = dot(n, lightDir);\n\
	                 if (NdotL > 0.0)\n\
		               diffuse = kd * NdotL;\n\
	                 vec3 rVector = normalize(2.0 * n * dot(n, lightDir) - lightDir);\n\
	                 vec3 viewVector = normalize(-vpos);\n\
	                 float RdotV = dot(rVector, viewVector);\n\
	                 if (RdotV > 0.0)\n\
		             specular = ks * pow(RdotV, 0.01);\n\
					 gl_FragColor = ambient + diffuse + specular;\n\
                     }\n\0";


	m_vs = vs;
	m_fs = fs;
}
PhongShader::~PhongShader()
{

}
void PhongShader::attachVertexShader()
{
	GLuint shaderId = CompileGLSLShader(GL_VERTEX_SHADER, m_vs.c_str());

	_vertexShaders.push_back(shaderId);
}

void PhongShader::attachFragmentShader()
{
	GLuint shaderId = CompileGLSLShader(GL_FRAGMENT_SHADER, m_fs.c_str());

	_fragmentShaders.push_back(shaderId);
}
