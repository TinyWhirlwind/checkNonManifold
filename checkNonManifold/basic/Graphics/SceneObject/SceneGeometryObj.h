#ifndef SCENE_GEOMETRY_OBJ_H
#define SCENE_GEOMETRY_OBJ_H

#include "SceneObject.h"
#include "boost/smart_ptr.hpp"

/* 场景几何绘制体基类 */
namespace sn3DCore
{
	class sn3DGeometryRenderer;
	class sn3DGeometryModel : public SceneObject
	{
	public:
		sn3DGeometryModel()			{}
		virtual ~sn3DGeometryModel(){}

		virtual void Render()		{}

		virtual void UpdateAll()	{}
		virtual void UpdateDisplay() {}

	protected:
		boost::shared_ptr<sn3DCore::sn3DGeometryRenderer> GetRenderer()	
		{
			return m_renderer;
		}
		
		boost::shared_ptr<sn3DCore::sn3DGeometryRenderer> m_renderer;
	};
}// namespace sn3DCore

#endif