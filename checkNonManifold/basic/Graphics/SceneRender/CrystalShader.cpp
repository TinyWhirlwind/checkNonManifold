#include"CrystalShader.h"

#include<gl/glew.h>

//#pragma comment (lib,"glew32.lib")

#include "GeoData/MeshData.h"
#include "GeoData/MeshTopology.h"

using namespace sn3DCore;
CrystalShader::CrystalShader()
{
	GLchar  vs[] = "varying vec3  Normal;\n\
                   varying vec3  EyeDir;\n\
                   varying vec4  EyePos;\n\
                   varying float LightIntensity;\n\
                   uniform vec3  LightPos;\n\
                   void main(void)\n\
                   {\n\
                   gl_Position    = ftransform();\n\
                   Normal         = normalize(gl_NormalMatrix * gl_Normal);\n\
                   vec4 pos       = gl_ModelViewMatrix * gl_Vertex;\n\
                   EyeDir         = pos.xyz;\n\
                   EyePos		  = gl_ModelViewProjectionMatrix * gl_Vertex;\n\
                   LightIntensity = max(dot(normalize(LightPos - EyeDir), Normal), 0.0);\n\
                   }\n\0";

	GLchar  fs[] = "const vec3 Xunitvec = vec3 (1.0, 0.0, 0.0);\n\
                     const vec3 Yunitvec = vec3 (0.0, 1.0, 0.0);\n\
                     uniform vec3  BaseColor;\n\
                     uniform float Depth;\n\
                     uniform float MixRatio;\n\
                     uniform float FrameWidth;\n\
                     uniform float FrameHeight;\n\
                     uniform sampler2D RefractionMap;\n\
                     varying vec3  Normal;\n\
                     varying vec3  EyeDir;\n\
                     varying vec4  EyePos;\n\
                     varying float LightIntensity;\n\
                     void main (void)\n\
                     { \n\
                      vec3 reflectDir = reflect(EyeDir, Normal);\n\
                      vec2 index;\n\
                      index.y = dot(normalize(reflectDir), Yunitvec);\n\
                      reflectDir.y = 0.0;\n\
                      index.x = dot(normalize(reflectDir), Xunitvec) * 0.5;\n\
                      if (reflectDir.z >= 0.0)\n\
                      index = (index + 1.0) * 0.5;\n\
                      else\n\
                      {\n\
                       index.t = (index.t + 1.0) * 0.5;\n\
                       index.s = (-index.s) * 0.5 + 1.0;\n\
                      }\n\
                      vec3 envColor = vec3(0.0,0.0,0.0);\n\
                      float fresnel = abs(dot(normalize(EyeDir), Normal));\n\
                      fresnel *= MixRatio;\n\
                      fresnel = clamp(fresnel, 0.1, 0.9);\n\
	                  vec3 refractionDir = normalize(EyeDir) - normalize(Normal);\n\
	                  float depthVal = Depth / -refractionDir.z;\n\
	                  float recipW = 1.0 / EyePos.w;\n\
	                  vec2 eye = EyePos.xy * vec2(recipW);\n\
	                  index.s = (eye.x + refractionDir.x * depthVal);\n\
	                  index.t = (eye.y + refractionDir.y * depthVal);\n\
	                  index.s = index.s / 2.0 + 0.5;\n\
	                  index.t = index.t / 2.0 + 0.5;\n\
	                  float recip1k = 1.0 / 2048.0;\n\
	                  index.s = clamp(index.s, 0.0, 1.0 - recip1k);\n\
 	                  index.t = clamp(index.t, 0.0, 1.0 - recip1k);\n\
	                  index.s = index.s * FrameWidth * recip1k;\n\
	                  index.t = index.t * FrameHeight * recip1k;\n\
                      vec3 RefractionColor = vec3 (texture2D(RefractionMap, index));\n\
					  vec3 base = (LightIntensity*1.5) * BaseColor;\n\
                      envColor = mix(envColor, RefractionColor, fresnel);\n\
                      envColor = mix(envColor, base, 0.3);\n\
                      gl_FragColor = vec4 (envColor, 1.0);\n\
                      }\n\0";

	m_vs = vs;
	m_fs = fs;
	m_init = false;
}
void CrystalShader::InitShader()
{
	if (m_init)return;
	m_init = true;
	GLint vertCompiled, fragCompiled;
	GLint linked;

	const char* vs_pointer = m_vs.c_str();
	const char* fs_pointer = m_fs.c_str();

	glewInit();

	m_crystalVS = glCreateShader(GL_VERTEX_SHADER);
	m_crystalFS = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(m_crystalVS, 1, &vs_pointer, NULL);
	glShaderSource(m_crystalFS, 1, &fs_pointer, NULL);

	glCompileShader(m_crystalVS);
	glGetShaderiv(m_crystalVS, GL_COMPILE_STATUS, &vertCompiled);

	glCompileShader(m_crystalFS);
	glGetShaderiv(m_crystalFS, GL_COMPILE_STATUS, &fragCompiled);


	if (!vertCompiled || !fragCompiled)
	{
		return;
	}

	m_crystalProg = glCreateProgram();
	glAttachShader(m_crystalProg, m_crystalVS);
	glAttachShader(m_crystalProg, m_crystalFS);

	glLinkProgram(m_crystalProg);
	glGetProgramiv(m_crystalProg, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		return;
	}

	//m_crystalList = glGenLists(1);

	int res = 5;

	Point3d corners[8];
	corners[0] = Point3d(-1.0, -1.0, 1.0);
	corners[1] = Point3d(1.0, -1.0, 1.0);
	corners[2] = Point3d(1.0, 1.0, 1.0);
	corners[3] = Point3d(-1.0, 1.0, 1.0);
	corners[4] = Point3d(-1.0, -1.0, -1.0);
	corners[5] = Point3d(1.0, -1.0, -1.0);
	corners[6] = Point3d(1.0, 1.0, -1.0);
	corners[7] = Point3d(-1.0, 1.0, -1.0);


	m_data.reset(new sn3DMeshData);
	boost::shared_ptr<sn3DMeshData> data = m_data;

	Point3d p0, p1, p2, p3;
	int v0, v1, v2;
	Point3d dir0;
	Point3d dir1;

	for (int f = 0; f < 6; f++)
	{
		switch (f)
		{
		case 0:
			p0 = corners[0];
			p1 = corners[1];
			p2 = corners[2];
			p3 = corners[3];
			break;
		case 1:
			p0 = corners[1];
			p1 = corners[5];
			p2 = corners[6];
			p3 = corners[2];
			break;
		case 2:
			p0 = corners[5];
			p1 = corners[4];
			p2 = corners[7];
			p3 = corners[6];
			break;
		case 3:
			p0 = corners[0];
			p1 = corners[3];
			p2 = corners[7];
			p3 = corners[4];
			break;
		case 4:
			p0 = corners[3];
			p1 = corners[2];
			p2 = corners[6];
			p3 = corners[7];
			break;
		case 5:
			p0 = corners[0];
			p1 = corners[4];
			p2 = corners[5];
			p3 = corners[1];
			break;
		}

		double step0 = (p1 - p0).Norm() / res;
		double step1 = (p2 - p1).Norm() / res;

		dir0 = (p1 - p0).Normalize();
		dir1 = (p2 - p1).Normalize();
		for (int i = 0; i < res; i++)
			for (int j = 0; j < res; j++)
			{
				Point3d point0 = p0 + dir0*i*step0 + dir1*j*step1;
				Point3d point1 = p0 + dir0*(i + 1)*step0 + dir1*j*step1;
				Point3d point2 = p0 + dir0*(i + 1)*step0 + dir1*(j + 1)*step1;
				Point3d point3 = p0 + dir0*i*step0 + dir1*(j + 1)*step1;
				int vn;
				vn = data->vn;
				v0 = vn;
				v1 = vn + 1;
				v2 = vn + 2;
				data->AddVertex(Point3f(point0[0], point0[1], point0[2]));
				data->AddVertex(Point3f(point1[0], point1[1], point1[2]));
				data->AddVertex(Point3f(point3[0], point3[1], point3[2]));
				data->AddFace(v0, v1, v2);
				vn = data->vn;
				v0 = vn;
				v1 = vn + 1;
				v2 = vn + 2;
				data->AddVertex(Point3f(point1[0], point1[1], point1[2]));
				data->AddVertex(Point3f(point2[0], point2[1], point2[2]));
				data->AddVertex(Point3f(point3[0], point3[1], point3[2]));
				data->AddFace(v0, v1, v2);
			}

	}

	Topology::RemoveRedundant(data.get());
	Topology::VertexFace(*data);
	data->UpdateNormal();

	//int fn = (int)data->fn;
	//glNewList(m_crystalList, GL_COMPILE);
	//glBegin(GL_TRIANGLES);

	//for(int i=0;i<fn;i++)
	//{
	// glNormal3f(data->F(i)->N().X(),data->F(i)->N().Y(),data->F(i)->N().Z());
 //    glVertex3f(data->F(i)->P(0).X(),data->F(i)->P(0).Y(),data->F(i)->P(0).Z());
	// glVertex3f(data->F(i)->P(1).X(),data->F(i)->P(1).Y(),data->F(i)->P(1).Z());
	// glVertex3f(data->F(i)->P(2).X(),data->F(i)->P(2).Y(),data->F(i)->P(2).Z());
	//}

	//glEnd();
	//glEndList();
	//model.Clear();
}
void CrystalShader::RenderCrystal(Point3f minp, Point3f maxp)
{
	InitShader();

	float xs, ys, zs;
	xs = maxp[0] - minp[0];
	ys = maxp[1] - minp[1];
	zs = maxp[2] - minp[2];

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	BeginShader(viewport[2], viewport[3]);
	glPushMatrix();
	glScalef(xs / 2.0, ys / 2.0, zs / 2.0);

	boost::shared_ptr<sn3DMeshData> data = m_data;
	int fn = (int)data->fn;

	glBegin(GL_TRIANGLES);
	for (int i = 0; i < fn; i++)
	{
		glNormal3f(data->F(i)->N().X(), data->F(i)->N().Y(), data->F(i)->N().Z());
		glVertex3f(data->F(i)->P(0).X(), data->F(i)->P(0).Y(), data->F(i)->P(0).Z());
		glVertex3f(data->F(i)->P(1).X(), data->F(i)->P(1).Y(), data->F(i)->P(1).Z());
		glVertex3f(data->F(i)->P(2).X(), data->F(i)->P(2).Y(), data->F(i)->P(2).Z());
	}

	glEnd();
	glPopMatrix();
	EndShader();

}
void CrystalShader::BeginShader(int w, int h)
{
	glUseProgram(m_crystalProg);

	static int tt = 0;
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	const int size = 2048;
	glActiveTexture(GL_TEXTURE0);
	if (!glIsTexture(m_texture))
	{
		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, size, size, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, w, h);
	}

	glDisable(GL_TEXTURE_2D);


	glUniform1i(glGetUniformLocation(m_crystalProg, "RefractionMap"), 0);
	float lightPos[3];
	lightPos[0] = 0.0;
	lightPos[1] = 0.0;
	lightPos[2] = 4.0;
	glUniform3fv(glGetUniformLocation(m_crystalProg, "lightPos"), 1, lightPos);
	float baseColor[3];
	baseColor[0] = 0.3;
	baseColor[1] = 0.3;
	baseColor[2] = 0.3;
	glUniform3fv(glGetUniformLocation(m_crystalProg, "BaseColor"), 1, baseColor);
	glUniform1f(glGetUniformLocation(m_crystalProg, "Depth"), 0.1);
	glUniform1f(glGetUniformLocation(m_crystalProg, "MixRatio"), 1.0);
	glUniform1f(glGetUniformLocation(m_crystalProg, "FrameWidth"), (float)w);
	glUniform1f(glGetUniformLocation(m_crystalProg, "FrameHeight"), (float)h);

}
void CrystalShader::EndShader()
{
	glUseProgram(0);
}