#ifndef _MESH_IMPORTER_
#define _MESH_IMPORTER_

#include "BaseImporter.h"
#include "../sn3DDataModel/sn3DMeshModel.h"
#include"../sn3DDataModel/sn3DTopology.h"

namespace sn3DCore
{

	class MeshImporter : public BaseImporter
	{
	public:

		virtual void Update()
		{
			if(m_curData!=NULL&&IsRedundant())
				Topology::RemoveRedundant(m_curData);//remove redundant point
		}

		void ReserveFace(int num)
		{
			if (m_curData!=NULL)
				m_curData->m_faces.reserve(num);
		}

		void ReserveVertex(int num)
		{
			if (m_curData!=NULL)
				m_curData->m_vertices.reserve(num);
		}

		virtual void AddVertex(Point3f & _point)
		{
			if (m_curData!=NULL)
				m_curData->AddVertex(_point);
		}

		virtual void AddFace(std::vector<int> & vIndices)
		{   
			if (vIndices.size() > 2)
				m_curData->AddFace(vIndices);
		}

		virtual void AddFace(int v0, int v1, int v2)
		{   
			m_curData->AddFace(v0,v1,v2);
		}

		size_t n_vertices() { return m_curData->n_vertices();}

		size_t n_faces()    { return m_curData->n_faces();}

		MeshImporter(sn3DMeshModel * model) : m_curModel(model)
		{
			m_curData = m_curModel->GetMeshData();
			m_flag = 0;
		}

	private:

		sn3DMeshModel * m_curModel;

		sn3DMeshData * m_curData;
	};

}
#endif

