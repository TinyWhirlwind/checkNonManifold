//#include"stdafx.h"
#include"PointsData.h"

namespace sn3DCore
{
	sn3DPointsData::sn3DPointsData()
	{
		m_memSize = 0;
		vn = 0;
		m_pointMem.clear();
		m_pointFlag.clear();
	}

	sn3DPointsData::sn3DPointsData(boost::shared_ptr<sn3DPointsData> pointsdata)
	{
		vn = 0;
		m_memSize = 0;
		if (pointsdata->vn > 0)
		{
			for (size_t i = 0; i < pointsdata->vn; ++i)
			{
				AddPoint(pointsdata->m_pointMem[i]);
				m_pointFlag[i] = pointsdata->m_pointFlag[i];
			}
		}

		if (pointsdata->m_vertexMark.size())
		{
			m_vertexMark = pointsdata->m_vertexMark;
		}
		if (pointsdata->m_vertexTexCoord.size())
		{
			m_vertexTexCoord = pointsdata->m_vertexTexCoord;
		}
		assert(vn == pointsdata->vn);
		assert(m_memSize == pointsdata->m_memSize);
	}

	sn3DPointsData::~sn3DPointsData()
	{

	}

	size_t  sn3DPointsData::n_vertices() const
	{
		return vn;
	}

	Point3f &sn3DPointsData::P(int index)
	{
		return m_vertices[index].P();
	}

	Vertex *sn3DPointsData::V(int index)
	{
		return &m_vertices[index];
	}
	int &sn3DPointsData::Flag(int index)
	{
		return m_pointFlag[index];
	}

	Vertex *sn3DPointsData::AddVertex(const Point3f & point)
	{
		int index = (int)m_vertices.size();
		Vertex vertex(point, index, this);
		m_vertices.push_back(vertex);
		vn++;
		return &m_vertices.back();
	}

	void sn3DPointsData::AddColor(Color4f & color)
	{
		m_vertexColor.push_back(color);
	}

	void sn3DPointsData::AddPoint(Point3f point)
	{
		if (m_pointMem.empty())
		{
			m_memSize = 10000;
			m_pointMem.resize(m_memSize);
			m_pointFlag.resize(m_memSize);
		}
		else if (vn == m_memSize)
		{
			m_memSize += 10000;// 线性增长
			m_pointMem.resize(m_memSize);
			m_pointFlag.resize(m_memSize);
		}

		m_pointMem.push_back(point);
		m_pointFlag.push_back(0);
		vn = (int)m_pointMem.size();
	}

	void sn3DPointsData::Clear()
	{
		vn = 0;
		m_memSize = 0;
		m_pointMem.clear();
		m_pointFlag.clear();
		if (m_vertexTexCoord.size())
			std::vector<Point2f>().swap(m_vertexTexCoord);
	}

	void sn3DPointsData::UpdateVertexMark()
	{
		m_vertexMark.resize(vn);
	}

	void sn3DPointsData::UpdateVertexTexCoord()
	{
		m_vertexTexCoord.resize(vn, Point2f(0.0, 0.0));
	}

	void sn3DPointsData::Compact()
	{
		int  indexV = 0;
		size_t  end = m_vertices.size();

		for (size_t i = 0; i < end; i++)
		{
			if (!m_vertices[i].IsD())
				indexV++;
		}

		if (indexV == end) return;

		int tempIndexV = 0;
		float * newPX = new float[indexV];
		float * newPY = new float[indexV];
		float * newPZ = new float[indexV];
		int * flagV = new int[indexV];

		Point3f * vertexNormal = 0;
		if (m_vertexNormal.size() == vn)
		{
			vertexNormal = new Point3f[indexV];
		}

		end = m_vertices.size();
		for (int i = 0; i < end; i++)
		{
			if (m_vertices[i].IsD())continue;
			newPX[tempIndexV] = m_vertices[i].m_coord.X();
			newPY[tempIndexV] = m_vertices[i].m_coord.Y();
			newPZ[tempIndexV] = m_vertices[i].m_coord.Z();
			if (vertexNormal)vertexNormal[tempIndexV] = m_vertexNormal[i];
			tempIndexV++;
		}

		//======
		m_vertices.resize(indexV);
		if (vertexNormal != 0)
			m_vertexNormal.resize(indexV);

		for (int i = 0; i < indexV; i++)
		{
			m_vertices[i].m_coord.X() = newPX[i];
			m_vertices[i].m_coord.Y() = newPY[i];
			m_vertices[i].m_coord.Z() = newPZ[i];
			m_vertices[i].Flag() = flagV[i];
			m_vertices[i].Index() = i;
			m_vertices[i].SetGeometry(this);
			if (vertexNormal)
				m_vertexNormal[i] = vertexNormal[i];
		}

		vn = (int)m_vertices.size();

		//======	
		delete newPX;
		delete newPY;
		delete newPZ;
		delete flagV;
		if (vertexNormal != 0)delete vertexNormal;
	}
}