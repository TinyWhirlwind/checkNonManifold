#ifndef SN3D_SPHERE_RENDERER_H
#define SN3D_SPHERE_RENDERER_H

#include "sn3DGeometryRenderer.h"

namespace sn3DCore
{
	class sn3DSphereModel;
	class sn3DSphereRenderer : public sn3DGeometryRenderer
	{
	public:
		sn3DSphereRenderer(sn3DSphereModel* model);
		virtual ~sn3DSphereRenderer(){}

		virtual void Draw();
		virtual void Update();

		virtual void DrawFill();

	};
}

#endif