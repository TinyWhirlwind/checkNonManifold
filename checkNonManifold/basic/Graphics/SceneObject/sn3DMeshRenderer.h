#ifndef _Mesh_Renderer_H_
#define _Mesh_Renderer_H_

#include "sn3DRenderer.h"
#include "GeoData/GeometryData.h"
#include "boost/smart_ptr.hpp"
using boost::shared_ptr;

namespace sn3DCore
{
	/*
	typedef enum
	{
		PLANE_SHADE = 0,
		SMOOTH_SHADE,
		PHONE_SHADE,
		NORMAL_COLOR
	} MeshShaderType; // 网格曲面着色方式
	*/

	// Render渲染Mesh的不同状态用到
	struct RenderData
	{
		unsigned int  m_elementbuffer;
		unsigned int  m_vertexArray;
		unsigned int  m_vertexBuffer;	//vbo = 0xffffffff;

		int id;		//识别号
		int fn;  
		int vn;
		RenderData()
		{
			m_elementbuffer = 0xffffffff;
			m_vertexArray = 0xffffffff;
			m_vertexBuffer = 0xffffffff;
			id = -1;
			fn = 0;
			vn = 0;
		}
	};

	class sn3DMeshRenderer : public sn3DRenderer
	{
	public:
		sn3DMeshRenderer();
		virtual ~sn3DMeshRenderer();

		virtual void Clear();

		///////////////////////////  绘制方法      ///////////////////////

		virtual void Draw();
		virtual void DrawFill();
		virtual void DrawWire();
		virtual void DrawPoints();
		virtual void StartProperty();
		virtual void EndProperty();
		virtual void RenderSelected();
		virtual void RenderDecorate(int index);

		////////////////////////// 设置绘制参数 ///////////////////////
		virtual void SetDrawMode(DrawMode dm);
		virtual void SetRenderMethod(RenderMethod rm);
		virtual void SetShadeMode(ShadeMode sm);
		virtual void SetColorMode(ColorMode cm);

		///////////////////////////  显示更新方法  ///////////////////////
		virtual void Update();

		/// 显示列表
		virtual void BuildPointList();        //  生成顶点显示列表
		virtual void BuildFaceList();         //  生成面显示列表

		/// Shader(ARB)
		void BuildBufferARB();
		void UpdateBufferARB();
		void ClearBufferARB();
		void DrawBufferARB();

		/// Shader
		virtual void BuildRangeData();
		virtual void DrawRangeData();

		virtual void DrawRangeDataFill();
		virtual void DrawRangeDataFillWire();

		/// 编译Shader
		/// \note
		///		父类可重写Shader
		virtual void CompileRangeShader();

		/// 设置Uniform参数
		///	\note
		///		父类可重写
		virtual void SetRangeUniform();

		///////////////////////////  数据访问  /////////////////////

		/// 设置几何模型
		void SetGeometry(boost::shared_ptr<sn3DGeometryData> data);  

		/// 设置模型变换矩阵4x4
		void SetTransform(float* mat);

		/// 设置模型颜色
		void SetColor(float *color);

	public:
		boost::shared_ptr<sn3DGeometryData> m_data;		//几何数据
		float m_mat[16];								//模型变换矩阵(行优先)
		float m_color[4];								//模型颜色

		bool m_bufferInit;				// VBO init
		unsigned int m_vertexBuffer;	// vertex buffer
		unsigned int m_indexBuffer;		// index buffer 
		unsigned int m_vertexArray;
		unsigned int m_rangeShader;			//shader句柄 

	protected:
		bool m_ReUpdataReader;             //更新reader
		bool m_SetRanderType;
		
	};
}


#endif