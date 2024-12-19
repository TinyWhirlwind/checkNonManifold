#ifndef _FITTING3_
#define _FITTING3_

#include <vector>
#include "plane3.h"
#include "../math/matrix44.h"
#include "../math/matrix33.h"
#include "../math/lin_algebra.h"


template <class S>
bool PlaneFittingPoints(  std::vector< Point3<S> > & samples,Plane3<S> &p){

	int j;
	Matrix44<S> m;m.SetZero();
	typename std::vector< Point3<S> > ::iterator i;
	
	Point3<S> c; c.Zero();
	for(i = samples.begin(); i != samples.end(); ++i)
		c+=*i;
	c/=samples.size();

	for(i = samples.begin(); i != samples.end(); ++i){	
	Point3<S> p = (*i)-c;
	for(j = 0 ; j < 3;++j)
				*(Point3<S>*)&m[j][0] += p * p[j];
	}

	m[0][3]=	m[1][3]=m[2][3]=0.0;
	m[3][3]= 1.0;
	m[3][0]=	m[3][1]=m[3][2]=0.0;

	int n;
	Matrix44<S> res;
	Point4<S> e;
	Point3<S> d;
	Jacobi(m,e,res,n);

	S mx = fabs(e[0]); int mxi=0;
	for(j=1; j < 3; ++j) if( (fabs(e[j]) < mx) ) {mx=fabs(e[j]); mxi = j;}
	
	d[0]=res[0][mxi];
	d[1]=res[1][mxi];
	d[2]=res[2][mxi];

	p.SetOffset(c*d/d.Norm());
	p.SetDirection(d/d.Norm());

return true;
}

template<class S>
inline double FIT_VExp( const Point3<S> & x, const int i )
{
	assert(i>=0);
	assert(i<4);
	if(i==0) return 1;
	else     return x[i-1];
}

	/** Fitting di piani: trova il piano che meglio approssima
	    l'insieme di punti dato
	 */
template<class S>
bool PlaneFittingPointsOld(  std::vector< Point3<S> > & samples, Plane3<S> & p )
{
	Point3<S> d;

  const int N = 4;
	S P[N][N];		// A = s' . s
	S U[N][N];
	int i,j,k,n;

	n = (int)samples.size();
	if(n<3)
		return false;

	for(i=0;i<N;++i)
	{
		for(j=i;j<N;++j)
		{
			P[i][j] = 0;
			for(k=0;k<n;++k)
				P[i][j] += FIT_VExp(samples[k],i) * FIT_VExp(samples[k],j);
		}
		for(j=0;j<i;++j)
			P[i][j] = P[j][i];
	}
	Matrix44<S> m;
	for(i=0;i<N;++i)
 		for(j=0;j<N;++j)
			m[i][j]=P[i][j];

	for(i=0;i<N;++i)
	{
		U[i][i] = 1.0;
		for(j=0;j<i;++j)
			U[i][j] = 0.0;
		for(j=i+1;j<N;++j)
		{
			if(P[i][i]==0.0)
				return false;

			U[i][j] = P[i][j]/P[i][i];
			for(k=j;k<N;++k)
				P[j][k] -= U[i][j]*P[i][k];
		}
	}
	
 	S norm = Point3<S>(U[1][2]*U[2][3]-U[1][3],-U[2][3],1).Norm();

 	p.SetDirection(Point3<S>(U[1][2]*U[2][3]-U[1][3],-U[2][3],1));
 	p.SetOffset(-(U[0][2]*U[2][3]-U[0][3]+U[0][1]*U[1][3]-U[0][1]*U[1][2]*U[2][3])/norm);
	return true;
}

#endif
