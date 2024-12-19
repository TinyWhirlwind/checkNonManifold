#ifndef _SPATIALSUBDIVISION_
#define _SPATIALSUBDIVISION_

#include <vector>
#include "Common\Point3.h"

/***************************************************
CSpatialSubdivision 定义了空间均匀剖分结构，用于对空
间顶点进行排序， 加速STL文件顶点去冗余和拓扑重建操作。
当结构被建立后相当于对空间中的元素进行了排序，加速了
空间元素的访问。
                                       
****************************************************/

 //空间坐标 每个剖分方格的索引(x,y,z)以及在方格内的存储索引
class SpaceCoord
{
	public:
	int x;//空间方格索引
	int y;
	int z;
	int entryIndex;//在空间方格中的存储索引
};

//CSpatialSubdivisionEntry表示空间剖分的一个单元格
class CSpatialSubdivisionEntry 
{
	public:
	int m_Size;  //该方格中所包含的元素数量          
	int * m_Blocks;//存储元素空间
	int m_CurBlockSize;//当前存储块空间大小
	    
	CSpatialSubdivisionEntry() { m_Size = 0; m_Blocks = NULL;}	
};

class CSpatialSubdivision
{
public:
	CSpatialSubdivision(void);	
	void Initialize(Point3f maxPoint,Point3f minPoint,int vertexNum);//初始化空间剖分结构,vertexNum是已知的元素数目
	void Insert(int id, Point3f v);//插入一顶点数据
	void FindNeighborhood(int id,int * & neighbors,int & num);//搜索元素 id 的邻域元素(在同一方格内)
	void Delete(int id);//从空间结构中删除一个元素
	void Clear();//Clear是内存清除函数
	
	int m_DivisionNum;//空间剖分的粒度
	Point3f m_MaxCorner;//空间结构的最大点(按x,y,z排序)
	Point3f m_MinCorner;//空间结构的最小点(按x,y,z排序)
	Point3f m_Delta;//为性能而预先计算的m_DivisionNum/(m_MaxCorner-m_MinCorner)
	
	CSpatialSubdivisionEntry ** table;//空间剖分数据定义为三维数组(线性存储)的指针
	SpaceCoord * m_Elements;//顺序记录了每个元素在空间剖分结构中的存储坐标
    int m_ElementCount;//元素计数
	int * m_NeighborBlock;//邻近元素存储空间
	int m_NeighborBlockSize;//上述存储空间大小
};

#endif