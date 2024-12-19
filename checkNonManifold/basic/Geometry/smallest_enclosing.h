#pragma once

#include "triangle3.h"
#include "tetra3.h"
#include "sphere3.h"
#include <assert.h>

/*计算最小包围体的函数*/
struct SmallestEnclosing {

	/// computes the smallest enclosing sphere of a triangle
	template <class TriangleType>
		static Sphere3<typename TriangleType::ScalarType>  SphereOfTriangle(const TriangleType & t); 

	/// computes the smallest enclosing sphere of a tetrahedron
	template <class TetraType>
		static Sphere3<typename TetraType::ScalarType>  SphereOfTetra(const TetraType & t); 

	/// computes the smallest enclosing sphere of two spheres
	template <class SphereType>
		static SphereType  SphereOfSpheres( const SphereType & s0,const SphereType & s1); 

	/// computes the smallest enclosing sphere of a container of spheres
	template <class SphereContType>
		static typename SphereContType::value_type  SphereOfSpheres( const SphereContType & t); 
};
/*@}*/

template <class TriangleType>
Sphere3<typename TriangleType::ScalarType> 
SmallestEnclosing::SphereOfTriangle(const TriangleType & t){
	return Sphere3<typename TriangleType::ScalarType>(t.Barycenter(),(t.Barycenter()-t.cP(0)).Norm() );
}

template <class TetraType>
Sphere3<typename TetraType::ScalarType> 
SmallestEnclosing::SphereOfTetra(const TetraType & t){
	return Sphere3<typename TetraType::ScalarType>( t.Barycenter(),( t.Barycenter() - t.cP(0) ).Norm() );
}

template <class SphereType>
  SphereType 
 SmallestEnclosing::
SphereOfSpheres(  const   SphereType  & s0, const  SphereType  & s1)
{
	typename SphereType::ScalarType radius;
	 Point3f center;

	if(s0.Radius()==-1.0) return s1; else if(s1.Radius()==-1.0) return s0;

 	float dst = (s1.Center()-s0.Center()).Norm() ;
	radius = (dst+s1.Radius()+s0.Radius())/2;
	Point3f a=s0.Center();
	Point3f b=s1.Center();
	Point3f dir = (b-a).Normalize();
	a = a - dir*s0.Radius();
	b = b + dir*s1.Radius();
	center = (a+b)/2.0;

	return  SphereType(center,radius);
}

template <typename SphereContType>
typename SphereContType::value_type
 SmallestEnclosing::
SphereOfSpheres(  const SphereContType & spheres)
{
	typename SphereContType::value_type::ScalarType radius;
	typename SphereContType::value_type res;
	typename SphereContType::const_iterator si;

	for(si = spheres.begin(); si != spheres.end(); ++si){
		res  = SphereOfSpheres(res,*si);
	}

	return res;
}
}


