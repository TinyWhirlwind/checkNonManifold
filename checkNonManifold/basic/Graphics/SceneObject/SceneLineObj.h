#ifndef SCENE_LINE_OBJ_H
#define SCENE_LINE_OBJ_H

#include "SceneGeometryObj.h"

namespace sn3DCore
{
	class sn3DLineModel : public sn3DGeometryModel
	{
	public:
		sn3DLineModel(Point3f start, Point3f end, float lineWidth, Color4f color = Color4f::Green);
		virtual ~sn3DLineModel()	{}

		virtual void Render();

		virtual void UpdateAll();
		virtual void UpdateDisplay();

	private:
		Point3f m_start;
		Point3f m_end;
		float m_lineWidth;
		Color4f m_lineColor;

		friend class sn3DLineRenderer;
	};

}// namespace sn3DCore
#endif