#ifndef _SPACEGRID_
#define _SPACEGRID_


#include <vector>
using namespace std;

#include "Common/Point3.h"
#include "GeoData/MeshData.h"

/* 空间Grid格栅 */
namespace sn3DCore
{
	class SpaceGridEntry
	{
	public:
		int m_Size;
		int m_BeginIndex;
		SpaceGridEntry() { m_Size = 0; m_BeginIndex = NULL; }
	};

	class CSpaceGrid
	{
	public:

		CSpaceGrid(void);
		int GetEntrySize(int x, int y);
		int GetEntryItem(int x, int y, int index);

	public:
		void Initialize(sn3DMeshData * m, int scale);
		int GetSpaceLength();
		Point3f GetMinPoint();
		Point3f GetMaxPoint();
		void Clear();

	public:

		std::vector<SpaceGridEntry> m_SpaceBox;
		std::vector<int> m_DataPool;
		std::vector<int> m_EntryMap;
		int m_DivisionNum;
		Point3f m_MaxCorner;
		Point3f m_MinCorner;
		Point3f m_Delta;
	};
}
#endif