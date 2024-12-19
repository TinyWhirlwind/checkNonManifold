#ifndef _POINTS_EXPORTER_
#define _POINTS_EXPORTER_

#include <vector>

#include "BaseExporter.h"
#include "SceneObject/ScenePointsObj.h"

namespace sn3DCore
{
	class PointsExporter : public BaseExporter
	{
	public:

		PointsExporter(sn3DPointsModel * model) : m_curModel(model) 
		{ 
			m_curData = model->GetPointCloud().get();
		}

		Point3f  point(int index)
		{ 
		  return m_curData->P(index); 
		}

		size_t n_vertices() { return m_curData->n_vertices(); }   

	public:

		sn3DPointsModel * m_curModel;

		sn3DPointsData * m_curData;
	};
}

#endif

