#ifndef _NEW_Mesh_Renderer_H_
#define _NEW_Mesh_Renderer_H_
#include "sn3DMeshRenderer.h"
//#include <gl/gl.h>
#include "qstring.h"
namespace sn3DCore
{
	enum emShadeType
	{
		EN_SHADETYPE_COM,             //模型通用shade，进行材质着色
		EN_SHADETYPE_VCOLOR,          //每个顶点颜色传入进行着色

		EN_SHADETYPE_NO
	};
	// Render渲染Mesh的不同状态用到
	struct RenderOptData
	{
		emShadeType shadetype;
		unsigned int  vertexArray;          //顶点素组对象VAO
		unsigned int  vertexBuffer;	        //顶点缓冲对象VBO
		unsigned int  elementbuffer;       //元素缓冲对象EBO
		std::vector<int>   veclocation;    //顶点着色器传入参数地址
		unsigned int rangeShader;          //着色器程序对象
		bool isbuiledShader;               //着色器是否链接过
		RenderOptData();
		bool  DeleteRangerOpt();
		bool  BindRangerOpt();
		bool DisableRangerOpt();
	};

	class ShadeAllFileLoadData
	{
	public:
		static ShadeAllFileLoadData &getInstance();
	private:
		ShadeAllFileLoadData() {};
		~ShadeAllFileLoadData() {};
	public:
		unsigned int GetShaderProgram(emShadeType type);
	private:
		QString GetVShadefileData(emShadeType type);
		QString GetFShadefileData(emShadeType type);
	private:
		bool ReadFileToString(QString filePath, QString &data);
		QString  GetShadeTypeComVSFileName();  
		QString  GetShadeTypeComFSFileName(); 
		QString  GetShadeTypeVColorVSFileName();
		
	private:
		std::map<int, QString> m_mapVShadefileData;
		std::map<int, QString> m_mapFShadefileData;
		std::map<int, unsigned int> m_mapShaderProgram;
	};

	class sn3DNewMeshRender :public sn3DMeshRenderer
	{
	public:
		sn3DNewMeshRender();
		~sn3DNewMeshRender();
		void AddReaderShade(emShadeType shadeType, bool setCurType = false);
		bool SwitchReaderShade(emShadeType shadeType);
		//重写
		void BuildRangeData();
		void DrawRangeDataFill();
	private:
		//--
		void BuildRangeData_Com();
		void BuildRangeData_VColor();
		//--
		void SetRangeUniform_com();
		void SetRangeUniform_vcolor();
	private:
	    void CompileRangeShader(emShadeType shadeType, boost::shared_ptr<sn3DCore::RenderOptData> &RenderOpt);

	public:
		emShadeType m_curId; //RenderData识别号
		std::map<int, boost::shared_ptr<sn3DCore::RenderOptData>> m_mapRenderData;

	private:
		bool m_bColorType = false;
	public:
		void SetColorType(bool bColor);

	};
}

#endif

