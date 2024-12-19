#ifndef _BOX2_
#define _BOX2_

#include <assert.h>
#include "base.h"
#include "point2.h"
namespace sn3DCore
{
/*二维包围盒模板类 	BoxScalarType 标量类型*/
template <class BoxScalarType>
class Box2
{
public:
	
	typedef BoxScalarType ScalarType;

    Point2<BoxScalarType> min;//最小坐标点
		
    Point2<BoxScalarType> max;//最大坐标点
	
	inline  Box2() { min.X()= 1; max.X()= -1; min.Y()= 1; max.Y()= -1; }
/*拷贝构造函数*/	
	inline  Box2( const Box2 & b ) { min=b.min; max=b.max; }
	inline ~Box2() { }	
	inline bool operator == ( Box2 const & p ) const
	{
		return min==p.min && max==p.max;
	}
		
	void Set( const Point2<BoxScalarType> & p )
	{
		min = max = p;
	}
/*初始化函数*/
	inline void Set( BoxScalarType minx, BoxScalarType miny, BoxScalarType maxx, BoxScalarType maxy )
	{
		min[0] = minx;
		min[1] = miny;
		max[0] = maxx;
		max[1] = maxy;
	}
/*讲包围盒设为无效*/
	void SetNull()
	{
		 min.X()= 1; max.X()= -1; min.Y()= 1; max.Y()= -1;
	}
/*包围盒加法*/
	void Add( Box2 const & b )
	{
		if(IsNull())
		{
			min=b.min;
			max=b.max;
		}
		else
		{
			if(min.X() > b.min.X()) min.X() = b.min.X();
			if(min.Y() > b.min.Y()) min.Y() = b.min.Y();

			if(max.X() < b.max.X()) max.X() = b.max.X();
			if(max.Y() < b.max.Y()) max.Y() = b.max.Y();
		}
	}
/*加入一个点*/
	void Add( const Point2<BoxScalarType> & p )
	{
		if(IsNull()) Set(p);
		else 
		{
			if(min.X() > p.X()) min.X() = p.X();
			if(min.Y() > p.Y()) min.Y() = p.Y();

			if(max.X() < p.X()) max.X() = p.X();
			if(max.Y() < p.Y()) max.Y() = p.Y();
		}
	}
/*包围盒求交*/
	void Intersect( const Box2 & b )
	{
		if(min.X() < b.min.X()) min.X() = b.min.X();
		if(min.Y() < b.min.Y()) min.Y() = b.min.Y();

		if(max.X() > b.max.X()) max.X() = b.max.X();
		if(max.Y() > b.max.Y()) max.Y() = b.max.Y();

		if(min.X()>max.X() || min.Y()>max.Y()) SetNull();
	}
/*平移包围盒*/
	void Translate( const Point2<BoxScalarType> & p )
	{
		min += p;
		max += p;
	}
/*检查点是否在包围盒内*/
	bool IsIn( Point2<BoxScalarType> const & p ) const
	{
		return (
			min.X() <= p.X() && p.X() <= max.X() &&
			min.Y() <= p.Y() && p.Y() <= max.Y()
		);
	}
/*检查点在包围盒内外*/
	bool IsInEx( Point2<BoxScalarType> const & p ) const
	{
		return  (
			min.X() <= p.X() && p.X() < max.X() &&
			min.Y() <= p.Y() && p.Y() < max.Y() 
		);
	}
/*检查两个包围盒是否碰撞*/
	bool Collide( Box2 const &b )
	{
		Box2 bb=*this;
		bb.Intersect(b);
		return bb.IsValid();
	}
/*检查包围盒是否无效*/
	inline bool IsNull() const { return min.X()>max.X() || min.Y()>max.Y(); }
/*检查包围盒是否有效*/
	inline bool IsValid() const { return min.X()<max.X() && min.Y()<max.Y(); }
/*检查包围盒是否为空*/
	inline bool IsEmpty() const { return min==max; }
/*返回包围盒对角线长度*/
	BoxScalarType Diag() const
	{
		return Distance(min,max);
	}
/*返回包围盒中心*/
	Point2<BoxScalarType> Center() const
	{
		return (min+max)/2;
	}
/*返回包围盒面积*/
	inline BoxScalarType Area() const
	{
		return (max[0]-min[0])*(max[1]-min[1]);
	}
/*返回包围盒X方向长度*/
	inline BoxScalarType DimX() const { return max.X()-min.X(); }
/*返回包围盒Y方向长度*/
	inline BoxScalarType DimY() const { return max.Y()-min.Y(); }

/*返回包围盒对角线向量*/
	inline Point2<BoxScalarType> Dim() const { return max-min; }

	inline void Normalize( Point2<BoxScalarType> & p )
	{
		p -= min;
		p[0] /= max[0]-min[0];
		p[1] /= max[1]-min[1];
	}
}; 
	
typedef Box2<short>  Box2s;
typedef Box2<int>	 Box2i;	
typedef Box2<float>  Box2f;
typedef Box2<double> Box2d;
}
#endif
