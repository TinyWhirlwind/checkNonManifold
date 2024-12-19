//#include"stdafx.h"
#include "SceneObject.h"

#include <string.h>
#include <GL\glew.h>

namespace sn3DCore
{
	SceneObject::SceneObject()
	{
		SetName("Object");
		m_type	= TYPE_OBJECT;
		m_flag	= 0;
		m_index = -1;
	}
	SceneObject::~SceneObject()
	{
		
	}

	void SceneObject::Cylinder(int slices,float lenght,float width)
	{

		Point3f p0;
		Point3f p1;

		glScaled(lenght,width,width);

		float step=6.28f/(float)slices;
		float angle=0;

		//绘制圆柱侧面
		glBegin(GL_TRIANGLE_STRIP);
		for(int b = 0; b <= slices; ++b)
		{
			if(b == slices) angle = 0.0;
			p0 = Point3f( 0, sin(angle),cos(angle));
			p1 = p0; p1[0] = 1.f;
			glNormal3f(p0[0],p0[1],p0[2]);
			glVertex3d(p0[0],p0[1],p0[2]);
			glVertex3d(p1[0],p1[1],p1[2]);
			angle+=step;
		}
		glEnd();

		//绘制圆柱底面
		angle=0;
		p0 = Point3f(0,0,0);

		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(-1,0,0);
		glVertex3d(p0[0],p0[1],p0[2]);
		for(int b = 0; b <= slices-1; ++b)
		{
			glNormal3f(-1,0,0);
			p1 = Point3f( 0, sin(angle),cos(angle));
			glVertex3d(p1[0],p1[1],p1[2]);
			angle+=step;
		}
		glNormal3f(-1,0,0);
		glVertex3d(0,0,1);
		glEnd();

		//绘制圆柱顶面
		angle=6.28f;
		p0 = Point3f(1,0,0);

		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(1,0,0);
		glVertex3d(p0[0],p0[1],p0[2]);
		for(int b = 0; b <= slices-1; ++b)
		{
			glNormal3f(1,0,0);
			p1 = Point3f( 1, sin(angle),cos(angle));
			glVertex3d(p1[0],p1[1],p1[2]);
			angle-=step; //逆时针
		}
		glNormal3f(1,0,0);
		glVertex3d(1,0,1);
		glEnd();

	}	

	void SceneObject::Cone(int slices,float lenght,float width)
	{				
		Point3f p0;//顶点或中心点
		Point3f P[2]; 
		Point3f N[2];

		glScaled(lenght,width,width); //对高度1底面半径为1的圆锥进行缩放

		//绘制侧面
		p0 = Point3f(1.0,0,0);
		N[0]= Point3f( 1.f,sinf(0),cosf(0) ); //没有归一化
		P[0]= Point3f( 0,sinf(0),cosf(0));

		for(int b = 1; b<=slices; ++b)
		{
			float angle = -6.28f*(float)b/(float)slices;//逆时针
			if (b==slices) angle=0;

			N[1] = Point3f(1.f, sinf(angle), cosf(angle));
			P[1] = Point3f(0,sinf(angle), cosf(angle));

			glBegin(GL_TRIANGLES);
			Point3f n =  ( (P[0]-p0) ^ (P[1]-p0) ).Normalize();
			glNormal3f(n[0],n[1],n[2]);
			glVertex3f(p0[0],p0[1],p0[2]);
			glNormal3f(N[0][0],N[0][1],N[0][2]);
			glVertex3f(P[0][0],P[0][1],P[0][2]);
			glNormal3f(N[1][0],N[1][1],N[1][2]);
			glVertex3f(P[1][0],P[1][1],P[1][2]);
			glEnd();

			N[0] = N[1];
			P[0] = P[1];
		}

		//绘制底面
		p0 = Point3f(0,0,0);
		P[0]= Point3f( 0,sinf(0),cosf(0));

		for(int b = 1; b<=slices; ++b)
		{
			float angle = -6.28f*(float)b/(float)slices;
			if (b==slices) angle=0;

			Point3f n= Point3f( -1.f,0,0 );
			P[1] = Point3f(0, sinf(angle), cosf(angle));

			glBegin(GL_TRIANGLES);
			glNormal3f(n[0],n[1],n[2]);
			glVertex3f(P[0][0],P[0][1],P[0][2]);
			glVertex3f(p0[0],p0[1],p0[2]);
			glVertex3f(P[1][0],P[1][1],P[1][2]);
			glEnd();

			P[0] = P[1];
		}
	}	
}
