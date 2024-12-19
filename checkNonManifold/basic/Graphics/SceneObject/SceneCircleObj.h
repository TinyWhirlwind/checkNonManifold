#ifndef SCENE_CIRCLE_OBJ_H
#define SCENE_CIRCLE_OBJ_H

#include "SceneGeometryObj.h"

namespace sn3DCore
{
	class sn3DCircleModel : public sn3DGeometryModel
	{
	public:
		sn3DCircleModel(Point3f c, Point3f n, float r, Color4f color = Color4f::Green);
		virtual ~sn3DCircleModel()	{}
		virtual void Render();
		virtual void UpdateAll();
		virtual void UpdateDisplay();

	public:
		Point3f m_circleC;
		Point3f m_circleN;
		float m_circleR;
		Color4f m_circleColor;
		std::vector<Point3f> m_circleEdgePts;
		bool GenerateCircleEdge();
		friend class sn3DCircleRenderer;
	};
}// namespace sn3DCore
#endif