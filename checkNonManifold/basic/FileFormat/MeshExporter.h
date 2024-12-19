#ifndef _MESH_EXPORTER_
#define _MESH_EXPORTER_

#include <vector>

#include "BaseExporter.h"
#include "../SceneObject/SceneMeshObj.h"
#include "ImageConvert.h"

namespace sn3DCore
{
	class MeshExporter : public BaseExporter
	{
	public:

		MeshExporter(sn3DMeshModel * model) : m_curModel(model) 
		{
			m_curData = model->GetMeshData().get();
		}

		MeshExporter(sn3DMeshData* mesh) : m_curModel(NULL)
		{
			m_curData = mesh; 
		}

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
			return Point3f(0.0,0.0,0.0);
		}

		Point4f  VertexColorA(int index) 
		{
			return Point4f(0.0,0.0,0.0,0.0);
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

		bool IsMultPolygon(int findex)
		{

         return m_curData->F(findex)->IsMultPolygon();
		}

		void GetTextureImage(unsigned char ** data,int & w,int & h,int & channel)
		{

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

		size_t n_texcoords(){return m_curData->n_texcoords();}

		size_t n_normals() {return m_curData->n_normals();}

	public:

		sn3DMeshModel * m_curModel;

		sn3DMeshData * m_curData;
	};
}

#endif

