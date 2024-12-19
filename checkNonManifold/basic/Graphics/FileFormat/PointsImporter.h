#ifndef _POINTSIMPORTER_
#define _POINTSIMPORTER_

#include "BaseImporter.h"
#include "../SceneObject/ScenePointsObj.h"

namespace sn3DCore
{
	class PointsImporter : public BaseImporter
	{
	public:
		virtual void Update()
		{
			if (m_curData == nullptr)
				return;
		}

		SceneObject * GetObjectPointer()
		{
			return m_curModel;
		}

		virtual void AddVertex(Point3f & _point)
		{
			if (m_curData != nullptr)
				m_curData->AddVertex(_point);
		}

		virtual void AddVertexColor(Color4f & _color)
		{
			if (m_curData != nullptr)
				m_curData->AddColor(_color);
		}

		void AddVertexNormal(Point3f normal)
		{
			m_curData->m_vertexNormal.push_back(normal);
		}

		size_t n_vertices() { return m_curData->n_vertices(); }

		size_t n_faces() { return 0; }

		PointsImporter()
		{
			m_texId = -1;
		}

		PointsImporter(sn3DPointsModel * model) : m_curModel(model)
		{
			m_curData = m_curModel->GetPointCloud().get();
			m_flag = 0;
			m_texId = -1;
		}

		PointsImporter(sn3DPointsData * data) : m_curData(data)
		{
			m_curModel = nullptr;
			m_flag = 0;
			m_texId = -1;
		}

	private:
		sn3DPointsModel * m_curModel;
		sn3DPointsData * m_curData;
	};

}
#endif

