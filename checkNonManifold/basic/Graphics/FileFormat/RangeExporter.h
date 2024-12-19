#ifndef _MESH_EXPORTER_
#define _MESH_EXPORTER_

#include <vector>

#include "BaseExporter.h"

#include "../sn3DDataModel/sn3DMeshModel.h"

namespace sn3DCore
{

	class MeshExporter : public BaseExporter
	{
	public:

		MeshExporter(sn3DMeshModel * model) : m_curModel(model) 
		{ m_curData = model->GetMeshData();}

		Point3f  point(int index)
		{ 
			return m_curData->V(index)->m_coord; 
		}

		Point3f  VertexNormal(int index)  
		{ 
			return m_curData->V(index)->N(); 
		}

		Point3f  VertexColor(int index)  
		{
			return m_curData->V(index)->Color();
		}

		Point4f  VertexColorA(int index) 
		{
			return m_curData->V(index)->ColorA();
		}

		Point2f  VertexTexcoord(int index)
		{
			return m_curData->V(index)->TexCoord();		
		}

		unsigned int GetVertices(int fIndex,std::vector<int>& _vIndices)
		{
			unsigned int count(0);
			_vIndices.clear();

			for (int j=0; j<3; ++j)
			{
				_vIndices.push_back(m_curData->F(fIndex)->V(j)->Index());
				++count;
			}
			return count;
		}

		Point3f  FaceNormal(int index)
		{ 
			return m_curData->F(index)->N();
		}

		Point3f  FaceColor(int index)
		{ 
			return m_curData->F(index)->Color();
		}

		Point4f  FaceColorA(int index)
		{ 
			return m_curData->F(index)->ColorA();
		}

		size_t n_vertices() { return m_curData->n_vertices(); }   

		size_t n_faces()    { return m_curData->n_faces(); }

	public:

		sn3DMeshModel * m_curModel;

		sn3DMeshData * m_curData;
	};

}

#endif

