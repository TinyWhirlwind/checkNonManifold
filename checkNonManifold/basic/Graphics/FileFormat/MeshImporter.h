#ifndef _MESHIMPORTER_
#define _MESHIMPORTER_

#include "BaseImporter.h"
#include "SceneObject/SceneMeshObj.h"
#include "GeoData/MeshTopology.h"

namespace sn3DCore
{
	class MeshImporter : public BaseImporter
	{
	public:

		virtual void Update()
		{
			if (m_curData == nullptr)return;
			if (m_curData != nullptr && IsRedundant())
				Topology::RemoveRedundant(m_curData);//remove redundant point

			if (m_curModel != nullptr)
			{
				if (m_curData->m_faceTexCoord.size() != 0)
				{
					m_curData->UpdateVertexTexCoord();
					for (int i = 0; i < (int)m_curData->fn; i++)
					{
						m_curData->F(i)->V(0)->SetTexCoord(m_curData->F(i)->TexCoord(0));
						m_curData->F(i)->V(1)->SetTexCoord(m_curData->F(i)->TexCoord(1));
						m_curData->F(i)->V(2)->SetTexCoord(m_curData->F(i)->TexCoord(2));
					}
				}
				else
				{

				}
			}
		}

		SceneObject * GetObjectPointer()
		{
			return m_curModel;
		}
		void ReserveFace(int num)
		{
			if (m_curData != nullptr)
				m_curData->m_faces.reserve(num);
		}

		void ReserveVertex(int num)
		{
			if (m_curData != nullptr)
				m_curData->m_vertices.reserve(num);
		}

		virtual void AddVertex(Point3f & _point)
		{
			if (m_curData != nullptr)
				m_curData->AddVertex(_point);
		}

		virtual void AddVertex(Point3f& _point, Color4f& _color) 
		{
			if (m_curData != nullptr)
			{
				Vertex* v = m_curData->AddVertex(_point, _color);
			}
		}; 

		virtual void AddFace(std::vector<int> & vIndices)
		{
			if (vIndices.size() > 2)
				m_curData->AddFace(vIndices);
		}

		virtual void AddFace(int v0, int v1, int v2)
		{
			m_curData->AddFace(v0, v1, v2);
		}

		virtual void AddFace(int v0, int v1, int v2, bool mult)
		{
			Face * f = m_curData->AddFace(v0, v1, v2);
		}

		virtual void AddVertexColor(Color4f & color)
		{
			m_curData->m_vertexColor.push_back(color);
		}

		void AddFaceTexCoord(Point2f p0, Point2f p1, Point2f p2)
		{
			m_curData->AddFaceTexCoord(p0, p1, p2);
		}

		void AddVertexNormal(Point3f normal)
		{
			m_curData->m_vertexNormal.push_back(normal);
		}

		size_t n_vertices() { return m_curData->n_vertices(); }
		size_t n_faces() { return m_curData->n_faces(); }

		MeshImporter()
		{
			m_texId = -1;
		}

		MeshImporter(sn3DMeshModel * model) : m_curModel(model)
		{
			m_curData = m_curModel->GetMeshData().get();
			m_flag = 0;
			m_texId = -1;
		}

		MeshImporter(sn3DMeshData * data) : m_curData(data)
		{
			m_curModel = nullptr;
			m_flag = 0;
			m_texId = -1;
		}

	private:
		sn3DMeshModel * m_curModel;
		sn3DMeshData * m_curData;
	};
}
#endif

