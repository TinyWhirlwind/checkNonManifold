#ifndef _RAY3_
#define _RAY3_

#include "point3.h"
/**************************************************************************
3D射线模板类,
  存储为两个 Point3:原点和方向,if NORM==true 法线总是单位化的
  表示向量场

***************************************************************************/

template <class RayScalarType, bool NORM=false>
class Ray3
{
public: 

	typedef RayScalarType ScalarType;
	typedef Point3<RayScalarType> PointType;
	typedef Ray3<RayScalarType,NORM> RayType;

public:

    inline const PointType & Origin() const { return _ori;} 
    inline PointType & Origin() { return _ori;} 
    inline const PointType & Direction() const { return _dir;} 
    inline void SetOrigin( const PointType & ori ){_ori=ori; }
    inline void SetDirection( const PointType & dir){_dir=dir; if(NORM) _dir.Normalize();}	
    inline void Set(const PointType & ori,const PointType & dir){SetOrigin(ori); SetDirection(dir);}


    Ray3() {};
    Ray3(const PointType &ori, const PointType &dir) {SetOrigin(ori); SetDirection(dir);}
	
	inline bool operator == ( RayType const & p ) const
	{return _ori==p._ori && _dir==p._dir;}
	inline bool operator != ( RayType const & p ) const
	{return _ori!=p._ori || _dir!=p._dir;}
		 
	// importer for different ray types (with any scalar type or normalization beaviour)
	template <class Q, bool K>
	inline void Import( const Ray3<Q,K> & b )
	{ 
	  _ori.Import(b.Origin());_dir.Import( b.Direction()); 
	  if ((NORM) && (!K)) _dir.Normalize(); 
	}
		
	// constructs a new ray importing it from an existing one
	template <class Q, bool K>
	static RayType Construct( const Ray3<Q,K> & b )
	{ 
	 RayType res; res.Import(b);  return res;
	}	

	Ray3(const Ray3<ScalarType,!NORM > &r) 
	{ Import(r); };
	inline RayType & operator = ( Ray3<ScalarType,!NORM> const &r) 
	{ Import(r); return *this; }

	//======Operation
	inline ScalarType Projection(const PointType &p) const // Projects a point on the ray
	{ 
	  if(NORM) return ScalarType((p-_ori)*_dir); 
	  else     return ScalarType((p-_ori)*_dir/_dir.SquaredNorm()); 
	}
	  
	static bool IsNormalized() {return NORM;}
	  
	inline PointType P( const ScalarType t ) const // calculates the point of parameter t on the ray.
	{ return _ori + _dir * t; }
		
	inline Ray3<ScalarType,true> & Normalize() // normalizes direction field (returns a Normalized Ray)
	{ if (!NORM) _dir.Normalize(); return *((Ray3<ScalarType,true>*)this);}
		
	// normalizes direction field (returns a Normalized Ray) - static version
	static Ray3<ScalarType,true> &Normalize(RayType &p)
	{ p.Normalize(); return *((Ray3<ScalarType,true>*)(&p));}

	PointType ClosestPoint(const PointType & p) const{return P(Projection(p));}
	  
	inline void Flip() {_dir=-_dir;};// flips the ray

	inline Ray3<ScalarType,false> operator + ( RayType const & p) const
	{return Ray3<ScalarType,false> ( _ori+p.Origin(), _dir+p.Direction() );}
	inline Ray3<ScalarType,false> operator - ( RayType const & p) const
	{return Ray3<ScalarType,false> ( _ori-p.Origin(), _dir-p.Direction() );}
	inline Ray3<ScalarType,false> operator * ( const ScalarType s ) const
	{return Ray3<ScalarType,false> ( _ori*s, _dir*s );}
	inline Ray3<ScalarType,false> operator / ( const ScalarType s ) const
	{ScalarType s0=((ScalarType)1.0)/s; return RayType( _ori*s0, _dir*s0 );}

private:

	PointType _ori;// Origin
	PointType _dir;// Direction (not necessarily normalized, unless so specified by NORM)
}; 

typedef Ray3<short>  Ray3s;
typedef Ray3<int>    Ray3i;
typedef Ray3<float>  Ray3f;
typedef Ray3<double> Ray3d;

typedef Ray3<short ,true> Ray3sN;
typedef Ray3<int   ,true> Ray3iN;
typedef Ray3<float ,true> Ray3fN;
typedef Ray3<double,true> Ray3dN;


template <class ScalarType, bool NORM> 
Point3<ScalarType> ClosestPoint(Ray3<ScalarType,NORM> r, const Point3<ScalarType> & p) 
{
	ScalarType t = r.Projection(p); 
	if (t<0) return r.Origin();
	return r.P(t); 
}


#endif
