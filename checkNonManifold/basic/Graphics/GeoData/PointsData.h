#ifndef _SN3D_POINTS_DATA_
#define _SN3D_POINTS_DATA_

#include "GeometryData.h"
#include "Face.h"

#include <boost/smart_ptr.hpp>

/*  sn3DPointsData
  该类定义了点云数据结构  
*/

namespace sn3DCore
{
   class sn3DPointsData : public  sn3DGeometryData
   {
	public:

	/////////////////////////////  几何数据访问方法    //////////////////////////

		Point3f&P(int index);

		Vertex* V(int index);

		int &   Flag(int index);

		size_t  n_vertices() const;              //   获取点数

		Vertex* AddVertex(const Point3f & point);
		
		void    AddPoint(Point3f point);         //   增加点

		void    AddColor(Color4f & color); // 增加颜色

		void    Clear();	                     //   清除几何数据

		void    Compact();

		void	UpdateVertexMark();

		void	UpdateVertexTexCoord();

	public:
		sn3DPointsData();
		sn3DPointsData(boost::shared_ptr<sn3DPointsData> data);
		virtual ~sn3DPointsData();

	////////////////////  points flag  /////////////////////////

	 bool  IsPointH(int index)			{return (Flag(index) & POINTINVISIBLE)!=0;}	//  隐藏位 
	 void  SetPointH(int index)			{Flag(index) |= POINTINVISIBLE;	}	                     	
	 void  ClearPointH(int index)		{Flag(index) &= (~POINTINVISIBLE);}     

	 bool  IsPointD(int index)			{return (Flag(index) & POINTDELETE)!=0;}	//  隐藏位 
	 void  SetPointD(int index)			{Flag(index) |= POINTDELETE;	}	                     	
	 void  ClearPointD(int index)		{Flag(index) &= (~POINTDELETE);}     


	 ///////// Point flag
	 enum 
	 { 	
	   POINTINVISIBLE     = 0x01,	// 是否隐藏
	   POINTDELETE        = 0x02	// 是否删除
	  };

	public:
		int vn;			//  顶点数
		int m_memSize;		//存储空间大小
	};
}
#endif