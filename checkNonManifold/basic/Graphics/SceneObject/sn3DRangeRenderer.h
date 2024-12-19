#ifndef _SN3D_RENGE_RENDERER_
#define _SN3D_RENGE_RENDERER_

#include "sn3DRenderer.h"
#include "boost/smart_ptr.hpp"

/*    sn3DRenderer 类  

该类实现了常用OpenGL渲染绘制功能

*/

namespace sn3DCore
{
	class sn3DRangeData;
	class sn3DRangeModel;
	class sn3DRangeRenderer: public sn3DRenderer
	{
	public:
		sn3DRangeRenderer();
		virtual ~sn3DRangeRenderer();

	public:

		///////////////////////////  显示更新方法  ///////////////////////
		// 待实现 todo:
		virtual void Update()	{}

		virtual void BuildFaceList(){};			// 生成面显示列表 vbo
		virtual void BuildPointList(){};

		virtual void BuildList();
		virtual void Clear();
		
		// 待实现 todo:
		virtual void Draw()	{}

		virtual void DrawFill();
		virtual void DrawWire();
		virtual void DrawPoints();

		virtual void RenderSelected();
				void RenderRefPoints();


		void	SetGeometry(boost::shared_ptr<sn3DRangeModel> model)	{  m_model = model;}
		void	SetRefRadius(double radius)			{ m_refRadius=radius;}

	protected:
		void	SetGLFucntin();
		bool	GetColor(sn3DRangeData *data, unsigned char *color, int vn);
		void	SetFunction();

		bool BuildFace(unsigned int &vertexBuf, unsigned int &indexBuf,unsigned int &colorBuf,int detail, int &vn, int &fn);
		bool BuildPoint(unsigned int &vertexBuf, unsigned int &indexBuf,unsigned int &colorBuf,int detail,int &vn);
		void DrawFill(unsigned int &vertexBuf, unsigned int &indexBuf,unsigned int &colorBuf,int fn, int vn);
		void DrawWire(unsigned int &vertexBuf, unsigned int &indexBuf, unsigned int &colorBuf, int fn, int vn);
		void DrawPoints(unsigned int &vertexBuf, unsigned int &indexBuf,int unsigned &colorBuf,int pointSize,int vn);
	
	protected:
		int m_memory;
		boost::shared_ptr<sn3DRangeModel> m_model;   //几何数据

		double m_refRadius;

		// 静态
		unsigned int m_vertexBuffer;
		unsigned int m_indexBuffer;
		unsigned int m_colorBuffer;
		int m_vn;
		int m_fn;

		// 动态
		unsigned int m_vertexBufferDynamic;
		unsigned int m_indexBufferDynamic;
		unsigned int m_colorBufferDynamic;
		int m_vnDynamic;
		int m_fnDynamic;
		bool		m_bModified;
		bool		m_bVetexColor;
		bool		m_bDrawDynamic;
	};
}
#endif