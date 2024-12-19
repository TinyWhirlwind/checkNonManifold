#ifndef _UNIFORMGRID_
#define _UNIFORMGRID_

#include <vector>
#include "Common/point3.h"

namespace sn3DCore
{
	class UniformGrid
	{
	public:

		class BoxCoord
		{
		public:
			int x;//空间方格索引
			int y;
			int z;
			int entryIndex;//在空间方格中的存储索引
		};

		class SpaceBoxEntry  //CSpatialSubdivisionEntry表示空间剖分的一个单元格
		{
		public:

			int m_Size;  //该方格中所包含的元素数量          

			int m_BeginIndex;//数据池中起始地址

			double m_percent;

			SpaceBoxEntry() { m_Size = 0; m_BeginIndex = 0; }
		};

	public:

		void Initialize(Point3f * pointData, int num, int scale);

		int GetEntrySize(int x, int y, int z);//获取单元格大小

		Point3f GetMinPoint();//获取最小点

		Point3f GetMaxPoint();//获取最大点

		int  GetEntryItem(int x, int y, int z, int index);//获取单元格对象(index索引)

		void Clear(); //内存清除函数

		UniformGrid(void);

	public:

		std::vector<SpaceBoxEntry> m_SpaceBox;//空间单元格

		std::vector<int> m_DataPool;//数据

		std::vector<int> m_EntryMap;//单元格映射

		Point3f m_MaxCorner;//空间结构的最大点(按x,y,z排序)

		Point3f m_MinCorner;//空间结构的最小点(按x,y,z排序)

		int m_DivisionNum[3];//空间剖分的粒度

		Point3f m_Delta;//为性能而预先计算的m_DivisionNum/(m_MaxCorner-m_MinCorner)
	};
}
#endif