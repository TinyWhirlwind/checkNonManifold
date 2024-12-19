#ifndef _POINT2_
#define _POINT2_

#include <assert.h>
#include <math.h>

template <class P2ScalarType> 
class Point2
{
public:
	
	typedef P2ScalarType ScalarType;
	
/////////////////////////////  构造函数   //////////////////////////

	inline Point2 () {}
	 
	inline Point2 ( const ScalarType nx, const ScalarType ny )
	{_v[0] = nx; _v[1] = ny;}

	inline Point2 ( Point2 const & p)
	{  _v[0]= p._v[0];    _v[1]= p._v[1];}
	
	inline Point2 & operator =( Point2 const & p)
	{_v[0]= p._v[0]; _v[1]= p._v[1];return *this;}
	
	inline void Zero()
	{ _v[0] = 0;_v[1] = 0;}


////////////////////////  数据操作   //////////////////////////////

	inline const ScalarType &X() const {return _v[0];} 

	inline const ScalarType &Y() const {return _v[1];}
	
	inline ScalarType &X() {return _v[0];}
	
	inline ScalarType &Y() {return _v[1];}
  
	inline const ScalarType * V() const
	{return _v;}

	inline ScalarType & V( const int i )
	{assert(i>=0 && i<2);return _v[i];}

	inline const ScalarType & V( const int i ) const
	{assert(i>=0 && i<2);return _v[i];}

	inline const ScalarType & operator [] ( const int i ) const
	{assert(i>=0 && i<2);return _v[i];}

	inline ScalarType & operator [] ( const int i )
	{assert(i>=0 && i<2);return _v[i];}

	
//////////////////////////  数学操作  //////////////////////////////

	inline ScalarType operator * ( Point2 const & p ) const
	{return ( _v[0]*p._v[0] + _v[1]*p._v[1] );}

	inline ScalarType operator ^ ( Point2 const & p ) const
	{return _v[0]*p._v[1] - _v[1]*p._v[0];} 

	inline Point2 operator + ( Point2 const & p) const
	{return Point2<ScalarType>( _v[0]+p._v[0], _v[1]+p._v[1]);}

	inline Point2 operator - ( Point2 const & p) const
	{return Point2<ScalarType>( _v[0]-p._v[0], _v[1]-p._v[1] );}

	inline Point2 operator * ( const ScalarType s ) const
	{return Point2<ScalarType>( _v[0] * s, _v[1] * s );}

	inline Point2 operator / ( const ScalarType s ) const
	{return Point2<ScalarType>( _v[0] / s, _v[1] / s );}

	inline Point2 & operator += ( Point2 const & p)
	{ _v[0] += p._v[0];    _v[1] += p._v[1];
	  return *this;}

	inline Point2 & operator -= ( Point2 const & p)
	{ _v[0] -= p._v[0]; _v[1] -= p._v[1];return *this;}

	inline Point2 & operator *= ( const ScalarType s )
	{_v[0] *= s;    _v[1] *= s;return *this;}
	
	inline Point2 & operator /= ( const ScalarType s )
	{_v[0] /= s; _v[1] /= s;return *this;}
 
	inline ScalarType Norm( void ) const
	{return sqrt( _v[0]*_v[0] + _v[1]*_v[1] );}
	 
	inline ScalarType SquaredNorm( void ) const
	{return ( _v[0]*_v[0] + _v[1]*_v[1] );}

	inline Point2 & Scale( const ScalarType sx, const ScalarType sy )
	{_v[0] *= sx;_v[1] *= sy;return * this;}
	 
	inline Point2 & Normalize( void )
	{
	  ScalarType n = sqrt(_v[0]*_v[0] + _v[1]*_v[1]);
	  if(n>0.0) {_v[0] /= n;	_v[1] /= n; }
	  return *this;}
	
	inline bool operator == ( Point2 const & p ) const
	{return (_v[0]==p._v[0] && _v[1]==p._v[1]);} 
	
	inline bool operator != ( Point2 const & p ) const
	{return ((_v[0]!=p._v[0]) || (_v[1]!=p._v[1]) );}
	 
	inline bool operator <  ( Point2 const & p ) const
	{return	(_v[1]!=p._v[1])?(_v[1]<p._v[1]):(_v[0]<p._v[0]);}
	
	inline bool operator >  ( Point2 const & p ) const
	{return	(_v[1]!=p._v[1])?(_v[1]>p._v[1]):(_v[0]>p._v[0]);}
	
	inline bool operator <= ( Point2 const & p ) const
	{return	(_v[1]!=p._v[1])?(_v[1]< p._v[1]):(_v[0]<=p._v[0]);}

	inline bool operator >= ( Point2 const & p ) const
	{return	(_v[1]!=p._v[1])?(_v[1]> p._v[1]):(_v[0]>=p._v[0]);}

	inline ScalarType Distance( Point2 const & p ) const
	{return Norm(*this-p);}
	
	inline ScalarType SquaredDistance( Point2 const & p ) const
	{return Norm2(*this-p);}	
	
	inline ScalarType Angle() const 
	{return math::Atan2(_v[1],_v[0]);}

	inline Point2 & Cartesian2Polar()
	{
	  ScalarType t = Angle();
	  _v[0] = Norm();
	  _v[1] = t;
	  return *this;
	}

	inline Point2 & Polar2Cartesian()
	{
	  ScalarType l = _v[0];
	  _v[0] = (ScalarType)(l*math::Cos(_v[1]));
	  _v[1] = (ScalarType)(l*math::Sin(_v[1]));
	  return *this;
	}

	inline Point2 & Rotate( const ScalarType rad )
	{
	  ScalarType t = _v[0];
	  ScalarType s = math::Sin(rad);
	  ScalarType c = math::Cos(rad);
	  _v[0] = _v[0]*c - _v[1]*s;
	  _v[1] =   t *s + _v[1]*c;
	  return *this;
	}

	
	inline ScalarType Ext( const int i ) const
	{
	  if(i>=0 && i<2) return _v[i];
	  else            return 0;
	}

	template <class T>
	inline void Import( const Point2<T> & b )
	{_v[0] = b.X(); _v[1] = b.Y();}
	
	template <class T>
	static Point2 Construct( const Point2<T> & b )
	{return Point2(b.X(),b.Y());}

	public:
		union {
			struct { P2ScalarType x, y; };
			struct { P2ScalarType _v[2]; };
		};
protected:
	
	enum {Dimension = 2};
};


//////////////////////////  常用函数  /////////////////////////////////

   template <class T>
   inline T Angle( Point2<T> const & p0, Point2<T> const & p1)
   {return p1.Angle() - p0.Angle();}

   template <class T>
   inline Point2<T> operator - ( Point2<T> const & p)
   {return Point2<T>( -p[0], -p[1] );}

   template <class T>
   inline Point2<T> operator * ( const T s, Point2<T> const & p )
   {return Point2<T>( p[0] * s, p[1] * s  );}

   template <class T>
   inline T Norm( Point2<T> const & p )
   {return p.Norm();}

   template <class T>
   inline T SquaredNorm( Point2<T> const & p )
   {return p.SquaredNorm();}

   template <class T>
   inline Point2<T> & Normalize( Point2<T> & p )
   {return p.Normalize();}

   template <class T>
   inline T Distance( Point2<T> const & p1,Point2<T> const & p2 )
   {return Norm(p1-p2);}

   template <class T>
   inline T SquaredDistance( Point2<T> const & p1,Point2<T> const & p2 )
   {return SquaredNorm(p1-p2);}

///////////////////////////////   类型定义   ////////////////////////////////

typedef Point2<short>  Point2s;
typedef Point2<int>	   Point2i;
typedef Point2<float>  Point2f;
typedef Point2<double> Point2d;

#endif
