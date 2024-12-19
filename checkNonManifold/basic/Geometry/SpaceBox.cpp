//#include "stdafx.h"
#include "SpaceBox.h"
//#include <QDebug>
#include "Common/base.h"

#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif

namespace sn3DCore
{
	CSpaceBox::CSpaceBox(int res)
	{
		//这个值的确定由牙齿模型与内存分配共同决定：越大，效率越高，但内存负担越重；反之，效率低，内存负担小。	
		m_DivisionNum = res;
	}
	void CSpaceBox::Initialize(sn3DMeshData * m)
	{
		std::vector<Point3f> pointData(m->m_vertices.size());
		for (int i = 0; i < m->m_vertices.size(); ++i)
			pointData.push_back(m->V(i)->P());
		
		Initialize(pointData);
	}

	void CSpaceBox::Initialize(sn3DPointsData * m)
	{
		std::vector<Point3f> pointData(m->vn);
		for (int i = 0; i < m->vn; ++i)
			pointData.push_back(m->P(i));

		Initialize(pointData);
	}

	void CSpaceBox::Initialize(const std::vector<Point3f>& pointData)
	{
		m_SpaceBox.clear();
		m_DataPool.clear();//数据
		m_EntryMap.clear();//单元格映射

		//确定包围盒最大点与最小点
		Point3f maxv, minv;
		ComputeBBox(pointData, minv, maxv);
		
		SpaceBoxEntry entry;	//单元
		BoxCoord coord;			//单元坐标
		int sequence = 0;		//分配过程中得单元索引

		int num = pointData.size();
		std::vector<int> boxNum(m_DivisionNum*m_DivisionNum*m_DivisionNum);//每个单元中有多少个元素
		std::vector<int> entryIndex(num);//顶点在单元格中的存储顺序索引

		m_EntryMap.resize(m_DivisionNum*m_DivisionNum*m_DivisionNum);//单元存储映射

		//初始化空间状态
		for (int i = 0; i < m_DivisionNum; i++)
		{
			for (int j = 0; j < m_DivisionNum; j++)
			{
				for (int k = 0; k < m_DivisionNum; k++)
				{
					m_EntryMap[i*m_DivisionNum*m_DivisionNum + j*m_DivisionNum + k] = -1;//-1标识单元没有元素
				}
			}
		}

		//初始化层次结构几何信息

		m_MaxCorner = maxv;
		m_MinCorner = minv;
		if (AXYMath::Equivalent((m_MaxCorner.X() - m_MinCorner.X()), 0.0f))
			m_Delta.X() = 0.f;
		else
			m_Delta.X() = (float)m_DivisionNum / (m_MaxCorner.X() - m_MinCorner.X());//为提高性能而进行的预计算
		if (AXYMath::Equivalent((m_MaxCorner.Y() - m_MinCorner.Y()), 0.0f))
			m_Delta.Y() = 0.f;
		else
			m_Delta.Y() = (float)m_DivisionNum / (m_MaxCorner.Y() - m_MinCorner.Y());
		if (AXYMath::Equivalent((m_MaxCorner.Z() - m_MinCorner.Z()), 0.0f))
			m_Delta.Z() = 0.f;
		else
			m_Delta.Z() = (float)m_DivisionNum / (m_MaxCorner.Z() - m_MinCorner.Z());

		for (int i = 0; i < num; i++)
		{
			//计算单元坐标
			coord.x = (int)((pointData[i].X() - m_MinCorner.X())* m_Delta.X());
			if (coord.x == m_DivisionNum) coord.x = m_DivisionNum - 1;
			coord.y = (int)((pointData[i].Y() - m_MinCorner.Y())* m_Delta.Y());
			if (coord.y == m_DivisionNum) coord.y = m_DivisionNum - 1;
			coord.z = (int)((pointData[i].Z() - m_MinCorner.Z()) * m_Delta.Z());
			if (coord.z == m_DivisionNum) coord.z = m_DivisionNum - 1;

			if (m_EntryMap[coord.x*m_DivisionNum*m_DivisionNum + coord.y*m_DivisionNum + coord.z] == -1)//第一次存入元素
			{
				boxNum[coord.x*m_DivisionNum*m_DivisionNum + coord.y*m_DivisionNum + coord.z] = 1;//初始化元素数目 
				m_EntryMap[coord.x*m_DivisionNum*m_DivisionNum + coord.y*m_DivisionNum + coord.z] = sequence;//初始化单元索引
				entryIndex[i] = sequence;//单元索引
				sequence++;//单元索引增加
			}
			else
			{
				boxNum[coord.x*m_DivisionNum*m_DivisionNum + coord.y*m_DivisionNum + coord.z]++;//元素数目增加
				entryIndex[i] = m_EntryMap[coord.x*m_DivisionNum*m_DivisionNum + coord.y*m_DivisionNum + coord.z];
			}
		}

		///搜索空间存储数据

		m_DataPool.resize(num);//分配数据空间大小为模型顶点数目

		m_SpaceBox.resize(sequence);//分配单元空间大小为单元数目

		//初始化单元大小
		for (int i = 0; i < m_DivisionNum; i++)
		{
			for (int j = 0; j < m_DivisionNum; j++)
			{
				for (int k = 0; k < m_DivisionNum; k++)
				{
					if (m_EntryMap[i*m_DivisionNum*m_DivisionNum + j*m_DivisionNum + k] != -1)//如果盒子非空
					{
						int pos = m_EntryMap[i*m_DivisionNum*m_DivisionNum + j*m_DivisionNum + k];//存储位置

						m_SpaceBox[pos].m_Size = boxNum[i*m_DivisionNum*m_DivisionNum + j*m_DivisionNum + k];

					}
				}
			}
		}

		//初始存储位置
		int beginIndex = 0;//单元数据起始索引

		for (int i = 0; i < sequence; i++)
		{
			m_SpaceBox[i].m_BeginIndex = beginIndex;

			beginIndex += m_SpaceBox[i].m_Size;

			m_SpaceBox[i].m_Size = 0;//清零，存储元素时重新计数
		}

		//存储每个单元格的顶点信息
		for (int i = 0; i < num; i++)
		{
			int pos = entryIndex[i];

			int begin = m_SpaceBox[pos].m_BeginIndex;

			int size = m_SpaceBox[pos].m_Size;

			m_DataPool[begin + size] = i;//存入顶点标识

			m_SpaceBox[pos].m_Size++;
		}
	}

	Point3f CSpaceBox::GetMinPoint()
	{
		return m_MinCorner;
	}
	Point3f CSpaceBox::GetMaxPoint()
	{
		return  m_MaxCorner;
	}
	//获取空间长度
	int CSpaceBox::GetSpaceLength()
	{

		return  m_DivisionNum;
	}

	//获取单元格大小
	int CSpaceBox::GetEntrySize(int x, int y, int z)
	{
		if (x > m_DivisionNum)return -1;
		if (y > m_DivisionNum)return -1;
		if (z > m_DivisionNum)return -1;
		if (x < 0)return -1;
		if (y < 0)return -1;
		if (z < 0)return -1;


		int pos = m_EntryMap[x*m_DivisionNum*m_DivisionNum + y*m_DivisionNum + z];

		if (pos == -1)return -1;

		return m_SpaceBox[pos].m_Size;//返回单元大小
	}
	//获取单元格对象(index索引)
	int CSpaceBox::GetEntryItem(int x, int y, int z, int index)
	{
		int pos = m_EntryMap[x*m_DivisionNum*m_DivisionNum + y*m_DivisionNum + z];

		if (pos == -1)return -1;

		return m_DataPool[m_SpaceBox[pos].m_BeginIndex + index];

	}
	void CSpaceBox::Clear()
	{
		m_SpaceBox.clear();
		std::vector<SpaceBoxEntry>().swap(m_SpaceBox);

		m_DataPool.clear();//数据
		std::vector<int>().swap(m_DataPool);

		m_EntryMap.clear();//单元格映射
		std::vector<int>().swap(m_EntryMap);

	}

	void CSpaceBox::ComputeBBox(sn3DCore::sn3DMeshData* m, Point3f& minv, Point3f& maxv)
	{
		// 确定包围盒最大点与最小点
		minv.X() = maxv.X() = m->V(0)->m_coord.X();
		minv.Y() = maxv.Y() = m->V(0)->m_coord.Y();
		minv.Z() = maxv.Z() = m->V(0)->m_coord.Z();

		for (int i = 1; i < (int)m->vn; i++)
		{
			if (minv.X() > m->V(i)->m_coord.X()) minv.X() = m->V(i)->m_coord.X();
			if (minv.Y() > m->V(i)->m_coord.Y()) minv.Y() = m->V(i)->m_coord.Y();
			if (minv.Z() > m->V(i)->m_coord.Z()) minv.Z() = m->V(i)->m_coord.Z();
			if (maxv.X() < m->V(i)->m_coord.X()) maxv.X() = m->V(i)->m_coord.X();
			if (maxv.Y() < m->V(i)->m_coord.Y()) maxv.Y() = m->V(i)->m_coord.Y();
			if (maxv.Z() < m->V(i)->m_coord.Z()) maxv.Z() = m->V(i)->m_coord.Z();
		}
	}
	void CSpaceBox::ComputeBBox(sn3DCore::sn3DPointsData* m, Point3f& minv, Point3f& maxv)
	{
		// 确定包围盒最大点与最小点
		minv.X() = maxv.X() = m->V(0)->m_coord.X();
		minv.Y() = maxv.Y() = m->V(0)->m_coord.Y();
		minv.Z() = maxv.Z() = m->V(0)->m_coord.Z();

		for (int i = 1; i < (int)m->vn; i++)
		{
			if (minv.X() > m->V(i)->m_coord.X()) minv.X() = m->V(i)->m_coord.X();
			if (minv.Y() > m->V(i)->m_coord.Y()) minv.Y() = m->V(i)->m_coord.Y();
			if (minv.Z() > m->V(i)->m_coord.Z()) minv.Z() = m->V(i)->m_coord.Z();
			if (maxv.X() < m->V(i)->m_coord.X()) maxv.X() = m->V(i)->m_coord.X();
			if (maxv.Y() < m->V(i)->m_coord.Y()) maxv.Y() = m->V(i)->m_coord.Y();
			if (maxv.Z() < m->V(i)->m_coord.Z()) maxv.Z() = m->V(i)->m_coord.Z();
		}
	}
	void CSpaceBox::ComputeBBox(const std::vector<Point3f>& points, Point3f& minv, Point3f& maxv)
	{
		if (points.empty())
			return;

		// 确定包围盒最大点与最小点
		minv.X() = maxv.X() = points[0].X();
		minv.Y() = maxv.Y() = points[0].Y();
		minv.Z() = maxv.Z() = points[0].Z();

		for (int i = 1; i < points.size(); i++)
		{
			if (minv.X() > points[i].X()) minv.X() = points[i].X();
			if (minv.Y() > points[i].Y()) minv.Y() = points[i].Y();
			if (minv.Z() > points[i].Z()) minv.Z() = points[i].Z();
			if (maxv.X() < points[i].X()) maxv.X() = points[i].X();
			if (maxv.Y() < points[i].Y()) maxv.Y() = points[i].Y();
			if (maxv.Z() < points[i].Z()) maxv.Z() = points[i].Z();
		}
	}
}