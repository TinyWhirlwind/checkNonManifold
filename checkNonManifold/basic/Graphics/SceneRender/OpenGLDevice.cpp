#include "OpenGLDevice.h"

#include <gl/gl.h>

namespace sn3DCore
{
	OpenGLDevice::OpenGLDevice()
	{
		m_selectBuf = new GLuint[20000];
		m_depthBuffer = NULL;
		m_hGLContext = NULL;
		m_hWnd = NULL;             //  窗口句柄
		m_hDC  = NULL;             //  绘制资源
	}

	OpenGLDevice::~OpenGLDevice()
	{
		DestroyGL();
		if(m_depthBuffer!=NULL) 
			delete[] m_depthBuffer;
		if(m_selectBuf!=NULL)
			delete[] m_selectBuf;
	}

	bool OpenGLDevice::InitializeGL(float width,float height,HDC hDC)
	{

		m_hDC = hDC;
		glShadeModel(GL_SMOOTH);
		glEnable(GL_DEPTH_TEST);
		glPixelStorei(GL_PACK_ROW_LENGTH, 0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glEnable(GL_NORMALIZE);
		static float diffuseColor[]={1.0,1.0,1.0,1.0};
		return true;
	}

	void OpenGLDevice::DestroyGL()
	{
		if(wglGetCurrentContext() != NULL)
			wglMakeCurrent(NULL,NULL);

		if(m_hGLContext != NULL)
		{
			wglDeleteContext(m_hGLContext);
			m_hGLContext = NULL;
		}
	}

	BOOL OpenGLDevice::SetWindowPixelFormat(HDC hDC)
	{
		return TRUE;
	}

	BOOL OpenGLDevice::CreateViewGLContext(HDC hDC)
	{
		m_hGLContext = wglCreateContext(hDC);

		if(m_hGLContext==NULL)
			return FALSE;

		if(wglMakeCurrent(hDC,m_hGLContext)==FALSE)
			return FALSE;

		return TRUE;	
	}

	void OpenGLDevice::GetMvMatrix(double* matrix)
	{	
		for(int i=0;i<16;i++)
		{
			matrix[i] = m_mvMatrix[i];
		}

	}

	void OpenGLDevice::GetProjMatrix(double* matrix)
	{
		for(int i=0;i<16;i++)
		{
			matrix[i] = m_projMatrix[i];
		}
	}

	void OpenGLDevice::GetViewport(int * viewport)
	{
		for(int i=0;i<4;i++)
		{
			viewport[i] = m_viewport[i];
		}

	}
}