#ifndef SN3D_LINE_RENDERER_H
#define SN3D_LINE_RENDERER_H

#include "sn3DGeometryRenderer.h"

namespace sn3DCore
{
	class sn3DLineModel;
	class sn3DLineRenderer : public sn3DGeometryRenderer
	{
	public:
		sn3DLineRenderer(sn3DLineModel* model);
		virtual ~sn3DLineRenderer()	{}

		virtual void Draw();
		virtual void Update();

		virtual void DrawFill();

	};

}// namespace sn3DCore

#endif