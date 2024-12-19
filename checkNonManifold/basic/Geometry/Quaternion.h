// ==========================================================================
// File name:	Quaternion.h
//
// Copyright (C), 2009-20??, Shining 3D Tech. Co., Ltd.
//
// Description:	
// -Class definition : Quaternion.
//	A quaternion is a point in the unit sphere in four dimension: all
//	rotations in three-dimensional space can be represented by a quaternion
//
// Histroy:		
//
// ============================================================================
#ifndef _Quaternion_H
#define _Quaternion_H

#include "Point3.h"
#include "Point4.h"

#include "Matrix44.h"
#include "Matrix33.h"

namespace sn3DCore
{
	template<class S> 
	class Quaternion: public Point4<S>
	{
	public:

		Quaternion() {_v[0] = _v[1] = _v[2] = _v[3] =  0.0;}
		Quaternion(const S v0, const S v1, const S v2, const S v3): Point4<S>(v0,v1,v2,v3){}	
		Quaternion(const Point4<S> p) : Point4<S>(p)	{}
		Quaternion(const S phi, const Point3<S> &a);

		Quaternion operator*(const S &s) const;
		//Quaternion &operator*=(S d);
		Quaternion operator*(const Quaternion &q) const;
		Quaternion &operator*=(const Quaternion &q);
		void Invert();

		void SetIdentity();

		void FromAxis(const S phi, const Point3<S> &a);
		void ToAxis(S &phi, Point3<S> &a ) const;

		void FromMatrix(Matrix44<S> &m);
		void ToMatrix(Matrix44<S> &m) const;
		void ToMatrix(Matrix33<S> &m) const;

		void ToEulerAngles(S &alpha, S &beta, S &gamma);
		void FromEulerAngles(S alpha, S beta, S gamma);

		Point3<S> Rotate(const Point3<S> vec) const; 
		//duplicated ... because of gcc new confoming to ISO template derived classes
		//do no 'see' parent members (unless explicitly specified) 
		const S & V ( const int i ) const	{ assert(i>=0 && i<4); return Point4<S>::V(i); }
		S & V ( const int i )	{ assert(i>=0 && i<4); return Point4<S>::V(i); }
	};

	template <class S> Quaternion<S> Interpolate(  Quaternion<S>   a,   Quaternion<S>   b, double t);
	template <class S> Quaternion<S> &Invert(Quaternion<S> &q);
	template <class S> Quaternion<S> Inverse(const Quaternion<S> &q);


	//Implementation
	template <class S> 
	void Quaternion<S>::SetIdentity()
	{
		FromAxis(0, Point3<S>(1, 0, 0));
	}


	template <class S> Quaternion<S>::Quaternion(const S phi, const Point3<S> &a) 
	{
		FromAxis(phi, a);
	}


	template <class S> Quaternion<S> Quaternion<S>::operator*(const S &s) const 
	{
		return (Quaternion(V(0)*s,V(1)*s,V(2)*s,V(3)*s));
	}

	template <class S> Quaternion<S> Quaternion<S>::operator*(const Quaternion &q) const 
	{		
		Point3<S> t1(V(1), V(2), V(3));
		Point3<S> t2(q.V(1), q.V(2), q.V(3));    

		S d  = t2 * t1;
		Point3<S> t3 = t1 ^ t2;

		t1 *= q.V(0);
		t2 *= V(0);

		Point3<S> tf = t1 + t2 +t3;

		Quaternion<S> t;
		t.V(0) = V(0) * q.V(0) - d;
		t.V(1) = tf[0];
		t.V(2) = tf[1];
		t.V(3) = tf[2];
		return t;
	}

	template <class S> Quaternion<S> &Quaternion<S>::operator*=(const Quaternion &q) 
	{
		S ww = V(0) * q.V(0) - V(1) * q.V(1) - V(2) * q.V(2) - V(3) * q.V(3);
		S xx = V(0) * q.V(1) + V(1) * q.V(0) + V(2) * q.V(3) - V(3) * q.V(2);
		S yy = V(0) * q.V(2) - V(1) * q.V(3) + V(2) * q.V(0) + V(3) * q.V(1);

		V(0) = ww; 
		V(1) = xx; 
		V(2) = yy;
		V(3) = V(0) * q.V(3) + V(1) * q.V(2) - V(2) * q.V(1) + V(3) * q.V(0);
		return *this;
	}	 

	template <class S> void Quaternion<S>::Invert() 
	{
		V(1)*=-1;
		V(2)*=-1;
		V(3)*=-1;
	}


	template <class S> void Quaternion<S>::FromAxis(const S phi, const Point3<S> &a) 
	{
		Point3<S> b = a;
		b.Normalize();
		S s = Sin(phi/(S(2.0)));

		V(0) = Cos(phi/(S(2.0)));
		V(1) = b[0]*s;
		V(2) = b[1]*s;
		V(3) = b[2]*s;
	}

	template <class S> void Quaternion<S>::ToAxis(S &phi, Point3<S> &a) const 
	{
		S s = math::Asin(V(0))*S(2.0);
		phi = math::Acos(V(0))*S(2.0);

		if(s < 0) 
			phi = - phi;

		a.V(0) = V(1);
		a.V(1) = V(2);
		a.V(2) = V(3);
		a.Normalize();
	}

	template <class S> Point3<S> Quaternion<S>::Rotate(const Point3<S> p) const {
		Quaternion<S> co = *this;
		co.Invert();

		Quaternion<S> tmp(0, p.V(0), p.V(1), p.V(2));		

		tmp = (*this) * tmp * co;
		return 	Point3<S>(tmp.V(1), tmp.V(2), tmp.V(3));
	}

	template <class S> void Quaternion<S>::ToMatrix(Matrix44<S> &m) const	{
		S q00 = V(1)*V(1);
		S q01 = V(1)*V(2);
		S q02 = V(1)*V(3);
		S q03 = V(1)*V(0);
		S q11 = V(2)*V(2);
		S q12 = V(2)*V(3);
		S q13 = V(2)*V(0);
		S q22 = V(3)*V(3);
		S q23 = V(3)*V(0);

		m[0][0] = (S)(1.0-(q11 + q22)*2.0);
		m[1][0] = (S)((q01 - q23)*2.0);
		m[2][0] = (S)((q02 + q13)*2.0);
		m[3][0] = (S)0.0;

		m[0][1] = (S)((q01 + q23)*2.0);
		m[1][1] = (S)(1.0-(q22 + q00)*2.0);
		m[2][1] = (S)((q12 - q03)*2.0);
		m[3][1] = (S)0.0;

		m[0][2] = (S)((q02 - q13)*2.0);
		m[1][2] = (S)((q12 + q03)*2.0);
		m[2][2] = (S)(1.0-(q11 + q00)*2.0);
		m[3][2] = (S)0.0;

		m[0][3] = (S)0.0;
		m[1][3] = (S)0.0;
		m[2][3] = (S)0.0;
		m[3][3] = (S)1.0;
	}

	template <class S> void Quaternion<S>::ToMatrix(Matrix33<S> &m) const	{
		S q00 = V(1)*V(1);
		S q01 = V(1)*V(2);
		S q02 = V(1)*V(3);
		S q03 = V(1)*V(0);
		S q11 = V(2)*V(2);
		S q12 = V(2)*V(3);
		S q13 = V(2)*V(0);
		S q22 = V(3)*V(3);
		S q23 = V(3)*V(0);

		m[0][0] = (S)(1.0-(q11 + q22)*2.0);
		m[1][0] = (S)((q01 - q23)*2.0);
		m[2][0] = (S)((q02 + q13)*2.0);

		m[0][1] = (S)((q01 + q23)*2.0);
		m[1][1] = (S)(1.0-(q22 + q00)*2.0);
		m[2][1] = (S)((q12 - q03)*2.0);

		m[0][2] = (S)((q02 - q13)*2.0);
		m[1][2] = (S)((q12 + q03)*2.0);
		m[2][2] = (S)(1.0-(q11 + q00)*2.0);
	}

	// warning m must be a rotation matrix penalty disaster.
	template <class S> void Quaternion<S>::FromMatrix(Matrix44<S> &m) 
	{		
		S Sc;
		S t = (m.V()[0] + m.V()[5] + m.V()[10] + (S)1.0);	
		if(t > 0) 
		{
			Sc = (S)0.5 / Sqrt(t);
			V(0) = (S)0.25 / Sc;
			V(1) = ( m.V()[9] - m.V()[6] ) * Sc;
			V(2) = ( m.V()[2] - m.V()[8] ) * Sc;
			V(3) = ( m.V()[4] - m.V()[1] ) * Sc;
		}	
		else 
		{	
			if(m.V()[0] > m.V()[5] && m.V()[0] > m.V()[10]) 
			{
				Sc  = Sqrt( (S)1.0 + m.V()[0] - m.V()[5] - m.V()[10] ) * (S)2.0;
				V(1) = (S)0.5 / Sc;
				V(2) = (m.V()[1] + m.V()[4] ) / Sc;
				V(3) = (m.V()[2] + m.V()[8] ) / Sc;
				V(0) = (m.V()[6] + m.V()[9] ) / Sc;	
			}	
			else if( m.V()[5] > m.V()[10]) 
			{
				Sc  = Sqrt( (S)1.0 + m.V()[5] - m.V()[0] - m.V()[10] ) * (S)2.0;
				V(1) = (m.V()[1] + m.V()[4] ) / Sc;
				V(2) = (S)0.5 / Sc;
				V(3) = (m.V()[6] + m.V()[9] ) / Sc;
				V(0) = (m.V()[2] + m.V()[8] ) / Sc;
			}	
			else 
			{
				Sc  = Sqrt( (S)1.0 + m.V()[10] - m.V()[0] - m.V()[5] ) * (S)2.0;
				V(1) = (m.V()[2] + m.V()[8] ) / Sc;
				V(2) = (m.V()[6] + m.V()[9] ) / Sc;
				V(3) = (S)0.5 / Sc;
				V(0) = (m.V()[1] + m.V()[4] ) / Sc;
			}
		}
	}

	template<class S>
	void Quaternion<S>::ToEulerAngles(S &alpha, S &beta, S &gamma)
	{
		//...TODO...
	}

	template<class S>
	void Quaternion<S>::FromEulerAngles(S alpha, S beta, S gamma)
	{
		S cosalpha = cos(alpha / 2.0);
		S cosbeta = cos(beta / 2.0);
		S cosgamma = cos(gamma / 2.0);
		S sinalpha = sin(alpha / 2.0);
		S sinbeta = sin(beta / 2.0);
		S singamma = sin(gamma / 2.0);

		V(0) = cosalpha * cosbeta * cosgamma + sinalpha * sinbeta * singamma;
		V(1) = sinalpha * cosbeta * cosgamma - cosalpha * sinbeta * singamma;
		V(2) = cosalpha * sinbeta * cosgamma + sinalpha * cosbeta * singamma;
		V(3) = cosalpha * cosbeta * singamma - sinalpha * sinbeta * cosgamma;
	}

	template <class S> Quaternion<S> &Invert(Quaternion<S> &m) {
		m.Invert();
		return m;
	}

	template <class S> Quaternion<S> Inverse(const Quaternion<S> &m) {
		Quaternion<S> a = m;
		a.Invert();
		return a;
	}

	template <class S> Quaternion<S> Interpolate(   Quaternion<S>   a ,    Quaternion<S>   b , double t) {

		double v = a.V(0) * b.V(0) + a.V(1) * b.V(1) + a.V(2) * b.V(2) + a.V(3) * b.V(3);
		double phi = math::Acos(v);
		if(phi > 0.01) {
			a = a  * (math::Sin(phi *(1-t))/math::Sin(phi));
			b = b  * (math::Sin(phi * t)/math::Sin(phi));	
		}

		Quaternion<S> c;
		c.V(0) = a.V(0) + b.V(0);
		c.V(1) = a.V(1) + b.V(1);
		c.V(2) = a.V(2) + b.V(2);
		c.V(3) = a.V(3) + b.V(3);

		if(v < -0.999) 
		{ //almost opposite
			double d = t * (1 - t);
			if(c.V(0) == 0)
				c.V(0) += d;
			else
				c.V(1) += d;		
		}
		c.Normalize();
		return c;
	}

	template <class S> 
	void  QuaternionMultiply(Quaternion<S> & q,Quaternion<S> q1,Quaternion<S> q2)
	{
		q.W() = q1.W()*q2.W() - q1.X()*q2.X() - q1.Y()*q2.Y() - q1.Z()*q2.Z();
		q.X() = q1.Y()*q2.Z() - q1.Z()*q2.Y() + q1.X()*q2.W()  + q1.W()*q2.X();
		q.Y() = q1.Z()*q2.X() - q1.X()*q2.Z() + q1.Y()*q2.W()  + q1.W()*q2.Y();
		q.Z() = q1.X()*q2.Y() - q1.Y()*q2.X() + q1.Z()*q2.W()  + q1.W()*q2.Z();
	}

	template <class S> 
	void  GetMatrixFromQuaternion(Quaternion<S> q,float * matrix)
	{
		//计算局部坐标架总旋转矩阵：四元数导出旋转矩阵
		float q00 = q.X()*q.X();
		float q01 = q.X()*q.Y();
		float q02 = q.X()*q.Z();
		float q03 = q.X()*q.W();
		float q11 = q.Y()*q.Y();
		float q12 = q.Y()*q.Z();
		float q13 = q.Y()*q.W();
		float q22 = q.Z()*q.Z();
		float q23 = q.Z()*q.W();

		matrix[0]  = (float)(1.0-(q11 + q22)*2.0);//OpenGL方式存储
		matrix[4]  = (float)((q01 - q23)*2.0);
		matrix[8]  = (float)((q02 + q13)*2.0);
		matrix[12] = (float)0.0;

		matrix[1]  = (float)((q01 + q23)*2.0);
		matrix[5]  = (float)(1.0-(q22 + q00)*2.0);
		matrix[9]  = (float)((q12 - q03)*2.0);
		matrix[13] = (float)0.0; 

		matrix[2]  = (float)((q02 - q13)*2.0);
		matrix[6]  = (float)((q12 + q03)*2.0);
		matrix[10] = (float)(1.0-(q11 + q00)*2.0);
		matrix[14] = (float)0.0;

		matrix[3]  = (float)0.0;
		matrix[7]  = (float)0.0;
		matrix[11] = (float)0.0;
		matrix[15] = (float)1.0;


	}

	typedef Quaternion<float>  Quaternionf;
	typedef Quaternion<double> Quaterniond;
}
#endif
