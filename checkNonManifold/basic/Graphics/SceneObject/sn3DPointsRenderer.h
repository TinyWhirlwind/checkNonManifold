#ifndef _SN3D_POINTS_RENDERER_
#define _SN3D_POINTS_RENDERER_

#include "sn3DRenderer.h"

#include"GeoData/GeometryData.h"

#include <boost/smart_ptr.hpp>

/*     sn3DPointsRenderer 类

该类实现了基于OpenGL的大规模点云绘制功能  */

namespace sn3DCore
{
	class sn3DPointsRenderer : public sn3DRenderer
	{
	public:
		unsigned int g_rangeShader;
		virtual void  Update();
		virtual void  Draw();								//  绘制
		void  BuildList();
		void  BuildBuffer();
		void  UpdateBuffer();
		void  ClearBuffer();
		void  CompileRangeShader();

		///////////////////////////  显示数据访问  /////////////////////
		void SetGeometry(boost::shared_ptr<sn3DGeometryData> data);  // 设置几何模型

	public:
		sn3DPointsRenderer();
		virtual ~sn3DPointsRenderer();

	private:
		boost::shared_ptr<sn3DGeometryData> m_data;   //几何数据
		bool m_bufferInit; // VBO init
		unsigned int  m_vertexArray;
		unsigned int  m_vertexBuffer;//vbo = 0xffffffff;
		unsigned int  m_normalBuffer;//vbo = 0xffffffff;
		///////////  data

		std::vector<unsigned int> m_vbos;
		std::vector<unsigned int> m_ibos;
		std::vector<int> m_scales;
	};
}
#endif