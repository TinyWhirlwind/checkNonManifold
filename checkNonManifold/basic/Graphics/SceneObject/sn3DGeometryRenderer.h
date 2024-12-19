#ifndef SN3D_GEOMETRY_RENDERER_H
#define SN3D_GEOMETRY_RENDERER_H

#include "sn3DRenderer.h"

namespace sn3DCore
{
	class sn3DGeometryModel;
	class sn3DGeometryRenderer : public sn3DRenderer
	{
	public: 
		sn3DGeometryRenderer(sn3DGeometryModel* model);
		virtual ~sn3DGeometryRenderer()	{}

		virtual void Draw();
		virtual void Update();

		virtual void DrawPoints()	{}
		virtual void DrawFill()		{}
		virtual void DrawWire()		{}

		/// 构建显示列表
		virtual void BuildPointList();
		virtual void BuildFaceList();

	protected:
		sn3DGeometryModel* m_model;
	};
}// namespaec sn3DCore

#endif