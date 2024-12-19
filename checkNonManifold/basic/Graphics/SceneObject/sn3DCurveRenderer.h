#ifndef SN3D_CURVE_RENDERER_H
#define SN3D_CURVE_RENDERER_H

#include "sn3DGeometryRenderer.h"

namespace sn3DCore
{
	class sn3DCurveModel;
	class sn3DCurveRenderer : public sn3DGeometryRenderer
	{
	public:
		sn3DCurveRenderer(sn3DCurveModel* model);
		virtual ~sn3DCurveRenderer()	{}

		virtual void Draw();
		virtual void Update();

		virtual void DrawFill();

	};

}// namespace sn3DCore

#endif