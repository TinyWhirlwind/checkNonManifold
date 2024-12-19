#ifndef SCENE_CURVE_OBJ_H
#define SCENE_CURVE_OBJ_H

#include "SceneGeometryObj.h"

namespace sn3DCore
{
	class sn3DCurveModel : public sn3DGeometryModel
	{
	public:
		sn3DCurveModel(const std::vector<Point3f>& curve, bool bClosed, float fLineWidth, Color4f clr = Color4f::Red);
		virtual ~sn3DCurveModel() {}
		virtual void Render();
		virtual void UpdateAll();
		virtual void UpdateDisplay();

	public:
		void setCurve(const std::vector<Point3f>& curve, bool bClosed);
		void setCurveColor(Color4f clr);

	private:
		std::vector<Point3f> m_curve;
		bool m_closed;
		float m_linewidth;
		Color4f m_color;
		friend class sn3DCurveRenderer;
	};
}//namespace sn3DCore
#endif