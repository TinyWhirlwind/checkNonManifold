#ifndef _SPACEBOX_
#define _SPACEBOX_

#include <vector>
#include "Common/Point3.h"
#include "GeoData/MeshData.h"
#include "GeoData/PointsData.h"

/* 空间格栅 */
namespace sn3DCore
{
	class BoxCoord
	{
	public:
		int x;			//空间方格索引
		int y;
		int z;
		int entryIndex;	//在空间方格中的存储索引
	};

	/* CSpatialSubdivisionEntry表示空间剖分的一个单元格 */
	class SpaceBoxEntry
	{
	public:
		SpaceBoxEntry() 
		{
			m_Size = 0; 
			m_BeginIndex = NULL; 
		}
		int m_Size;			//该方格中所包含的元素数量          
		int m_BeginIndex;	//数据池中起始地址
	};


	/* 空间划分格栅 
	*	\note
	*		三个维度划分的格栅数相等
	*/
	class CSpaceBox
	{
	public:
		/// \param
		///		res		[in]	格栅数：res*res*res
		CSpaceBox(int res);

		/// 初始化空间剖分结构
		void Initialize(sn3DMeshData * m);

		void Initialize(sn3DPointsData * m);

		void Initialize(const std::vector<Point3f>& pointData);

		/// 获取空间长度
		int GetSpaceLength();

		/// 获取单元格大小
		int GetEntrySize(int x, int y, int z);

		/// 获取最小点
		Point3f GetMinPoint();

		/// 获取最大点
		Point3f GetMaxPoint();

		/// 获取单元格对象(index索引)
		int GetEntryItem(int x, int y, int z, int index);

		/// Clear是内存清除函数
		void Clear();

	private:
		void ComputeBBox(sn3DCore::sn3DMeshData* m, Point3f& minv, Point3f& maxv);
		void ComputeBBox(sn3DCore::sn3DPointsData* m, Point3f& minv, Point3f& maxv);
		void ComputeBBox(const std::vector<Point3f>& points, Point3f& minv, Point3f& maxv);

	public:
		std::vector<SpaceBoxEntry> m_SpaceBox;//空间单元格

		std::vector<int> m_DataPool;//数据

		std::vector<int> m_EntryMap;//单元格映射

		int m_DivisionNum;//空间剖分的粒度

		Point3f m_MaxCorner;//空间结构的最大点(按x,y,z排序)

		Point3f m_MinCorner;//空间结构的最小点(按x,y,z排序)

		Point3f m_Delta;//为性能而预先计算的m_DivisionNum/(m_MaxCorner-m_MinCorner)
	};

}
#endif