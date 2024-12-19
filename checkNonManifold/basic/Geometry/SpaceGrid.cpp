
#include "SpaceGrid.h"

namespace sn3DCore
{
	CSpaceGrid::CSpaceGrid(void)
	{
		m_DivisionNum = 35;
	}

	void CSpaceGrid::Initialize(sn3DMeshData * data, int scale)
	{
		m_SpaceBox.clear();
		m_DataPool.clear();
		m_EntryMap.clear();

		int sequence = 0;

		if (scale < 1) return;

		m_DivisionNum = scale;

		std::vector<int> vertexIndices;

		int *  boxNum = new int[m_DivisionNum*m_DivisionNum];
		vertexIndices.resize(data->vn);
		m_EntryMap.resize(m_DivisionNum*m_DivisionNum, -1);



		Point3f maxv, minv;

		minv.X() = maxv.X() = data->V(0)->m_coord.X();
		minv.Y() = maxv.Y() = data->V(0)->m_coord.Y();
		minv.Z() = maxv.Z() = data->V(0)->m_coord.Z();

		for (int i = 1; i < (int)data->vn; i++)
		{
			if (data->V(i)->IsD())continue;
			if (minv.X() > data->V(i)->m_coord.X()) minv.X() = data->V(i)->m_coord.X();
			if (minv.Y() > data->V(i)->m_coord.Y()) minv.Y() = data->V(i)->m_coord.Y();
			if (minv.Z() > data->V(i)->m_coord.Z()) minv.Z() = data->V(i)->m_coord.Z();
			if (maxv.X() < data->V(i)->m_coord.X()) maxv.X() = data->V(i)->m_coord.X();
			if (maxv.Y() < data->V(i)->m_coord.Y()) maxv.Y() = data->V(i)->m_coord.Y();
			if (maxv.Z() < data->V(i)->m_coord.Z()) maxv.Z() = data->V(i)->m_coord.Z();
		}

		m_MaxCorner = maxv;
		m_MinCorner = minv;
		m_Delta.X() = (float)m_DivisionNum / (m_MaxCorner.X() - m_MinCorner.X());
		m_Delta.Y() = (float)m_DivisionNum / (m_MaxCorner.Y() - m_MinCorner.Y());

		for (unsigned int i = 0; i < data->vn; i++)
		{
			int x = (int)((data->V(i)->m_coord.X() - m_MinCorner.X())* m_Delta.X());
			if (x == m_DivisionNum) x = m_DivisionNum - 1;
			int y = (int)((data->V(i)->m_coord.Y() - m_MinCorner.Y())* m_Delta.Y());
			if (y == m_DivisionNum) y = m_DivisionNum - 1;

			if (m_EntryMap[x*m_DivisionNum + y] == -1)
			{
				boxNum[x*m_DivisionNum + y] = 1;
				m_EntryMap[x*m_DivisionNum + y] = sequence;
				vertexIndices[i] = sequence;
				sequence++;
			}
			else
			{
				boxNum[x*m_DivisionNum + y]++;
				vertexIndices[i] = m_EntryMap[x*m_DivisionNum + y];
			}
		}

		m_DataPool.resize(data->vn);
		m_SpaceBox.resize(sequence);


		for (int i = 0; i < m_DivisionNum; i++)
			for (int j = 0; j < m_DivisionNum; j++)
			{
				if (m_EntryMap[i*m_DivisionNum + j] != -1)
				{
					int pos = m_EntryMap[i*m_DivisionNum + j];

					m_SpaceBox[pos].m_Size = boxNum[i*m_DivisionNum + j];

				}
			}

		int beginIndex = 0;

		for (int i = 0; i < sequence; i++)
		{
			m_SpaceBox[i].m_BeginIndex = beginIndex;

			beginIndex += m_SpaceBox[i].m_Size;

			m_SpaceBox[i].m_Size = 0;
		}

		for (int i = 0; i < (int)data->vn; i++)
		{
			int pos = vertexIndices[i];

			int begin = m_SpaceBox[pos].m_BeginIndex;

			int size = m_SpaceBox[pos].m_Size;

			m_DataPool[begin + size] = i;

			m_SpaceBox[pos].m_Size++;
		}

		delete boxNum;
		vertexIndices.clear();
	}


	Point3f CSpaceGrid::GetMinPoint()
	{
		return m_MinCorner;
	}
	Point3f CSpaceGrid::GetMaxPoint()
	{
		return  m_MaxCorner;
	}
	int CSpaceGrid::GetSpaceLength()
	{
		return  m_DivisionNum;
	}
	int CSpaceGrid::GetEntrySize(int x, int y)
	{
		if (x > m_DivisionNum)return -1;
		if (y > m_DivisionNum)return -1;
		if (x < 0)return -1;
		if (y < 0)return -1;

		int pos = m_EntryMap[x*m_DivisionNum + y];

		if (pos == -1)return -1;

		return m_SpaceBox[pos].m_Size;
	}
	int CSpaceGrid::GetEntryItem(int x, int y, int index)
	{
		int pos = m_EntryMap[x*m_DivisionNum + y];

		if (pos == -1)return -1;

		return m_DataPool[m_SpaceBox[pos].m_BeginIndex + index];

	}
	void CSpaceGrid::Clear()
	{
		m_SpaceBox.clear();

		m_DataPool.clear();

		m_EntryMap.clear();

	}
}