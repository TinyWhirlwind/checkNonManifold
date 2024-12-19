#include "SceneGL.h"

#include <gl/GL.h>
#include "SceneParam.h"

namespace sn3DCore
{
	SceneGL::SceneGL()
	{
	}
	SceneGL::~SceneGL()
	{
		Destroy();	
	}
	void SceneGL::Initialize(float width,float height,HWND hWnd)
	{
		m_device.m_hWnd = hWnd;
		m_device.InitializeGL(width,height,GetDC(hWnd));
		InitCamera((int)width,(int)height);
	}
	void SceneGL::InitCamera(int width,int height)
	{
		m_camera.fov			= SceneParam::Inst().CameraFov;
		m_camera.clipRatioFar	= SceneParam::Inst().CameraClipRatioFar;
		m_camera.clipRatioNear	= SceneParam::Inst().CameraClipRatioNear;
		m_camera.nearPlane		= SceneParam::Inst().CameraNearPlane;//-500.f;
		m_camera.farPlane		= SceneParam::Inst().CameraFarPlane;
		m_camera.viewport[0]	= 0.f;
		m_camera.viewport[1]	= 0.f;
		m_camera.viewport[2]	= width;
		m_camera.viewport[3]	= height;
		m_camera.ratio			= (float) width/(float) height;
		m_camera.m_Eye.X()		= SceneParam::Inst().CameraEye.x;
		m_camera.m_Eye.Y()		= SceneParam::Inst().CameraEye.y;
		m_camera.m_Eye.Z()		= SceneParam::Inst().CameraEye.z;
		m_camera.m_Up.X()		= SceneParam::Inst().CameraUp.x;
		m_camera.m_Up.Y()		= SceneParam::Inst().CameraUp.y;
		m_camera.m_Up.Z()		= SceneParam::Inst().CameraUp.z;
		m_camera.m_Target.X()	= 0;
		m_camera.m_Target.Y()	= 0;
		m_camera.m_Target.Z()	= 0;
	}
	void SceneGL::Resize(int x,int y,int _width,int _height)
	{ 
		glViewport(x,y,_width,_height);

		if(m_device.m_depthBuffer!=NULL)
		{
			delete[] m_device.m_depthBuffer;
			m_device.m_depthBuffer = NULL;
		}
		if(_width*_height>0)
			m_device.m_depthBuffer = new GLfloat[_width*_height]; //分配深度缓存空间

		m_camera.viewport[2]=_width;
		m_camera.viewport[3]=_height;
		m_camera.ratio = (float)_width /(float)_height;
	}

	void SceneGL::GlMakeCurrent()
	{
		wglMakeCurrent(m_device.m_hDC, m_device.GetContext());
	}

	void SceneGL::PaintBegin()
	{
		PaintStatic(); // 背景绘制
		PaintScene();  // 场景绘制
	}

	void SceneGL::PaintEnd()
	{
		PaintMotion(); // 场景控件绘制
		glFinish();
	}
	void SceneGL::Invalidate()
	{
		PaintBegin();
		PaintEnd();
	}

	void SceneGL::PaintStatic()
	{
	}

	void SceneGL::PaintScene()
	{
	};

	bool SceneGL::InitializeGL(float width,float height,HDC hDC)
	{

		if(m_device.SetWindowPixelFormat(hDC)==FALSE)
			return false;

		if(m_device.CreateViewGLContext(hDC)==FALSE)
			return false;

		glShadeModel(GL_SMOOTH);
		glEnable(GL_DEPTH_TEST);
		glPixelStorei(GL_PACK_ROW_LENGTH, 0);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_NORMALIZE);
		static float diffuseColor[]={1.0,1.0,1.0,1.0};
		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glLightfv(GL_LIGHT1,GL_DIFFUSE,diffuseColor);

		return true;
	}

	void SceneGL::UpdateSceneBox()
	{
	}

	void SceneGL::SetSceneState(int state)
	{
	}

	void SceneGL::Destroy()
	{
	}

	void SceneGL::SetLightModel()
	{
	}

	HGLRC SceneGL::GetContext()
	{
		return m_device.GetContext();
	}

	float* SceneGL::GetDepthBuffer()
	{
		return m_device.GetDepthBuffer();
	}

	unsigned int* SceneGL::GetSelectBuffer()
	{
		return m_device.GetSelectBuffer();
	}

	void SceneGL::GetMvMatrix(double* matrix)
	{
		m_device.GetMvMatrix(matrix);
	}

	void SceneGL::GetProjMatrix(double* matrix)
	{
		m_device.GetProjMatrix(matrix);	
	}

	void SceneGL::GetViewport(int * viewport)
	{
		m_device.GetViewport(viewport);
	}
}