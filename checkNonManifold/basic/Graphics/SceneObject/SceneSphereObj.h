#ifndef SCENE_SPHERE_OBJ_H
#define SCENE_SPHERE_OBJ_H

#include "SceneGeometryObj.h"

namespace sn3DCore
{
	class sn3DSphereModel : public sn3DGeometryModel
	{
	public:
		sn3DSphereModel(Point3f c, float r, Color4f clr = Color4f::Red);
		virtual ~sn3DSphereModel() {}

		virtual void Render();

		virtual void UpdateAll();
		virtual void UpdateDisplay();

	private:
		Point3f m_center;
		float m_radius;
		Color4f m_color;
		friend class sn3DSphereRenderer;
	};
}//namespace sn3DCore
#endif