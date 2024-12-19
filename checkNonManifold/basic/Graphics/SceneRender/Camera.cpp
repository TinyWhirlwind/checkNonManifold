//#include "stdafx.h"
#include "Camera.h"
#include <gl/glew.h>

namespace sn3DCore
{
	void Camera::GetView()
	{
		float m[16];
		glGetFloatv(GL_PROJECTION_MATRIX, m);
		proj.Import(Matrix44f(m));
		Transpose(proj);
		glGetFloatv(GL_MODELVIEW_MATRIX, m);
		model.Import(Matrix44f(m));
		Transpose(model);
		glGetIntegerv(GL_VIEWPORT, (GLint*)viewport);
		matrix = proj*model;
		inverse = matrix;
		Invert(inverse);
	}

	void Camera::SetView()
	{
	}

	Point3f Camera::ViewPoint() const
	{
		Matrix44f mi = model;
		Invert(mi);
		return mi* Point3f(0, 0, 0);
	}

	Plane3f Camera::ViewPlaneFromModel(const Point3f &p)//假定P为模型坐标
	{
		Point3f vp = ViewPoint();
		Plane3f pl;  //垂直于视线方向的平面
		pl._dir = (vp - p);
		pl._offset = pl._dir*p;
		return pl;
	}

	Line3f Camera::ViewLineFromModel(const Point3f &p)//假定P为模型坐标
	{
		Point3f vp = ViewPoint();
		Line3f line;
		line.SetOrigin(vp);
		line.SetDirection(p - vp);
		return line;
	}

	Line3f Camera::ViewLineFromWindow(const Point3f &p)//假定P为窗口坐标
	{
		Line3f ln;
		Point3f vp = ViewPoint();
		Point3f pp = UnProject(p);
		ln.SetOrigin(vp);
		ln.SetDirection(pp - vp);
		return ln;
	}

	Point3f Camera::Project(const Point3f &p) const
	{
		Point3f r;
		r = matrix * p;
		return NormDevCoordToWindowCoord(r);
	}

	Point3f Camera::UnProject(const Point3f &p) const
	{
		Point3f s = WindowCoordToNormDevCoord(p);
		s = inverse * s;
		return s;
	}

	Point3f Camera::NormDevCoordToWindowCoord(const Point3f &p) const
	{
		Point3f a;
		a[0] = (p[0] + 1)*(viewport[2] / (float)2.0) + viewport[0];
		a[1] = (p[1] + 1)*(viewport[3] / (float)2.0) + viewport[1];
		//a[1] = viewport[3] - a[1];
		a[2] = (p[2] + 1) / 2;
		return a;
	}

	Point3f Camera::WindowCoordToNormDevCoord(const Point3f &p) const
	{
		Point3f a;
		a[0] = (p[0] - viewport[0]) / (viewport[2] / (float)2.0) - 1;
		a[1] = (p[1] - viewport[1]) / (viewport[3] / (float)2.0) - 1;
		a[2] = 2 * p[2] - 1;
		return a;
	}

}