#include "stdafx.h"
#include "sn3DNewMeshRender.h"
#include "QApplication.h"
#include "SceneRender/Shader.h"
#include <QTextCodec>
#include "GeoData/MeshData.h"
#include <gl/glew.h>

using namespace sn3DCore;

RenderOptData::RenderOptData()
{
	elementbuffer = 0xffffffff;
	vertexArray = 0xffffffff;
	vertexBuffer = 0xffffffff;
	veclocation.clear();
	rangeShader = 0xffffffff;
	isbuiledShader = false;
	shadetype = EN_SHADETYPE_NO;
}

bool RenderOptData::DeleteRangerOpt()
{
	if (!isbuiledShader)return false;
	if (glIsVertexArray(vertexArray))
	{
		glDeleteVertexArrays(1, &vertexArray);
		vertexArray = 0xffffffff;
	}
	if (glIsBuffer(vertexBuffer))
	{
		glDeleteBuffers(1, &vertexBuffer);
		vertexBuffer = 0xffffffff;
	}
	if (glIsBuffer(elementbuffer))
	{
		glDeleteBuffers(1, &elementbuffer);
		elementbuffer = 0xffffffff;
	}
	return true;
}

bool RenderOptData::BindRangerOpt()
{
	if (!isbuiledShader)return false;
	glUseProgram(rangeShader);
	glBindVertexArray(vertexArray);
	for (int i = 0; i < veclocation.size(); i++)
	{
		glEnableVertexAttribArray(veclocation[i]);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	return true;
}

bool RenderOptData::DisableRangerOpt()
{
	if (!isbuiledShader) return false;
	for (int i = 0; i < veclocation.size(); i++)
	{
		glDisableVertexAttribArray(veclocation[i]);
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);//不加这句会导致qpaint和opengl得切换绘制时会崩溃
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
	return true;
}

ShadeAllFileLoadData &ShadeAllFileLoadData::getInstance()
{
	static ShadeAllFileLoadData g_pShadeAllFileLoadData;
	return g_pShadeAllFileLoadData;
}

unsigned int ShadeAllFileLoadData::GetShaderProgram(emShadeType type)
{
	int shadetype = type;
	if (m_mapShaderProgram.find(shadetype) != m_mapShaderProgram.end())return m_mapShaderProgram[shadetype];
	//编译链接
	QString qsVData = GetVShadefileData(type);
	QString qsFData = GetFShadefileData(type);

	Shader shader;
	shader.init();
	shader.attach(GL_VERTEX_SHADER, qsVData.toStdString().c_str());
	shader.attach(GL_FRAGMENT_SHADER, qsFData.toStdString().c_str());
	shader.link();
	m_mapShaderProgram[shadetype] = shader.GetProgram();
	return m_mapShaderProgram[shadetype];
	
}

QString ShadeAllFileLoadData::GetVShadefileData(emShadeType type)
{
	int tmptype = type;
	if (m_mapVShadefileData.find(tmptype) != m_mapVShadefileData.end())return m_mapVShadefileData[tmptype];
	QString temporaryDirPath = QApplication::applicationDirPath() + "/" + "Resources/Shade/";
	QString vpath("");
	switch (type)
	{
	case sn3DCore::EN_SHADETYPE_COM:
		vpath = temporaryDirPath + GetShadeTypeComVSFileName();
		break;
	case sn3DCore::EN_SHADETYPE_VCOLOR:
		vpath = temporaryDirPath + GetShadeTypeVColorVSFileName();
		break;
	default:
		return "";
	}
	//读取文件，且存储
	QString data("");
	if(!ReadFileToString(vpath, data))return "";
	m_mapVShadefileData[tmptype] = data;
	return m_mapVShadefileData[tmptype];
}

QString ShadeAllFileLoadData::GetFShadefileData(emShadeType type)
{
	int tmptype = type;
	if (m_mapFShadefileData.find(tmptype) != m_mapFShadefileData.end())return m_mapFShadefileData[tmptype];
	QString temporaryDirPath = QApplication::applicationDirPath() + "/" + "Resources/Shade/";
	QString fpath("");
	switch (type)
	{
	case sn3DCore::EN_SHADETYPE_COM:
		fpath = temporaryDirPath + GetShadeTypeComFSFileName();
		break;
	case sn3DCore::EN_SHADETYPE_VCOLOR:
		fpath = temporaryDirPath + GetShadeTypeComFSFileName();
		break;
	default:
		return "";
	}
	//读取文件，且存储
	QString data("");
	if (!ReadFileToString(fpath, data))return "";
	m_mapFShadefileData[tmptype] = data;
	return m_mapFShadefileData[tmptype];
}

bool ShadeAllFileLoadData::ReadFileToString(QString filePath, QString &data)
{
	QTextCodec *code = QTextCodec::codecForName("GB2312");
	std::string changPath = code->fromUnicode(filePath).data();
	FILE *f = fopen(changPath.data(), "rb");
	if (!f)
	{
		QString qsMsg("读取文件二进制失败");
		return false;
	}
	fseek(f, 0, SEEK_END);
	long sizeold = ftell(f);
	long size = ftell(f) + 1;
	char *czShaderCode = new char[size];
	fseek(f, 0L, SEEK_SET);
	fread(czShaderCode, sizeof(char), size, f);
	if (f)fclose(f);
	czShaderCode[sizeold] = '\0';
	data.clear();
	data.append(czShaderCode);
	delete []czShaderCode;
	return true;
}

QString  ShadeAllFileLoadData::GetShadeTypeComVSFileName()
{
	QString qsname("comShade.vs");
	return qsname;
}
QString  ShadeAllFileLoadData::GetShadeTypeComFSFileName()
{
	QString qsname("comShade.fs");
	return qsname;
}
QString  ShadeAllFileLoadData::GetShadeTypeVColorVSFileName()
{
	QString qsname("vcolorShade.vs");
	return qsname;
}

//-----------------------------------------------------------
sn3DNewMeshRender::sn3DNewMeshRender():sn3DMeshRenderer()
{
	m_curId = emShadeType::EN_SHADETYPE_NO;
}


sn3DNewMeshRender::~sn3DNewMeshRender()
{
}
void sn3DNewMeshRender::AddReaderShade(emShadeType shadeType, bool setCurType)
{
	int type = shadeType;
	if (m_mapRenderData.find(type) == m_mapRenderData.end())
	{
		boost::shared_ptr<sn3DCore::RenderOptData> opt(new RenderOptData);
		m_mapRenderData[type] = opt;
		CompileRangeShader(shadeType, m_mapRenderData[type]);
	}
	if (setCurType) m_curId = shadeType;
}

bool sn3DNewMeshRender::SwitchReaderShade(emShadeType shadeType)
{
	int type = shadeType;
	if (m_mapRenderData.find(type) == m_mapRenderData.end())return false;
	m_curId = shadeType;
	return true;
}
//重写
void sn3DNewMeshRender::BuildRangeData()
{
	switch (m_curId)
	{
	case EN_SHADETYPE_COM:
		BuildRangeData_Com();
		break;
	case EN_SHADETYPE_VCOLOR:
		BuildRangeData_VColor();
		break;
	}
}

void sn3DNewMeshRender::DrawRangeDataFill()
{
	boost::shared_ptr<sn3DMeshData> data = boost::dynamic_pointer_cast<sn3DMeshData>(m_data);
	if (!data)return;
	int curidtype = m_curId;
	if (m_mapRenderData.find(curidtype) == m_mapRenderData.end())return;
	if (!m_mapRenderData[curidtype]->isbuiledShader)return;
	//绑定
	m_mapRenderData[curidtype]->BindRangerOpt();
	// 设置Uniform参数值
	switch (m_curId)
	{
	case EN_SHADETYPE_COM:
		SetRangeUniform_com();
		break;
	case EN_SHADETYPE_VCOLOR:
		SetRangeUniform_vcolor();
		break;
	}
	//绘画
	glDrawElements(GL_TRIANGLES, data->fn * 3, GL_UNSIGNED_INT, 0);
	//解绑
	m_mapRenderData[curidtype]->DisableRangerOpt();
	
}

void sn3DNewMeshRender::BuildRangeData_Com()
{
	boost::shared_ptr<sn3DMeshData> data = boost::dynamic_pointer_cast<sn3DMeshData>(m_data);
	if (!data)return;
	int curidtype = m_curId;
	if (m_curId != EN_SHADETYPE_COM)return;
	if (m_mapRenderData.find(curidtype) == m_mapRenderData.end())return;
	if (!m_mapRenderData[curidtype]->isbuiledShader)return;
	//--
	m_mapRenderData[curidtype]->DeleteRangerOpt();
	//--赋值
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	int strip;	//GLfloat的strip

	if (SMFlat == m_shadeMode)
	{
		int vn = data->m_vertices.size();
		int fn = data->m_faces.size();

		//strip = 3 * 2;
		strip = 3 * 3;
		vertices.resize(fn * 3 * strip);

		for (int i = 0; i < fn; i++)
		{
			Face* f = data->F(i);
			for (int j = 0; j < 3; ++j)
			{
				Point3f p = f->V(j)->P();
				Point3f n = f->N();
				Color4f color(m_color[0], m_color[1], m_color[2], m_color[3]);
				if (f->V(j)->Index() < data->m_vertexColor.size())
					color = data->m_vertexColor[f->V(j)->Index()];

				vertices[(i * 3 + j) * strip] = p[0];
				vertices[(i * 3 + j) * strip + 1] = p[1];
				vertices[(i * 3 + j) * strip + 2] = p[2];

				vertices[(i * 3 + j) * strip + 3] = n[0];
				vertices[(i * 3 + j) * strip + 4] = n[1];
				vertices[(i * 3 + j) * strip + 5] = n[2];

				vertices[(i * 3 + j) * strip + 6] = color[0];
				vertices[(i * 3 + j) * strip + 7] = color[1];
				vertices[(i * 3 + j) * strip + 8] = color[2];
			}
		}
		indices.resize(fn * 3);
		for (int i = 0; i < fn; i++)
		{
			for (int j = 0; j < 3; ++j)
			{
				indices[i * 3 + j] = i * 3 + j;
				indices[i * 3 + j] = i * 3 + j;
				indices[i * 3 + j] = i * 3 + j;
			}
		}
	}
	else if (SMNormalSplit == m_shadeMode)
	{
		int vn = data->m_vertices.size();
		int fn = data->m_faces.size();

		//strip = 3 * 2;
		strip = 3 * 3;
		vertices.resize(fn * 3 * strip);

		for (int i = 0; i < fn; i++)
		{
			Face* f = data->F(i);
			for (int j = 0; j < 3; ++j)
			{
				Point3f p = f->V(j)->P();
				Point3f n = f->V(j)->N();
				Color4f color(m_color[0], m_color[1], m_color[2], m_color[3]);
				if (f->V(j)->Index() < data->m_vertexColor.size())color = data->m_vertexColor[f->V(j)->Index()];

				vertices[(i * 3 + j) * strip] = p[0];
				vertices[(i * 3 + j) * strip + 1] = p[1];
				vertices[(i * 3 + j) * strip + 2] = p[2];

				vertices[(i * 3 + j) * strip + 3] = n[0];
				vertices[(i * 3 + j) * strip + 4] = n[1];
				vertices[(i * 3 + j) * strip + 5] = n[2];

				vertices[(i * 3 + j) * strip + 6] = color[0];
				vertices[(i * 3 + j) * strip + 7] = color[1];
				vertices[(i * 3 + j) * strip + 8] = color[2];
			}
		}
		indices.resize(fn * 3);
		for (int i = 0; i < fn; i++)
		{
			for (int j = 0; j < 3; ++j)
			{
				indices[i * 3 + j] = i * 3 + j;
				indices[i * 3 + j] = i * 3 + j;
				indices[i * 3 + j] = i * 3 + j;
			}
		}
	}
	else //SMSmooth
	{
		int vn = data->m_vertices.size();
		int fn = data->m_faces.size();

		//strip = 3 * 2;
		strip = 3 * 3;
		vertices.resize(vn * strip);

		for (int i = 0; i < vn; i++)
		{
			Point3f p = data->V(i)->P();
			Point3f n = data->V(i)->N();
			Color4f color(m_color[0], m_color[1], m_color[2], m_color[3]);
			if (data->V(i)->Index() < data->m_vertexColor.size())color = data->m_vertexColor[data->V(i)->Index()];

			vertices[i * strip] = p[0];
			vertices[i * strip + 1] = p[1];
			vertices[i * strip + 2] = p[2];

			vertices[i * strip + 3] = n[0];
			vertices[i * strip + 4] = n[1];
			vertices[i * strip + 5] = n[2];

			vertices[i * strip + 6] = color[0];
			vertices[i * strip + 7] = color[1];
			vertices[i * strip + 8] = color[2];
		}
		indices.resize(fn * 3);
		for (int i = 0; i < fn; i++)
		{
			int v0 = data->F(i)->V(0)->Index();
			int v1 = data->F(i)->V(1)->Index();
			int v2 = data->F(i)->V(2)->Index();

			indices[i * 3] = v0;
			indices[i * 3 + 1] = v1;
			indices[i * 3 + 2] = v2;
		}
	}
	//赋值
	glGenBuffers(1, &m_mapRenderData[curidtype]->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_mapRenderData[curidtype]->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &m_mapRenderData[curidtype]->elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mapRenderData[curidtype]->elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	//================= Vertex Attributes ===============
	glGenVertexArrays(1, &m_mapRenderData[curidtype]->vertexArray);
	glBindVertexArray(m_mapRenderData[curidtype]->vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_mapRenderData[curidtype]->vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * strip, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * strip, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * strip, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mapRenderData[curidtype]->elementbuffer);
	m_mapRenderData[curidtype]->veclocation.clear();
	for (int i=0; i<3; i++)
	{
		m_mapRenderData[curidtype]->veclocation.push_back(i);
	}
	//移除
	m_mapRenderData[curidtype]->DisableRangerOpt();
}

void sn3DNewMeshRender::BuildRangeData_VColor()
{
	boost::shared_ptr<sn3DMeshData> data = boost::dynamic_pointer_cast<sn3DMeshData>(m_data);
	if (!data)return;
	int curidtype = m_curId;
	if (m_curId != EN_SHADETYPE_VCOLOR)return;
	if (m_mapRenderData.find(curidtype) == m_mapRenderData.end())return;
	if (!m_mapRenderData[curidtype]->isbuiledShader)return;
	//--
	m_mapRenderData[curidtype]->DeleteRangerOpt();
	//--赋值
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	int strip;	//GLfloat的strip

	if (SMFlat == m_shadeMode)
	{
		int vn = data->m_vertices.size();
		int fn = data->m_faces.size();

		//strip = 3 * 2;
		strip = 3 * 3;
		vertices.resize(fn * 3 * strip);

		for (int i = 0; i < fn; i++)
		{
			Face* f = data->F(i);
			for (int j = 0; j < 3; ++j)
			{
				Point3f p = f->V(j)->P();
				Point3f n = f->N();
				Color4f color =  f->V(j)->C();
				if (f->V(j)->Index() < data->m_vertexColor.size())
					color = data->m_vertexColor[f->V(j)->Index()];

				vertices[(i * 3 + j) * strip] = p[0];
				vertices[(i * 3 + j) * strip + 1] = p[1];
				vertices[(i * 3 + j) * strip + 2] = p[2];

				vertices[(i * 3 + j) * strip + 3] = n[0];
				vertices[(i * 3 + j) * strip + 4] = n[1];
				vertices[(i * 3 + j) * strip + 5] = n[2];

				vertices[(i * 3 + j) * strip + 6] = color[0];
				vertices[(i * 3 + j) * strip + 7] = color[1];
				vertices[(i * 3 + j) * strip + 8] = color[2];
			}
		}
		indices.resize(fn * 3);
		for (int i = 0; i < fn; i++)
		{
			for (int j = 0; j < 3; ++j)
			{
				indices[i * 3 + j] = i * 3 + j;
				indices[i * 3 + j] = i * 3 + j;
				indices[i * 3 + j] = i * 3 + j;
			}
		}
	}
	else if (SMNormalSplit == m_shadeMode)
	{
		int vn = data->m_vertices.size();
		int fn = data->m_faces.size();

		//strip = 3 * 2;
		strip = 3 * 3;
		vertices.resize(fn * 3 * strip);

		for (int i = 0; i < fn; i++)
		{
			Face* f = data->F(i);
			for (int j = 0; j < 3; ++j)
			{
				Point3f p = f->V(j)->P();
				Point3f n = f->V(j)->N();
				Color4f color = f->V(j)->C();
				if (f->V(j)->Index() < data->m_vertexColor.size())
					color = data->m_vertexColor[f->V(j)->Index()];

				vertices[(i * 3 + j) * strip] = p[0];
				vertices[(i * 3 + j) * strip + 1] = p[1];
				vertices[(i * 3 + j) * strip + 2] = p[2];

				vertices[(i * 3 + j) * strip + 3] = n[0];
				vertices[(i * 3 + j) * strip + 4] = n[1];
				vertices[(i * 3 + j) * strip + 5] = n[2];

				vertices[(i * 3 + j) * strip + 6] = color[0];
				vertices[(i * 3 + j) * strip + 7] = color[1];
				vertices[(i * 3 + j) * strip + 8] = color[2];
			}
		}
		indices.resize(fn * 3);
		for (int i = 0; i < fn; i++)
		{
			for (int j = 0; j < 3; ++j)
			{
				indices[i * 3 + j] = i * 3 + j;
				indices[i * 3 + j] = i * 3 + j;
				indices[i * 3 + j] = i * 3 + j;
			}
		}
	}
	else //SMSmooth
	{
		int vn = data->m_vertices.size();
		int fn = data->m_faces.size();

		//strip = 3 * 2;
		strip = 3 * 3;
		vertices.resize(vn * strip);

		for (int i = 0; i < vn; i++)
		{
			Point3f p = data->V(i)->P();
			Point3f n = data->V(i)->N();
			Color4f color =  data->V(i)->C();
			if (data->V(i)->Index() < data->m_vertexColor.size())
				color = data->m_vertexColor[data->V(i)->Index()];

			vertices[i * strip] = p[0];
			vertices[i * strip + 1] = p[1];
			vertices[i * strip + 2] = p[2];

			vertices[i * strip + 3] = n[0];
			vertices[i * strip + 4] = n[1];
			vertices[i * strip + 5] = n[2];

			vertices[i * strip + 6] = color[0];
			vertices[i * strip + 7] = color[1];
			vertices[i * strip + 8] = color[2];
		}
		indices.resize(fn * 3);
		for (int i = 0; i < fn; i++)
		{
			int v0 = data->F(i)->V(0)->Index();
			int v1 = data->F(i)->V(1)->Index();
			int v2 = data->F(i)->V(2)->Index();

			indices[i * 3] = v0;
			indices[i * 3 + 1] = v1;
			indices[i * 3 + 2] = v2;
		}
	}
	//赋值
	glGenBuffers(1, &m_mapRenderData[curidtype]->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_mapRenderData[curidtype]->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &m_mapRenderData[curidtype]->elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mapRenderData[curidtype]->elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	//================= Vertex Attributes ===============
	glGenVertexArrays(1, &m_mapRenderData[curidtype]->vertexArray);
	glBindVertexArray(m_mapRenderData[curidtype]->vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, m_mapRenderData[curidtype]->vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * strip, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * strip, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * strip, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);
	m_mapRenderData[curidtype]->veclocation.clear();
	for (int i = 0; i<3; i++)
	{
		m_mapRenderData[curidtype]->veclocation.push_back(i);
	}
	//移除
	m_mapRenderData[curidtype]->DisableRangerOpt();

}

void sn3DNewMeshRender::SetRangeUniform_com()
{
	int curidtype = m_curId;
	if (m_mapRenderData.find(curidtype) == m_mapRenderData.end())return;
	if (!m_mapRenderData[curidtype]->isbuiledShader)return;

	float m_MvMatrix[16];
	float m_ProjMatrix[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, m_MvMatrix);
	glGetFloatv(GL_PROJECTION_MATRIX, m_ProjMatrix);

	Matrix44f mvMat, projMat, mat;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			mvMat[i][j] = m_MvMatrix[i * 4 + j];
			projMat[i][j] = m_ProjMatrix[i * 4 + j];

			//mat[i][j] = m_mat[i * 4 + j];
		}
	//mvMat = mat * mvMat;

	Matrix33f normalMat;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			normalMat[i][j] = mvMat[i][j];

	Matrix44f mvpMat = projMat * mvMat;

	Point3f lightDiffuse;
	Point3f lightSpecular;
	Point3f lightAmbient;
	float lightPos[3];

	glGetLightfv(GL_LIGHT0, GL_AMBIENT, &lightAmbient[0]);
	glGetLightfv(GL_LIGHT0, GL_DIFFUSE, &lightDiffuse[0]);
	glGetLightfv(GL_LIGHT0, GL_SPECULAR, &lightSpecular[0]);
	glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	Point3f materialDiffuse[2];
	Point3f materialSpecular[2];
	Point3f materialAmbient[2];
	//float shininess = 128.f;
	//float shininess = 20.f;
	float shininess[2] = {20.0f, 20.0f};
	bool isEVcolor = m_colorMode == CMPerVertex? true: m_bColorType;
	//正反面
	for (int i = 0; i<2; i++)
	{
		materialAmbient[i] = m_color;
		materialDiffuse[i] = m_color;
		materialSpecular[i] = Point3f(0.0, 0.0, 0.0);
		//materialSpecular[i] = Point3f(0.5, 0.5, 0.5);
	}
	if (m_colorMode == CMMaterial && !m_material.empty())
	{
		
		/*std::map<MaterialFace, MaterialInfo>::iterator iter = m_material.find(MF_FRONT);
		if (iter == m_material.end())
		iter = m_material.find(MF_FRONT_AND_BACK);
		materialAmbient = Point3f(iter->second.ambient[0], iter->second.ambient[1], iter->second.ambient[2]);
		materialDiffuse = Point3f(iter->second.diffuse[0], iter->second.diffuse[1], iter->second.diffuse[2]);
		materialSpecular = Point3f(iter->second.specular[0], iter->second.specular[1], iter->second.specular[2]);*/
		std::map<MaterialFace, MaterialInfo>::iterator iter = m_material.find(MF_FRONT);
		std::map<MaterialFace, MaterialInfo>::iterator iterBack = m_material.find(MF_BACK);
		std::map<MaterialFace, MaterialInfo>::iterator iterOr = m_material.find(MF_FRONT_AND_BACK);
		if (iter == m_material.end() && iterBack == m_material.end() && iterOr != m_material.end())
		{
			//正反面
			for (int i = 0; i<2; i++)
			{
				materialAmbient[i] = Point3f(iterOr->second.ambient[0], iterOr->second.ambient[1], iterOr->second.ambient[2]);
				materialDiffuse[i] = Point3f(iterOr->second.diffuse[0], iterOr->second.diffuse[1], iterOr->second.diffuse[2]);
				materialSpecular[i] = Point3f(iterOr->second.specular[0], iterOr->second.specular[1], iterOr->second.specular[2]);
				shininess[i] = iterOr->second.shininess;
			}
		}
		else if (iter != m_material.end() && iterBack == m_material.end())
		{
			//正反面
			for (int i = 0; i<2; i++)
			{
				materialAmbient[i] = Point3f(iter->second.ambient[0], iter->second.ambient[1], iter->second.ambient[2]);
				materialDiffuse[i] = Point3f(iter->second.diffuse[0], iter->second.diffuse[1], iter->second.diffuse[2]);
				materialSpecular[i] = Point3f(iter->second.specular[0], iter->second.specular[1], iter->second.specular[2]);
				shininess[i] = iter->second.shininess;
			}
		}
		else if (iter == m_material.end() && iterBack != m_material.end())
		{
			//正反面
			for (int i = 0; i<2; i++)
			{
				materialAmbient[i] = Point3f(iterBack->second.ambient[0], iterBack->second.ambient[1], iterBack->second.ambient[2]);
				materialDiffuse[i] = Point3f(iterBack->second.diffuse[0], iterBack->second.diffuse[1], iterBack->second.diffuse[2]);
				materialSpecular[i] = Point3f(iterBack->second.specular[0], iterBack->second.specular[1], iterBack->second.specular[2]);
				shininess[i] = iterBack->second.shininess;
			}
		}
		else if (iter != m_material.end() && iterBack != m_material.end())
		{
			//正面
			materialAmbient[0] = Point3f(iter->second.ambient[0], iter->second.ambient[1], iter->second.ambient[2]);
			materialDiffuse[0] = Point3f(iter->second.diffuse[0], iter->second.diffuse[1], iter->second.diffuse[2]);
			materialSpecular[0] = Point3f(iter->second.specular[0], iter->second.specular[1], iter->second.specular[2]);
			shininess[0] = iter->second.shininess;
			//反面
			materialAmbient[1] = Point3f(iterBack->second.ambient[0], iterBack->second.ambient[1], iterBack->second.ambient[2]);
			materialDiffuse[1] = Point3f(iterBack->second.diffuse[0], iterBack->second.diffuse[1], iterBack->second.diffuse[2]);
			materialSpecular[1] = Point3f(iterBack->second.specular[0], iterBack->second.specular[1], iterBack->second.specular[2]);
			shininess[1] = iterBack->second.shininess;
		}
		isEVcolor = m_bColorType;
		//--------
		if (shininess[0] >= 0.0001 && shininess[1] >= 0.0001)
		{
			for (int i = 0; i < 3; i++)
			{
				lightAmbient[i] = 0.1f;
				lightDiffuse[i] = 1.0f;
				lightSpecular[i] = 1.0f;
			}
		}
		else
		{
			shininess[0] = 20;
			shininess[1] = 20;
		}
		//---------
	}


	int g_uniformLightDiffuse = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Light.Ld");
	int g_uniformLightAmbient = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Light.La");
	int g_uniformLightSpecular = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Light.Ls");
	int g_uniformLightPosition = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Light.Position");

	//正面材质
	int g_uniformMaterialDiffuse = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Material.Kd");
	int g_uniformMaterialAmbient = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Material.Ka");
	int g_uniformMaterialSpecular = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Material.Ks");
	int g_uniformMaterialShininess = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Material.Shininess");
	//背面材质
	int g_uniformMaterialBackDiffuse = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "MaterialBack.Kd");
	int g_uniformMaterialBackAmbient = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "MaterialBack.Ka");
	int g_uniformMaterialBackSpecular = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "MaterialBack.Ks");
	int g_uniformMaterialBackShininess = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "MaterialBack.Shininess");

	int g_uniformMVMat = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "ModelViewMatrix");
	int g_uniformNormalMat = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "NormalMatrix");
	int g_uniformProjMat = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "ProjectionMatrix");

	//透明通道
	int g_uniformAlpha = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Alpha");
	//是否用顶点颜色
	int g_uniformIsEVcolor = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "EnableVColor");

	//////////////////////////////////////////////////////////////
	glUniform3fv(g_uniformLightDiffuse, 1, &lightDiffuse[0]);
	glUniform3fv(g_uniformLightAmbient, 1, &lightAmbient[0]);
	glUniform3fv(g_uniformLightSpecular, 1, &lightSpecular[0]);
	glUniform3fv(g_uniformLightPosition, 1, &lightPos[0]);
	//正面
	glUniform3fv(g_uniformMaterialDiffuse, 1, &materialDiffuse[0][0]);
	glUniform3fv(g_uniformMaterialAmbient, 1, &materialAmbient[0][0]);
	glUniform3fv(g_uniformMaterialSpecular, 1, &materialSpecular[0][0]);
	//glUniform1f(g_uniformMaterialShininess, shininess);
	glUniform1f(g_uniformMaterialShininess, shininess[0]);
	//反面
	glUniform3fv(g_uniformMaterialBackDiffuse, 1, &materialDiffuse[1][0]);
	glUniform3fv(g_uniformMaterialBackAmbient, 1, &materialAmbient[1][0]);
	glUniform3fv(g_uniformMaterialBackSpecular, 1, &materialSpecular[1][0]);
	//glUniform1f(g_uniformMaterialBackShininess, shininess);
	glUniform1f(g_uniformMaterialBackShininess, shininess[1]);

	glUniformMatrix4fv(g_uniformMVMat, 1, GL_FALSE, &mvMat[0][0]);
	glUniformMatrix3fv(g_uniformNormalMat, 1, GL_FALSE, &normalMat[0][0]);
	glUniformMatrix4fv(g_uniformProjMat, 1, GL_FALSE, &projMat[0][0]);

	//透明通道
	glUniform1f(g_uniformAlpha, m_color[3]);
	//是否用颜色着色器
	glUniform1i(g_uniformIsEVcolor, (int)isEVcolor);
}

void sn3DNewMeshRender::SetRangeUniform_vcolor()
{
	int curidtype = m_curId;
	if (m_mapRenderData.find(curidtype) == m_mapRenderData.end())return;
	if (!m_mapRenderData[curidtype]->isbuiledShader)return;

	float m_MvMatrix[16];
	float m_ProjMatrix[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, m_MvMatrix);
	glGetFloatv(GL_PROJECTION_MATRIX, m_ProjMatrix);

	Matrix44f mvMat, projMat, mat;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			mvMat[i][j] = m_MvMatrix[i * 4 + j];
			projMat[i][j] = m_ProjMatrix[i * 4 + j];

			//mat[i][j] = m_mat[i * 4 + j];
		}
	//mvMat = mat * mvMat;

	Matrix33f normalMat;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			normalMat[i][j] = mvMat[i][j];

	Matrix44f mvpMat = projMat * mvMat;

	Point3f lightDiffuse;
	Point3f lightSpecular;
	Point3f lightAmbient;
	float lightPos[3];

	glGetLightfv(GL_LIGHT0, GL_AMBIENT, &lightAmbient[0]);
	glGetLightfv(GL_LIGHT0, GL_DIFFUSE, &lightDiffuse[0]);
	glGetLightfv(GL_LIGHT0, GL_SPECULAR, &lightSpecular[0]);
	glGetLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	Point3f materialDiffuse[2];
	Point3f materialSpecular[2];
	Point3f materialAmbient[2];
	//float shininess = 128.f;
	//float shininess = 20.f;
	float shininess[2] = { 20.0f, 20.0f };
	bool isEVcolor = m_colorMode == CMPerVertex? true: m_bColorType;
	//正反面
	for (int i = 0; i<2; i++)
	{
		materialAmbient[i] = m_color;
		materialDiffuse[i] = m_color;
		materialSpecular[i] = Point3f(0.0, 0.0, 0.0);
		//materialSpecular[i] = Point3f(0.5, 0.5, 0.5);
	}
	if (m_colorMode == CMMaterial && !m_material.empty())
	{
		/*std::map<MaterialFace, MaterialInfo>::iterator iter = m_material.find(MF_FRONT);
		if (iter == m_material.end())
		iter = m_material.find(MF_FRONT_AND_BACK);
		materialAmbient = Point3f(iter->second.ambient[0], iter->second.ambient[1], iter->second.ambient[2]);
		materialDiffuse = Point3f(iter->second.diffuse[0], iter->second.diffuse[1], iter->second.diffuse[2]);
		materialSpecular = Point3f(iter->second.specular[0], iter->second.specular[1], iter->second.specular[2]);*/
		std::map<MaterialFace, MaterialInfo>::iterator iter = m_material.find(MF_FRONT);
		std::map<MaterialFace, MaterialInfo>::iterator iterBack = m_material.find(MF_BACK);
		std::map<MaterialFace, MaterialInfo>::iterator iterOr = m_material.find(MF_FRONT_AND_BACK);
		if (iter == m_material.end() && iterBack == m_material.end() && iterOr != m_material.end())
		{
			//正反面
			for (int i = 0; i<2; i++)
			{
				materialAmbient[i] = Point3f(iterOr->second.ambient[0], iterOr->second.ambient[1], iterOr->second.ambient[2]);
				materialDiffuse[i] = Point3f(iterOr->second.diffuse[0], iterOr->second.diffuse[1], iterOr->second.diffuse[2]);
				materialSpecular[i] = Point3f(iterOr->second.specular[0], iterOr->second.specular[1], iterOr->second.specular[2]);
				shininess[i] = iterOr->second.shininess;
			}
		}
		else if (iter != m_material.end() && iterBack == m_material.end())
		{
			//正反面
			for (int i = 0; i<2; i++)
			{
				materialAmbient[i] = Point3f(iter->second.ambient[0], iter->second.ambient[1], iter->second.ambient[2]);
				materialDiffuse[i] = Point3f(iter->second.diffuse[0], iter->second.diffuse[1], iter->second.diffuse[2]);
				materialSpecular[i] = Point3f(iter->second.specular[0], iter->second.specular[1], iter->second.specular[2]);
				shininess[i] = iter->second.shininess;
			}
		}
		else if (iter == m_material.end() && iterBack != m_material.end())
		{
			//正反面
			for (int i = 0; i<2; i++)
			{
				materialAmbient[i] = Point3f(iterBack->second.ambient[0], iterBack->second.ambient[1], iterBack->second.ambient[2]);
				materialDiffuse[i] = Point3f(iterBack->second.diffuse[0], iterBack->second.diffuse[1], iterBack->second.diffuse[2]);
				materialSpecular[i] = Point3f(iterBack->second.specular[0], iterBack->second.specular[1], iterBack->second.specular[2]);
				shininess[i] = iterBack->second.shininess;
			}
		}
		else if (iter != m_material.end() && iterBack != m_material.end())
		{
			//正面
			materialAmbient[0] = Point3f(iter->second.ambient[0], iter->second.ambient[1], iter->second.ambient[2]);
			materialDiffuse[0] = Point3f(iter->second.diffuse[0], iter->second.diffuse[1], iter->second.diffuse[2]);
			materialSpecular[0] = Point3f(iter->second.specular[0], iter->second.specular[1], iter->second.specular[2]);
			shininess[0] = iter->second.shininess;
			//反面
			materialAmbient[1] = Point3f(iterBack->second.ambient[0], iterBack->second.ambient[1], iterBack->second.ambient[2]);
			materialDiffuse[1] = Point3f(iterBack->second.diffuse[0], iterBack->second.diffuse[1], iterBack->second.diffuse[2]);
			materialSpecular[1] = Point3f(iterBack->second.specular[0], iterBack->second.specular[1], iterBack->second.specular[2]);
			shininess[1] = iterBack->second.shininess;
		}
		isEVcolor = m_bColorType;
		//--------
		if (shininess[0] >= 0.0001 && shininess[1] >= 0.0001)
		{
			for (int i = 0; i < 3; i++)
			{
				lightAmbient[i] = 0.1f;
				lightDiffuse[i] = 1.0f;
				lightSpecular[i] = 1.0f;
			}
		}
		else
		{
			shininess[0] = 20;
			shininess[1] = 20;
		}
		//---------
	}


	int g_uniformLightDiffuse = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Light.Ld");
	int g_uniformLightAmbient = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Light.La");
	int g_uniformLightSpecular = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Light.Ls");
	int g_uniformLightPosition = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Light.Position");

	//正面材质
	int g_uniformMaterialDiffuse = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Material.Kd");
	int g_uniformMaterialAmbient = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Material.Ka");
	int g_uniformMaterialSpecular = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Material.Ks");
	int g_uniformMaterialShininess = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Material.Shininess");
	//背面材质
	int g_uniformMaterialBackDiffuse = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "MaterialBack.Kd");
	int g_uniformMaterialBackAmbient = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "MaterialBack.Ka");
	int g_uniformMaterialBackSpecular = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "MaterialBack.Ks");
	int g_uniformMaterialBackShininess = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "MaterialBack.Shininess");

	int g_uniformMVMat = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "ModelViewMatrix");
	int g_uniformNormalMat = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "NormalMatrix");
	int g_uniformProjMat = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "ProjectionMatrix");

	//透明通道
	int g_uniformAlpha = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "Alpha");
	//是否用顶点颜色
	int g_uniformIsEVcolor = glGetUniformLocation(m_mapRenderData[curidtype]->rangeShader, "EnableVColor");

	//////////////////////////////////////////////////////////////
	glUniform3fv(g_uniformLightDiffuse, 1, &lightDiffuse[0]);
	glUniform3fv(g_uniformLightAmbient, 1, &lightAmbient[0]);
	glUniform3fv(g_uniformLightSpecular, 1, &lightSpecular[0]);
	glUniform3fv(g_uniformLightPosition, 1, &lightPos[0]);
	//正面
	glUniform3fv(g_uniformMaterialDiffuse, 1, &materialDiffuse[0][0]);
	glUniform3fv(g_uniformMaterialAmbient, 1, &materialAmbient[0][0]);
	glUniform3fv(g_uniformMaterialSpecular, 1, &materialSpecular[0][0]);
	//glUniform1f(g_uniformMaterialShininess, shininess);
	glUniform1f(g_uniformMaterialShininess, shininess[0]);
	//反面
	glUniform3fv(g_uniformMaterialBackDiffuse, 1, &materialDiffuse[1][0]);
	glUniform3fv(g_uniformMaterialBackAmbient, 1, &materialAmbient[1][0]);
	glUniform3fv(g_uniformMaterialBackSpecular, 1, &materialSpecular[1][0]);
	//glUniform1f(g_uniformMaterialBackShininess, shininess);
	glUniform1f(g_uniformMaterialBackShininess, shininess[1]);

	glUniformMatrix4fv(g_uniformMVMat, 1, GL_FALSE, &mvMat[0][0]);
	glUniformMatrix3fv(g_uniformNormalMat, 1, GL_FALSE, &normalMat[0][0]);
	glUniformMatrix4fv(g_uniformProjMat, 1, GL_FALSE, &projMat[0][0]);

	//透明通道
	glUniform1f(g_uniformAlpha, m_color[3]);
	//是否用颜色着色器
	glUniform1i(g_uniformIsEVcolor, (int)isEVcolor);
}

void sn3DNewMeshRender::CompileRangeShader(emShadeType shadeType, boost::shared_ptr<sn3DCore::RenderOptData> &RenderOpt)
{
	GLenum ret = glewInit();
	if (ret != GLEW_OK)
	{
		qDebug("ycx glewInit failure");
		return;
	}

	if (glIsShader(RenderOpt->rangeShader))return;
	if (RenderOpt->isbuiledShader)return;
	//
	/*QString qsVData = ShadeAllFileLoadData::getInstance().GetVShadefileData(shadeType);
	QString qsFData = ShadeAllFileLoadData::getInstance().GetFShadefileData(shadeType);

	Shader shader;
	shader.init();
	shader.attach(GL_VERTEX_SHADER, qsVData.toStdString().c_str());
	shader.attach(GL_FRAGMENT_SHADER, qsFData.toStdString().c_str());
	shader.link();
	RenderOpt->rangeShader = shader.GetProgram();*/
	RenderOpt->rangeShader = ShadeAllFileLoadData::getInstance().GetShaderProgram(shadeType);
	RenderOpt->isbuiledShader = true;
}

void sn3DCore::sn3DNewMeshRender::SetColorType(bool bColor)
{
	if (m_bColorType != bColor)
	{
		m_bColorType = bColor;
		m_ReUpdataReader = true;
	}
}