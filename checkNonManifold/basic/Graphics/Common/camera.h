#ifndef _CAMERA_
#define _CAMERA_

#include "../Geometry/point3.h"
#include "../Geometry/point2.h"
#include "similarity.h"

	enum {
		PERSPECTIVE =	0,
		ORTHO       =	1,
		ISOMETRIC	=   2,
		CAVALIERI	=   3
	};


template<class S>
class Camera
{
public:
	typedef S ScalarType;

	Camera():
		FocalMm(0.f),
		ViewportPx( Point2<int>(0,0)),
		PixelSizeMm( Point2<S>(0.0,0.0)),
		CenterPx( Point2<S>(0.0,0.0)),
		DistorCenterPx( Point2<S>(0.0,0.0)),
		cameraType(0)
		{}

	//------ camera intrinsics
	ScalarType	FocalMm;			/// Focal Distance: the distance between focal center and image plane. Expressed in mm
	Point2<int>	ViewportPx;			/// Dimension of the Image Plane (in pixels)
	Point2<S>	PixelSizeMm;		/// Dimension in mm of a single pixel
	Point2<S>	CenterPx;			/// Position of the projection of the focal center on the image plane. Expressed in pixels

	Point2<S>	DistorCenterPx;		/// Position of the radial distortion center on the image plane in pixels
	S			k[4];				/// 1st & 2nd order radial lens distortion coefficient (only the first 2 terms are used)
	//------------------------

	int cameraType;					/// Type of camera: PERSPECTIVE,ORTHO,ISOMETRIC,CAVALIERI

	void SetOrtho(S dist)
	{ 
		cameraType = ORTHO;
		ViewportPx = ( ((ViewportPx[0] * PixelSizeMm[0]) * (ViewportPx[1] * PixelSizeMm[1])) / FocalMm ) * dist;
	};

	bool IsOrtho() const
	{ 
		return ( cameraType == ORTHO ); 
	}

	//--- Set-up methods

	/// set the camera specifying the perspecive view
	inline void SetPerspective(S AngleDeg, S AspectRatio, S Focal,  Point2<int> Viewport);

	/// set the camera specifying the cavalieri view
	inline void SetCavalieri(S sx, S dx, S bt, S tp, S Focal,  Point2<int> Viewport);

	/// set the camera specifying the isometric view
	inline void SetIsometric(S sx, S dx, S bt, S tp, S Focal,  Point2<int> Viewport);

	/// set the camera specifying the frustum view
	inline void SetFrustum(S dx, S sx, S bt, S tp, S Focal,  Point2<int> Viewport);
	//------------------

	/// returns the frustum
	inline void GetFrustum(S & sx, S & dx, S & bt, S & tp, S & nr);

	//--- Space transformation methods 
	
	/// project a point from 3d CAMERA space to the camera local plane
	inline  Point2<S> Project(const  Point3<S> & p) const;

	/// unproject a point from the camera local plane (plus depth) to 3d CAMERA space 
	inline  Point3<S> UnProject(const  Point2<S> & p, const S & d) const;

	/// transforms local plane coords to vieport (pixel) coords	
	inline  Point2<S> LocalToViewportPx(const  Point2<S> & p) const;
	
	/// transforms vieport (pixel) coords to local plane coords 	
	inline  Point2<S> ViewportPxToLocal(const  Point2<S> & p) const;

	/// transforms local plane coords to [0 1] coords
	inline  Point2<S> LocalTo_0_1(const  Point2<S> & p) const;

	/// transforms local plane coords to [-1 1] coords
	inline  Point2<S> LocalTo_neg1_1(const  Point2<S> & p) const;
	//--------------------------------
};



/// project a point from 3d CAMERA space to the camera's plane]
template<class S>
 Point2<S> Camera<S>::Project(const  Point3<S> & p) const
{
	 Point2<S> q =  Point2<S>(p[0],p[1]);

	if(!IsOrtho())
	{
		q[0] *= FocalMm/p.Z();
		q[1] *= FocalMm/p.Z();
	}

	return q;
}

/// unproject a point from the camera 2d plane [-1,-1]x[1,1] (plus depth) to 3d CAMERA space 
template<class S>
 Point3<S> Camera<S>::UnProject(const  Point2<S> & p, const S & d) const
{
	 Point3<S> np = Point3<S>(p[0], p[1], d);

	if(!IsOrtho())
	{
		np[0] /= FocalMm/d;
		np[1] /= FocalMm/d;
	}

	return np;
}

/// transforms local plane coords to vieport (pixel) coords
template<class S>
 Point2<S> Camera<S>::LocalToViewportPx(const  Point2<S> & p) const
{
	 Point2<S> np;

	np[0] = (p[0] / PixelSizeMm.X()) + CenterPx.X();
	np[1] = (p[1] / PixelSizeMm.Y()) + CenterPx.Y();

	return np;
}

/// transforms vieport (pixel) coords to local plane coords 
template<class S>
 Point2<S> Camera<S>::ViewportPxToLocal(const  Point2<S> & p) const
{
	 Point2<S>  ps;
	ps[0] = (p[0]-CenterPx.X()) * PixelSizeMm.X();
	ps[1] = (p[1]-CenterPx.Y()) * PixelSizeMm.Y();
	return ps;
}

/// transforms local plane coords to [0-1] coords
template<class S>
 Point2<S> Camera<S>::LocalTo_0_1(const  Point2<S> & p) const
{
	 Point2<S>  ps;
	ps[0] = ( p[0]/PixelSizeMm.X() + CenterPx.X() ) / (S)ViewportPx[0];
	ps[1] = ( p[1]/PixelSizeMm.Y() + CenterPx.Y() ) / (S)ViewportPx[1];
	return ps;
}

/// transforms local plane coords to [-1 1] coords
template<class S>
 Point2<S> Camera<S>::LocalTo_neg1_1(const  Point2<S> & p) const
{
	 Point2<S>  ps;
	ps[0] = 2.0f * p[0] / ( PixelSizeMm.X() * (S)ViewportPx[0] );
	ps[1] = 2.0f * p[1] / ( PixelSizeMm.Y() * (S)ViewportPx[1] );
	return ps;
}

//--- basic camera setup (GL-like)

/// set the camera specifying the perspective view
template<class S>
void Camera<S>::SetPerspective(	S AngleDeg, S AspectRatio, S Focal,  Point2<int> Viewport)
{
	cameraType = PERSPECTIVE;
	S halfsize[2];

	halfsize[1] = tan(math::ToRad(AngleDeg/2.0f)) * Focal;	
	halfsize[0] = halfsize[1]*AspectRatio;

	SetFrustum(-halfsize[0],halfsize[0],-halfsize[1],halfsize[1],Focal,Viewport);
}

/// set the camera specifying the frustum view
template<class S>
void Camera<S>::SetFrustum(	S sx, S dx, S bt, S tp, S Focal,  Point2<int> Viewport)
{
	S vp[2];
	vp[0] = dx-sx;
	vp[1] = tp-bt;

	ViewportPx[0] = Viewport[0];
	if(vp[1] != -1)
		ViewportPx[1] = Viewport[1];			// the user specified the viewport
	else
		ViewportPx[1] = ViewportPx[0];	// default viewport

	PixelSizeMm[0] = vp[0] / (S)Viewport[0];
	PixelSizeMm[1] = vp[1] / (S)Viewport[1];

	CenterPx[0] = -sx/vp[0] * (S)Viewport[0];
	CenterPx[1] = -bt/vp[1] * (S)Viewport[1];

	FocalMm =Focal;
}

//--- special cameras setup

/// set the camera specifying the cavalieri view
template<class S>
void Camera<S>::SetCavalieri(S sx, S dx, S bt, S tp, S Focal,  Point2<int> Viewport)
{
	cameraType = CAVALIERI;
	SetFrustum(sx, dx, bt, tp, Focal,Viewport);
}

/// set the camera specifying the isometric view
template<class S>
void Camera<S>::SetIsometric(S sx, S dx, S bt, S tp, S Focal,  Point2<int> Viewport)
{
	cameraType = ISOMETRIC;
	SetFrustum(sx, dx, bt, tp, Focal,Viewport);
}

/// returns the frustum
template<class S>
void Camera<S>:: GetFrustum( S & sx, S & dx, S & bt, S & tp, S & nr)
{
	dx = CenterPx.X()* PixelSizeMm.X();			//scaled center
	sx = -( (S)ViewportPx.X() - CenterPx.X() ) * PixelSizeMm.X();

	bt = -CenterPx.Y()* PixelSizeMm.Y();
	tp = ( (S)ViewportPx.Y() - CenterPx.Y() ) * PixelSizeMm.Y();

	nr = FocalMm;
}


#endif




