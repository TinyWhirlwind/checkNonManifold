//#include "stdafx.h"
#include "SceneGL2D.h"

#include <gl\glaux.h>

namespace sn3DCore
{

	sn3DSceneGL2D::sn3DSceneGL2D()
	{
	}
	sn3DSceneGL2D::~sn3DSceneGL2D()
	{
	}
	void sn3DSceneGL2D::Initialize(float width, float height, HWND hWnd)
	{
		if (!hWnd)
			return;
		m_device.m_hWnd = hWnd;
		m_device.m_hDC = ::GetDC(hWnd);

		m_device.InitializeGL(width, height, m_device.m_hDC);
		InitCamera((int)width, (int)height);
	}
	void sn3DSceneGL2D::InitCamera(int width, int height)
	{
		m_camera.fov = 60;
		m_camera.clipRatioFar = 1;
		m_camera.clipRatioNear = 1;
		m_camera.nearPlane = .2f;
		m_camera.farPlane = 1500.f;
		m_camera.viewport[0] = 0;
		m_camera.viewport[1] = 0;
		m_camera.viewport[2] = width;
		m_camera.viewport[3] = height;
		m_camera.ratio = (float)width / (float)height;
		m_camera.m_Eye.X() = 0;
		m_camera.m_Eye.Y() = 0;
		m_camera.m_Eye.Z() = 2;
		m_camera.m_Up.X() = 0;
		m_camera.m_Up.Y() = 1;
		m_camera.m_Up.Z() = 0;
		m_camera.m_Target.X() = 0;
		m_camera.m_Target.Y() = 0;
		m_camera.m_Target.Z() = 0;
	}
	void sn3DSceneGL2D::Resize(int x, int y, int _width, int _height)
	{
		GlMakeCurrent();

		glViewport(x, y, _width, _height);
		//分配深度缓存
		if (m_device.m_depthBuffer != NULL)
		{
			delete[] m_device.m_depthBuffer;
			m_device.m_depthBuffer = NULL;
		}
		if (_width*_height > 0)
			m_device.m_depthBuffer = new GLfloat[_width*_height]; //分配深度缓存空间

		m_camera.viewport[2] = _width;
		m_camera.viewport[3] = _height;
		m_camera.ratio = (float)_width / (float)_height;
	}



	void sn3DSceneGL2D::PaintStatic()
	{
		//  绘制背景
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(0);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLE_STRIP);

		glColor4f(0.51953f, 0.59375f, 0.70703f, 1.0f);      glVertex2f(-1, 1);
		glColor4f(0.51953f, 0.59375f, 0.70703f, 1.0f);  glVertex2f(-1, -1);
		glColor4f(0.51953f, 0.59375f, 0.70703f, 1.0f);      glVertex2f(1, 1);
		glColor4f(0.51953f, 0.59375f, 0.70703f, 1.0f);  glVertex2f(1, -1);

		glEnd();
		glPopAttrib();

		glClearColor(0.68235f, 0.8196f, 1.0f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	}

	void sn3DSceneGL2D::PaintScene()
	{
		InitSceneParameter();//设置场景参数

		InitViewPoint();     //设置视点位置

		InitSceneCamera();	//设置场景照像机

		LightColorGL::FancyLight();  //设定场景灯光

		glPushMatrix();

		m_trackball.Apply(); //施加变换控制

		glGetDoublev(GL_MODELVIEW_MATRIX, m_device.m_mvMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, m_device.m_projMatrix);
	}

	void sn3DSceneGL2D::PaintMotion()
	{
		glPopMatrix();

		Matrix44<float> r;
		m_trackball.track.rot.ToMatrix(r);
		Transpose(r);
	}

	void sn3DSceneGL2D::InitSceneCamera()
	{

		//平行投影
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (m_camera.ratio > 1)
			glOrtho(-m_camera.ratio, m_camera.ratio, -1, 1,
				m_camera.nearPlane*m_trackball.track.sca, m_camera.farPlane*m_trackball.track.sca);
		else
			glOrtho(-1, 1, -1 / m_camera.ratio, 1 / m_camera.ratio,
				m_camera.nearPlane*m_trackball.track.sca, m_camera.farPlane*m_trackball.track.sca);
		//照像机视点变换
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(m_camera.m_Eye.X()*m_trackball.track.sca, m_camera.m_Eye.Y()*m_trackball.track.sca, m_camera.m_Eye.Z()*m_trackball.track.sca,
			m_camera.m_Target.X(), m_camera.m_Target.Y(), m_camera.m_Target.Z(),
			m_camera.m_Up.X(), m_camera.m_Up.Y(), m_camera.m_Up.Z());
	}

	void sn3DSceneGL2D::MouseDown(int x, int y, /*Button*/ int button)
	{
		m_trackball.MouseDown(x, y, button);
	}

	void sn3DSceneGL2D::MouseMove(int x, int y)
	{
		m_trackball.MouseMove(x, y);
	}

	void sn3DSceneGL2D::MouseUp(int x, int y, /*Button */ int button)
	{
		m_trackball.MouseUp(x, y, button);
	}

	void sn3DSceneGL2D::MouseWheel(float notch)
	{
		m_trackball.MouseWheel(notch);
	}

	void sn3DSceneGL2D::InitViewPoint()
	{
		m_camera.m_Eye.X() = 0.0;
		m_camera.m_Eye.Y() = 0.0;
		m_camera.m_Eye.Z() = 70.0;
		m_camera.m_Up.X() = 0.0;
		m_camera.m_Up.Y() = 1.0;
		m_camera.m_Up.Z() = 0.0;
	}

	void sn3DSceneGL2D::ScaleSceneModel()
	{
		float scale = 2.0f / m_sceneBox.Diag();
		glScalef(scale, scale, scale);
		Point3f sceneCenter = (m_sceneBox.max + m_sceneBox.min) / 2;
		glTranslatef(-sceneCenter.X(), -sceneCenter.Y(), -sceneCenter.Z());
	}

	void sn3DSceneGL2D::InitSceneParameter()
	{
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);		//清除深度缓存
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		glDisable(GL_CULL_FACE);
	}
}
