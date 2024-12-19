#ifndef SN3D_CIRCLE_RENDERER_H
#define SN3D_CIRCLE_RENDERER_H

#include "sn3DGeometryRenderer.h"

namespace sn3DCore
{
	class sn3DCircleModel;
	class sn3DCircleRenderer : public sn3DGeometryRenderer
	{
	public:
		sn3DCircleRenderer(sn3DCircleModel* model);
		virtual ~sn3DCircleRenderer()	{}

		virtual void Draw();
		virtual void Update();

		virtual void DrawFill();

	};

}// namespace sn3DCore

#endif