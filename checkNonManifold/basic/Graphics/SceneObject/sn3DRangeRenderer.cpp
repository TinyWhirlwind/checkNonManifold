#include "sn3DRangeRenderer.h"

#include <GL\glew.h>
#include <GL\glut.h>
#include <windows.h>

#include "GeoData/RangeData.h"
#include "SceneRangeObj.h"

#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif

namespace sn3DCore
{

	////////////////////////////////////////////////////////////////////////
	//
	//  class CTriangleStrip  该类用来opengl绘制三角带
	//
	////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////


#define  MAX_POINT	1300000

	PFNGLGENBUFFERSARBPROC glGenBuffersARB;
	PFNGLBINDBUFFERARBPROC glBindBufferARB;
	PFNGLBUFFERDATAARBPROC glBufferDataARB;
	PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
	PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;
	PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
	PFNGLMAPBUFFERARBPROC glMapBufferARB;
	PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;

	//////////////////////////////////////////////////////////////////////////
	sn3DRangeRenderer::sn3DRangeRenderer()
	{
		m_refRadius = 4;
		m_model = NULL;

		m_memory = 0;

		// 静态
		m_vn = m_fn		= 0;
		m_vertexBuffer	= 0;
		m_indexBuffer	= 0;
		m_colorBuffer	= 0;
		// 动态
		m_vnDynamic = m_fnDynamic = 0;
		m_vertexBufferDynamic	= 0;
		m_indexBufferDynamic	= 0;
		m_colorBufferDynamic	= 0;
		
		m_bModified	= true;
		m_bVetexColor = false;
		m_bDrawDynamic = false;
		SetGLFucntin();

		return;
	}
	sn3DRangeRenderer::~sn3DRangeRenderer()
	{

	}
	void sn3DRangeRenderer::SetGLFucntin()
	{
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
		glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
		glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
		glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
		glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");

	}
	void sn3DRangeRenderer::Clear()
	{
		SetGLFucntin();
		// 清除静态显示列表
		if( m_vn > 0)
		{
			glDeleteBuffersARB(1, &m_vertexBuffer);
			if( m_bVetexColor)
				glDeleteBuffersARB(1, &m_colorBuffer);
		}
		if( m_fn>0)
			glDeleteBuffersARB(1, &m_indexBuffer);

		m_vn = m_fn = 0;
		m_vertexBuffer	= 0;
		m_indexBuffer	= 0;
		m_colorBuffer	= 0;

		// 清除动态显示列表
		if( m_vnDynamic>0 )
		{
			glDeleteBuffersARB(1, &m_vertexBufferDynamic);
			if( m_bVetexColor)
				glDeleteBuffersARB(1, &m_colorBufferDynamic);
		}
		if( m_fnDynamic>0 )
			glDeleteBuffersARB(1, &m_indexBufferDynamic);
		m_vnDynamic = m_fnDynamic = 0;
		m_vertexBufferDynamic	= 0;
		m_indexBufferDynamic	= 0;
		m_colorBufferDynamic	= 0;

		m_memory = 0;
	}

	void sn3DRangeRenderer::BuildList( )
	{
		SetGLFucntin();
		Clear();
	}

	bool sn3DRangeRenderer::BuildFace(unsigned int &vertexBuf, unsigned int &indexBuf,unsigned int &colorBuf,int detail, int &vn, int &fn)
	{
		SetGLFucntin();
		bool ret = false;
		sn3DRangeData* range = m_model->CloneRange(detail, 0, 0);
		if( range == NULL )
			return ret;
		vn = range->GetVN();
		fn = range->GetFN();

		if(vn>3&&fn>0)
		{
			std::vector<Point3f> &stlPt = range->GetPoints();
			std::vector<Point3f> &stlNm = range->GetNorms();
			std::vector<Triangle> &stlF = range->GetFaces();

			GLfloat * points	= (GLfloat*)(&stlPt[0][0]);
			GLfloat * normals	= (GLfloat*)(&stlNm[0][0]);
			GLuint  * indices	= (GLuint*)(&stlF[0][0]);

			glGenBuffersARB(1,&vertexBuf);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, vertexBuf);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB,sizeof(GLfloat)*vn*3*3, 0, GL_STATIC_DRAW_ARB);
			glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0,						  sizeof(GLfloat)*vn*3, points);
			glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(GLfloat)*vn*3,   sizeof(GLfloat)*vn*3, normals);

			if( m_bVetexColor )
			{
				GLubyte * colors	= new GLubyte[vn*3];
				glGenBuffersARB(1,&colorBuf);
				glBindBufferARB(GL_ARRAY_BUFFER_ARB,	colorBuf);
				glBufferDataARB(GL_ARRAY_BUFFER_ARB,	sizeof(GLubyte)*vn*3, colors, GL_STATIC_DRAW_ARB);
				delete colors;
			}

			glGenBuffersARB(1,&indexBuf);
			glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,indexBuf);
			glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(GLuint)*fn*3,indices,GL_STATIC_DRAW_ARB);
			m_memory += 6*vn;
			ret = true;
		}
		else
		{
			vn=fn = 0;
		}

		delete range;
		return ret;
	}

	bool sn3DRangeRenderer::BuildPoint(unsigned int &vertexBuf, unsigned int &indexBuf,unsigned int &colorBuf,int detail,int &vn)
	{
		SetGLFucntin();
		bool ret = false;
		sn3DRangeData* range = m_model->CloneRange(detail, 0, 0);
		if( range == NULL )
			return ret;
		vn = range->GetVN();
		if( vn>0 )
		{
			std::vector<Point3f> &stlPt	= range->GetPoints();
			std::vector<Point3f> &stlNm	= range->GetNorms();
			GLfloat * points		= (GLfloat*)(&stlPt[0][0]);
			GLfloat * normals		= (GLfloat*)(&stlNm[0][0]);

			glGenBuffersARB(1,&vertexBuf);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB,		vertexBuf);
			glBufferDataARB(GL_ARRAY_BUFFER_ARB,		sizeof(GLfloat)*vn*3*2, 0, GL_STATIC_DRAW_ARB);
			glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,		0,							sizeof(GLfloat)*vn*3,points);
			glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,		sizeof(GLfloat)*vn*3,		sizeof(GLfloat)*vn*3,normals);

			if( m_bVetexColor )
			{
				GLubyte * colors	= new GLubyte[vn*3];
				GetColor( range, colors, vn);
				glGenBuffersARB(1,&colorBuf);
				glBindBufferARB(GL_ARRAY_BUFFER_ARB,	colorBuf);
				glBufferDataARB(GL_ARRAY_BUFFER_ARB,	sizeof(GLubyte)*vn*3, colors, GL_STATIC_DRAW_ARB);
				delete colors;
			}
			ret = true;
			m_memory += vn;
		}

		delete range;
		return ret;
	}

	void sn3DRangeRenderer::DrawFill(unsigned int &vertexBuf, unsigned int &indexBuf,unsigned int &colorBuf,int fn, int vn)
	{
		SetGLFucntin();
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,vertexBuf);         // for vertex coordinates
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,indexBuf);  // for indices

		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY); 

		glVertexPointer(3, GL_FLOAT, 0,0); 
		glNormalPointer(GL_FLOAT, 0,(GLvoid *)(sizeof(GLfloat)*vn*3));

		glDrawElements(GL_TRIANGLES,fn*3, GL_UNSIGNED_INT,0);
		glDisableClientState(GL_VERTEX_ARRAY);   
		glDisableClientState(GL_NORMAL_ARRAY);	

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	}

	void sn3DRangeRenderer::DrawWire(unsigned int &vertexBuf, unsigned int &indexBuf, unsigned int &colorBuf, int fn, int vn)
	{
		// todo:
	}

	void sn3DRangeRenderer::DrawPoints(unsigned int &vertexBuf, unsigned int &indexBuf,unsigned int &colorBuf,int pointSize,int vn)
	{
		SetGLFucntin();
		glPointSize(pointSize);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB,vertexBuf);         // for vertex coordinates
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY); 
		glVertexPointer(3, GL_FLOAT, 0,0); 
		glNormalPointer(GL_FLOAT,  0,(GLvoid *)(sizeof(GLfloat)*vn*3));

		if( m_bVetexColor)
		{
			glEnable(GL_COLOR_MATERIAL);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, colorBuf);
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(3,GL_UNSIGNED_BYTE, 0, 0);
		}
		glDrawArrays(GL_POINTS, 0, vn);
		glDisableClientState(GL_VERTEX_ARRAY);   
		glDisableClientState(GL_NORMAL_ARRAY);	

		if( m_vertexBuffer )
		{
			glDisableClientState(GL_COLOR_ARRAY);
			glDisable(GL_COLOR_MATERIAL);
		}
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	}

	////////////////////////////////////////////////////////////////////////////////
	//// 方法1 vbo
	////////////////////////////////////////////////////////////////////////////////
	bool sn3DRangeRenderer::GetColor(sn3DRangeData *data, unsigned char *color, int vn)
	{
		int n = vn*3;
		float r = 1.0f/n;
		for (int i=0; i<n; i+=3)
		{
			color[i+0] = 255;
			color[i+1] = 0;
			color[i+2] = 0;
		}
		return true;
	}

	void sn3DRangeRenderer::DrawPoints()
	{
	}

	void sn3DRangeRenderer::DrawFill()
	{
		if( m_bDrawDynamic )
		{
			DrawFill(m_vertexBufferDynamic, m_indexBufferDynamic,m_colorBufferDynamic,m_fnDynamic,m_vnDynamic);
		}
		else
		{
			DrawFill(m_vertexBuffer, m_indexBuffer,m_colorBuffer,m_fn,m_vn);
		}
	}

	void sn3DRangeRenderer::DrawWire()
	{
		if (m_bDrawDynamic)
		{
			DrawWire(m_vertexBufferDynamic, m_indexBufferDynamic, m_colorBufferDynamic, m_fnDynamic, m_vnDynamic);
		}
		else
		{
			DrawWire(m_vertexBuffer, m_indexBuffer, m_colorBuffer, m_fn, m_vn);
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// 选择绘制
	void sn3DRangeRenderer::RenderSelected()
	{ 
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT |GL_POINT_BIT );
		glEnable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		glColor4f(0.6f,0.0,0.0,1.0f); 
		glPolygonOffset(-5.5, -5.5);
		glPointSize(2);

		glBegin(GL_POINTS);
		sn3DRangeData* data = m_model->CloneRange(1, 0, 0);
		if( data->HasPoints())
		{
			int h = data->GetH();
			int w = data->GetW();
			// 删除顶点
			for(int i=0; i<h; i++)
			{
				for (int j=0; j<w; j++)
				{
					int id = data->GetMap()[i][j];
					if( id<0 )
						continue;

				}
			}
		}
		glEnd();

		glPopAttrib();
  
		delete data;
	}

	// 标志点绘制
	void sn3DRangeRenderer::RenderRefPoints()
	{
		double radius = m_refRadius;
		int	   slices = 10;
		int	   stacks = 10;

		glPushAttrib (GL_LINE_BIT|GL_TRANSFORM_BIT |GL_ENABLE_BIT | GL_CURRENT_BIT );
		glEnable (GL_LINE_SMOOTH);
		glEnable (GL_BLEND);
		glEnable(GL_COLOR_MATERIAL);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// 标志点是在世界坐标系，需要转换到模型坐标系
		double *R = m_model->m_rotate;
		double *T = m_model->m_trans;
		Point3d p;

		for (int i=0; i<m_model->m_refNum; i++)
		{
			Point3d &pt = m_model->m_refPts[i];
			if (m_model->IsRefpointSelect(i))
			   glColor4f(0.5f,0.3f,0.8f,1.0f);
			else if (m_model->IsRefpointValid(i))
				glColor4f( 0,1.0,1.0,1.0);
			else
				glColor4f(0,0,1.0f,1.0);

			// 标志点乘以模型矩阵的逆
			p.x = R[0]*(pt.x-T[0]) + R[3]*(pt.y-T[1]) + R[6]*(pt.z-T[2]);
			p.y = R[1]*(pt.x-T[0]) + R[4]*(pt.y-T[1]) + R[7]*(pt.z-T[2]);
			p.z = R[2]*(pt.x-T[0]) + R[5]*(pt.y-T[1]) + R[8]*(pt.z-T[2]);
			
			glPushMatrix();
			glTranslated(p.x, p.y, p.z);

			glutSolidSphere(radius,slices, stacks);
			
			glPopMatrix();
		}
		glPopAttrib ();
	}
}