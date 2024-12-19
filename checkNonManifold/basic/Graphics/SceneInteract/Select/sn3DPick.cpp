#include<algorithm>
#include "sn3DPick.h"

#include <windows.h>
#include<GL/gl.h>

namespace sn3DCore
{
	sn3DPick::sn3DPick()
	{
		m_PaintRadius=14;
		m_Pixels=NULL;
		m_SelectBuf = NULL;
		m_IsSelected = false;
		m_IsDragging = false;
		m_Rect.resize(2);
		m_SelectRadius = 30;
	}
	sn3DPick::~sn3DPick()
	{
		if(m_Pixels)
			delete[] m_Pixels;
		if(m_SelectBuf )
			delete[] m_SelectBuf;
	}
	void sn3DPick::PaintVertex(sn3DObject * obj,Quaternionf rot,int flag)
	{
	}
	void sn3DPick::SelectPolygonVertex(sn3DObject * obj,Quaternionf rot,int flag)
	{
	}
	void sn3DPick::SelectRefPolygonVertex(sn3DObject * obj,Quaternionf rot,int flag)
	{
	}
	void sn3DPick::RectVertex(sn3DObject * obj,Quaternionf rot,int flag)
	{	
	}
	void sn3DPick::DrawXORPaintCircle()
	{
		float CircleX[360];
		float CircleY[360];
		for(int i=0;i<360;i++)
		{
			CircleX[i]=(float)(m_SelectRadius*cos(i*3.1415927/180));
			CircleY[i]=(float)(m_SelectRadius*sin(i*3.1415927/180));
		}

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0,m_Viewport[2],0,m_Viewport[3],-1,1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glPushAttrib(GL_LINE_BIT|GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f,0.0,0.0,.6f);
		glPolygonOffset(-1.0, -1);

		glLineWidth(1.5);
		glPushMatrix();
		glTranslatef((GLfloat)m_CurPoint.x,(GLfloat)m_CurPoint.y,(GLfloat)0.99999);
		glBegin(GL_LINE_LOOP);
		for(int i=0;i<360;i++)
			glVertex3f(CircleX[i],CircleY[i],0);
		glEnd();
		glPopMatrix(); 
		glPopAttrib();
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

	}
	void sn3DPick::DrawPolygon()
	{
		int end;
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0,m_Viewport[2],0,m_Viewport[3],-1,1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glPushAttrib(GL_LINE_BIT|GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f,0.0,0.0,.6f);
		glPolygonOffset(-1.0, -1);

		glLineWidth(1.5);
		glPushMatrix();

		end = m_Polygon.size();

		glBegin(GL_LINES);
		for(int i=1;i<end;i++)
		{
			glVertex3f((GLfloat)m_Polygon[i-1].X(),(GLfloat)m_Polygon[i-1].Y(),(GLfloat)0.99999);
			glVertex3f((GLfloat)m_Polygon[i].X(),(GLfloat)m_Polygon[i].Y(),(GLfloat)0.99999);
		}
		glEnd();

		glPopMatrix(); 
		glPopAttrib();
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

	}
	void sn3DPick::DrawRect()
	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0,m_Viewport[2],0,m_Viewport[3],-1,1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glPushAttrib(GL_LINE_BIT|GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f,0.0,0.0,.6f);
		glPolygonOffset(-1.0, -1);

		glLineWidth(1.5);
		glPushMatrix();

		glBegin(GL_LINE_LOOP);

		glVertex3f((GLfloat)m_Rect[0].X(),(GLfloat)m_Rect[0].Y(),(GLfloat)0.99999);
		glVertex3f((GLfloat)m_Rect[1].X(),(GLfloat)m_Rect[0].Y(),(GLfloat)0.99999);
		glVertex3f((GLfloat)m_Rect[1].X(),(GLfloat)m_Rect[1].Y(),(GLfloat)0.99999);
		glVertex3f((GLfloat)m_Rect[0].X(),(GLfloat)m_Rect[1].Y(),(GLfloat)0.99999);

		glEnd();

		glPopMatrix(); 
		glPopAttrib();
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	void sn3DPick::UpdateMatrix()
	{
		glGetIntegerv(GL_VIEWPORT, m_Viewport);

		glGetDoublev (GL_MODELVIEW_MATRIX, m_MvMatrix);

		glGetDoublev (GL_PROJECTION_MATRIX, m_ProjMatrix);

		if (m_Pixels!=0) { delete m_Pixels; }

		m_Pixels= new float[ m_Viewport[2] * m_Viewport[3]];	

		glReadPixels(0,0,m_Viewport[2],m_Viewport[3],GL_DEPTH_COMPONENT,GL_FLOAT,m_Pixels);

	}

	void sn3DPick::Clear()
	{
	}
}