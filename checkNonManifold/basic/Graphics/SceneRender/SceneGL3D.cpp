#include "SceneGL3D.h"

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <gl/glew.h>

#include "SceneObject/AxisIcon.h"
#include "LightColorGL.h"
#include "SceneObject/sn3DRenderer.h"
#include "SceneParam.h"

#ifndef PI
#define PI M_PI
#endif

namespace sn3DCore
{
	SceneGL3D::SceneGL3D()
	{
		m_viewMode = VIEWPOINT_FRONT;			//默认前视图
		m_backgroundMode = BACKGROUND_DEFAULT;
		m_curObject = 0;
		m_bgColor = Color4f(1.f, 0.f, 0.f, 1.0f);
		noRender = false;
		m_isDragging = false;
		m_materials.resize(57);
		m_ShowBox = false;
		m_ShowAxis = false;
		m_materials[0] = Color4f(0.804, 0.706, 0.247, 1.0);
		m_materials[1] = Color4f(0.541, 0.745, 0.353, 1.0);
		m_materials[2] = Color4f(0.8, 0.266, 0.266, 1.0);
		m_materials[3] = Color4f(0.0, 1.0, 1.0, 1.0);
		m_materials[4] = Color4f(1.0, 0.0, 1.0, 1.0);
		m_materials[5] = Color4f(0.670588f, 0.517647f, 0.823529f, 1.0);
		m_materials[6] = Color4f(0.360784f, 0.811765f, 0.360784f, 1.0);
		m_materials[7] = Color4f(0.835294f, 0.835294f, 0.215686f, 1.0);
		m_materials[8] = Color4f(0.2f, 0.7568627f, 0.7568627f, 1.0);
		m_materials[9] = Color4f(0.188235f, 0.4470588f, 0.7058823f, 1.0);
		m_materials[10] = Color4f(0.5313725f, 0.115686f, 0.5313725f, 1.0);
		m_materials[11] = Color4f(0.96470588, 0.60392156, 0.964705882, 1.0);
		m_materials[12] = Color4f(0.6117647, 0.9803921568, 0.9803921568, 1.0);
		m_materials[13] = Color4f(0.6117647, 0.796078431, 0.98039215686, 1.0);
		m_materials[14] = Color4f(0.9333333, 0.584313725, 0.75686274509, 1.0);
		m_materials[15] = Color4f(0.7803921, 0.960784313, 0.6, 1.0);
		m_materials[16] = Color4f(0.9607843, 0.780392156, 0.6, 1.0);
		m_materials[17] = Color4f(0.6, 0.960784313, 0.780392156, 1.0);
		m_materials[18] = Color4f(0.811764705, 0.623529411, 0.996078431, 1.0);
		m_materials[19] = Color4f(0.584313725, 0.584313725, 0.584313725, 1.0);
		m_materials[20] = Color4f(0.960784313, 0.419607843, 0.419607843, 1.0);
		m_materials[21] = Color4f(0.380392156, 0.866666666, 0.380392156, 1.0);
		m_materials[22] = Color4f(0.407843137, 0.407843137, 0.933333333, 1.0);
		m_materials[23] = Color4f(0.905882352, 0.905882352, 0.227450980, 1.0);
		m_materials[24] = Color4f(0.945098039, 0.235294117, 0.945098039, 1.0);
		m_materials[25] = Color4f(0.235294117, 0.933333333, 0.933333333, 1.0);
		m_materials[26] = Color4f(0.247058823, 0.623529411, 0.996078431, 1.0);
		m_materials[27] = Color4f(0.905882352, 0.227450980, 0.568627450, 1.0);
		m_materials[28] = Color4f(0.568627450, 0.905882352, 0.227450980, 1.0);
		m_materials[29] = Color4f(0.933333333, 0.584313725, 0.235294117, 1.0);
		m_materials[30] = Color4f(0.235294117, 0.949019607, 0.592156862, 1.0);
		m_materials[31] = Color4f(0.568627450, 0.227450980, 0.905882352, 1.0);
		m_materials[32] = Color4f(0.737254901, 0.737254901, 0.737254901, 1.0);
		m_materials[33] = Color4f(0.960784313, 0.690196078, 0.690196078, 1.0);
		m_materials[34] = Color4f(0.698039215, 0.968627450, 0.698039215, 1.0);
		m_materials[35] = Color4f(0.698039215, 0.698039215, 0.968627450, 1.0);
		m_materials[36] = Color4f(0.925490196, 0.925490196, 0.580392156, 1.0);
		m_materials[37] = Color4f(0.980392156, 0.611764705, 0.980392156, 1.0);
		m_materials[38] = Color4f(0.6, 0.960784313, 0.960784313, 1.0);
		m_materials[39] = Color4f(0.623529411, 0.811764705, 0.996078431, 1.0);
		m_materials[40] = Color4f(0.980392156, 0.611764705, 0.796078431, 1.0);
		m_materials[41] = Color4f(0.796078431, 0.980392156, 0.611764705, 1.0);
		m_materials[42] = Color4f(0.960784313, 0.780392156, 0.6, 1.0);
		m_materials[43] = Color4f(0.623529411, 0.996078431, 0.811764705, 1.0);
		m_materials[44] = Color4f(0.780392156, 0.6, 0.960784313, 1.0);
		m_materials[45] = Color4f(0.584313725, 0.584313725, 0.584313725, 1.0);
		m_materials[46] = Color4f(0.933333333, 0.407843137, 0.407843137, 1.0);
		m_materials[47] = Color4f(0.419607843, 0.960784313, 0.419607843, 1.0);
		m_materials[48] = Color4f(0.435294117, 0.435294117, 0.996078431, 1.0);
		m_materials[49] = Color4f(0.909803921, 0.909803921, 0.231372549, 1.0);
		m_materials[50] = Color4f(0.945098039, 0.235294117, 0.945098039, 1.0);
		m_materials[51] = Color4f(0.243137254, 0.968627450, 0.968627450, 1.0);
		m_materials[52] = Color4f(0.231372549, 0.572549019, 0.917647058, 1.0);
		m_materials[53] = Color4f(0.913725490, 0.231372549, 0.572549019, 1.0);
		m_materials[54] = Color4f(0.576470588, 0.917647058, 0.231372549, 1.0);
		m_materials[55] = Color4f(0.945098039, 0.588235294, 0.235294117, 1.0);
		m_materials[56] = Color4f(0.231372549, 0.917647058, 0.576470588, 1.0);
		m_showTexture = false;
		m_drawDepth = false;
	}

	SceneGL3D::~SceneGL3D()
	{

	}

	void SceneGL3D::PaintScene()
	{
		InitSceneParameter();//设置场景参数

		InitViewPoint();     //设置视点位置

		InitSceneCamera();	//设置场景照像机

		LightColorGL::FancyLight();  //设定场景灯光

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();				// 对应的glPopMatrix在PaintMotion中

		m_trackball.Apply(); //施加变换控制

		ScaleSceneModel();   // 缩放场景中的模型至标准范围(CAD模式)

		if (!noRender)
			RenderCustomObject();//绘制自定义CAD数据或模型

		if (m_ShowBox)		// render scene box
			drawSceneBox();

		if (m_ShowAxis)
			drawSceneAxis();
		
		glGetDoublev(GL_MODELVIEW_MATRIX, m_device.m_mvMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, m_device.m_projMatrix);
	}

	void SceneGL3D::PaintStatic()
	{

	}

	void SceneGL3D::PaintMotion()
	{
		glPopMatrix();	//对应PaintScene中的glPushMatrix
	}

	void SceneGL3D::RenderDepthBuffer()
	{
		GLint m_Viewport[4];
		glGetIntegerv(GL_VIEWPORT, m_Viewport);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		std::vector<float> pixels; pixels.resize((int)(m_Viewport[2] * m_Viewport[3]));
		glReadPixels(0, 0, m_Viewport[2], m_Viewport[3], GL_DEPTH_COMPONENT, GL_FLOAT, &pixels[0]);

		int h = m_Viewport[3];
		int w = m_Viewport[2];
		w = (w + 3) >> 2 << 2;

		std::vector<unsigned char> image(h *w * 3);

		for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++)
			{
				unsigned byte = (unsigned)(255 * (pixels[i*m_Viewport[2] + j] - 0.4) / 0.2);
				image[(i*w + j) * 3] = byte;
				image[(i*w + j) * 3 + 1] = byte;
				image[(i*w + j) * 3 + 2] = byte;

			}

		glRasterPos2f(-1, -1);

		glDrawPixels(w, h, GL_BGR, GL_UNSIGNED_BYTE, &image[0]);
	}

	void SceneGL3D::RenderCustomObject()
	{
		// 绘制场景对象
		boost::shared_ptr<SceneObject> obj;
		int cnt = this->GetObjectCount();
		for (int i = 0; i < cnt; i++)
		{
			obj = m_customObjects[i];
			obj->Render();//render object
		}
	}

	void SceneGL3D::RenderCustomEntity()
	{
		std::vector<boost::shared_ptr<SceneObject>>::iterator oIter(m_customEntitys.begin());
		for (; oIter != m_customEntitys.end(); oIter++)
		{
			if (*oIter == NULL) continue;
			(*oIter)->Render();//render object
		}
	}

	void SceneGL3D::Reset()
	{
		m_trackball.Reset();
	}

	void SceneGL3D::InitSceneParameter()
	{
		glEnable(GL_DEPTH_TEST);
		//glClearColor(0.8627f, 0.90588f, 1.f, 1.f);
		glClearColor(SceneParam::Inst().fBgColorR, SceneParam::Inst().fBgColorG, SceneParam::Inst().fBgColorB, 1.0f);
		//glClearColor(0.0078125f*1.0f, 0.5976563f*1.0f, 0.71875f*1.0f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);			//清除深度缓存
		glEnable(GL_NORMALIZE);
		glEnable(GL_CULL_FACE);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	}

	void SceneGL3D::GetRotate(Quaternionf &rot)
	{
		rot.X() = m_trackball.track.rot.X();
		rot.Y() = m_trackball.track.rot.Y();
		rot.Z() = m_trackball.track.rot.Z();
		rot.W() = m_trackball.track.rot.W();
	}

	void SceneGL3D::UpdateSceneBox()
	{
		int end = (int)m_customObjects.size();

		m_sceneBox.SetNull();

		for (int i = 0; i < end; i++)
		{
			if (m_customObjects[i])
			{
				if (m_customObjects[i]->IsH())
					continue;
				boost::shared_ptr<SceneObject> obj = m_customObjects[i];
				m_sceneBox.Add(obj->m_bbox);
			}
		}
	}

	void SceneGL3D::InitSceneCamera()
	{
		//平行投影
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (m_camera.ratio > 1)
			glOrtho(-m_camera.ratio* m_ChangeRatio, m_camera.ratio* m_ChangeRatio, -1, 1,
				m_camera.nearPlane*m_trackball.track.sca, m_camera.farPlane*m_trackball.track.sca);
		else
			glOrtho(-1, 1, -1 / m_camera.ratio * m_ChangeRatio, 1 / m_camera.ratio* m_ChangeRatio,
				m_camera.nearPlane*m_trackball.track.sca, m_camera.farPlane*m_trackball.track.sca);

		//照像机视点变换
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(m_camera.m_Eye.X()*m_trackball.track.sca, m_camera.m_Eye.Y()*m_trackball.track.sca, m_camera.m_Eye.Z()*m_trackball.track.sca*m_trackball.track.sca,
			m_camera.m_Target.X(), m_camera.m_Target.Y(), m_camera.m_Target.Z(),
			m_camera.m_Up.X(), m_camera.m_Up.Y(), m_camera.m_Up.Z());
	}

	void SceneGL3D::InitViewPoint()
	{
		m_camera.m_Eye = SceneParam::Inst().CameraEye;
		m_camera.m_Up = SceneParam::Inst().CameraUp;
	}

	void SceneGL3D::ScaleSceneModel()
	{
		// 缩放到-1 - 1
		int dim = m_sceneBox.MaxDim();
		float scale = 2.f / (m_sceneBox.max[dim] - m_sceneBox.min[dim]);
		glScalef(scale, scale, scale);

		// 包围盒中心作为场景中心
		Point3f sceneCenter = (m_sceneBox.max + m_sceneBox.min)*0.5f;
		glTranslatef(-sceneCenter.X(), -sceneCenter.Y(), -sceneCenter.Z());
	}

	void SceneGL3D::GetLookAt(Point3f& eye, Point3f& center, Point3f& up)
	{
		eye.x = m_camera.m_Eye.X()*m_trackball.track.sca;
		eye.y = m_camera.m_Eye.Y()*m_trackball.track.sca;
		eye.z = m_camera.m_Eye.Z()*m_trackball.track.sca*m_trackball.track.sca;;
		center = m_camera.m_Target;
		up = m_camera.m_Up;
	}

	void SceneGL3D::SetViewMode(VIEWPOINT_TYPE mode)
	{
		m_viewMode = mode;
		float si, co;
		Quaternionf q;
		Quaternionf qx;
		Quaternionf qy;
		Quaternionf qz;

		switch (m_viewMode)
		{
		case VIEWPOINT_FRONT:	//前视图
			m_trackball.Reset();
			si = sin(-M_PI / 4.0);
			co = cos(-M_PI / 4.0);
			qx[0] = co;
			qx[1] = si;
			qx[2] = 0.0;
			qx[3] = 0.0;
			si = sin(M_PI / 4.0);
			co = cos(M_PI / 4.0);
			qy[0] = co;
			qy[1] = 0.0;
			qy[2] = si;
			qy[3] = 0.0;
			q = qy * qx;

			m_trackball.track.rot = q;
			break;

		case VIEWPOINT_TOP:		//顶视图
			m_trackball.Reset();

			si = sin(M_PI / 4.0);
			co = cos(M_PI / 4.0);
			qz[0] = co;
			qz[1] = 0.0;
			qz[2] = 0.0;
			qz[3] = si;
			m_trackball.track.rot = qz;
			break;
		case VIEWPOINT_BOTTOM:	//俯视图
			m_trackball.Reset();

			si = sin(M_PI / 2.0);
			co = cos(M_PI / 2.0);
			qy[0] = co;
			qy[1] = 0.0;
			qy[2] = si;
			qy[3] = 0.0;

			si = sin(-M_PI / 4.0);
			co = cos(-M_PI / 4.0);
			qz[0] = co;
			qz[1] = 0.0;
			qz[2] = 0.0;
			qz[3] = si;
			m_trackball.track.rot = qy * qz;
			break;
		case VIEWPOINT_RIGHT:	//右视图
			m_trackball.Reset();

			si = sin(-M_PI / 4.0);
			co = cos(-M_PI / 4.0);

			q[1] = si;
			q[2] = 0.0;
			q[3] = 0.0;
			q[0] = co;
			m_trackball.track.rot = q;
			break;

		case VIEWPOINT_LEFT:	//左视图
			m_trackball.Reset();

			si = sin(-M_PI / 4.0);
			co = cos(-M_PI / 4.0);

			q[1] = si;
			q[2] = 0.0;
			q[3] = 0.0;
			q[0] = co;
			si = sin(-M_PI / 2.0);
			co = cos(-M_PI / 2.0);
			qz[0] = co;
			qz[1] = 0.0;
			qz[2] = 0.0;
			qz[3] = si;
			m_trackball.track.rot = q * qz;
			break;

		case VIEWPOINT_BACK:	//后视图
			m_trackball.Reset();

			si = sin(M_PI / 2.0);
			co = cos(M_PI / 2.0);

			q[1] = 0.0;
			q[2] = si;
			q[3] = 0.0;
			q[0] = co;
			m_trackball.track.rot = q;
			break;

		}
	}

	void SceneGL3D::InitLightModel()
	{
	}

	void SceneGL3D::EnlargeSceneBox(boost::shared_ptr<SceneObject> obj)
	{
		m_sceneBox.Add(obj->m_bbox);
	}

	void SceneGL3D::MouseDown(int x, int y, /*Button*/ int button)
	{
		m_trackball.MouseDown(x, y, button);
	}

	void SceneGL3D::MouseMove(int x, int y)
	{
		m_trackball.MouseMove(x, y);
	}

	void SceneGL3D::MouseUp(int x, int y, /*Button */ int button)
	{
		m_trackball.MouseUp(x, y, button);
	}

	void SceneGL3D::MouseWheel(float notch)
	{
		m_trackball.MouseWheel(notch);
	}

	void SceneGL3D::SetBackColor(Color4f color)
	{
		m_bgColor = color;
	}

	boost::shared_ptr<SceneObject> SceneGL3D::GetObject(int i)
	{
		if (i >= (int)m_customObjects.size())
			return false;
		else
			return m_customObjects[i];
	}

	void SceneGL3D::AddObject(boost::shared_ptr<SceneObject> model, bool boxupdate)
	{
		if (boxupdate)
		{
			EnlargeSceneBox(model);
		}

		for (int i = 0; i < m_customObjects.size(); ++i)
		{
			m_customObjects[i]->ClearS();
			m_customObjects[i]->ClearShowBox();
		}
		model->SetID(m_customObjects.size());
		m_customObjects.push_back(model);
		model->SetS();
		model->SetShowBox();
		model->ClearH();
	}

	void SceneGL3D::PushObject(boost::shared_ptr<SceneObject> model)
	{
		if (m_customObjects.size() == 0)
			m_sceneBox.SetNull();
		m_customObjects.push_back(model);
		EnlargeSceneBox(model);
	}

	void SceneGL3D::RemoveObject(boost::shared_ptr<SceneObject> model)
	{
		std::vector<boost::shared_ptr<SceneObject>>::iterator it = m_customObjects.begin();
		for (; it != m_customObjects.end(); ++it)
		{
			if (model == (*it))
			{
				m_customObjects.erase(it);
				return;
			}
		}
	}

	boost::shared_ptr<SceneObject> SceneGL3D::GetActiveObject()
	{
		return m_curObject;
	}

	bool SceneGL3D::SetActiveObject(boost::shared_ptr<SceneObject> object)
	{
		int count = GetObjectCount();
		for (int i = 0; i < count; i++)
		{
			GetObject(i)->ClearS();
		}
		m_curObject = object;
		if (m_curObject)
			m_curObject->SetS();
		return true;
	}

	int  SceneGL3D::GetObjectCount()
	{
		return (int)m_customObjects.size();
	}

	void SceneGL3D::ClearScene(void)
	{
		m_customObjects.clear();
		m_curObject = 0;
		m_sceneBox.SetNull();
	}

	bool SceneGL3D::HitObj(int X, int Y)
	{
		long hits = 0;
		Point3f curPoint = Point3f(X, Y, 0);

		double ProjMatrix[16]; //投影矩阵
		GLint Viewport[4]; //当前视口
		glGetIntegerv(GL_VIEWPORT, Viewport);
		glGetDoublev(GL_PROJECTION_MATRIX, ProjMatrix);

		if (curPoint[0] >= 0 && curPoint[0] < Viewport[2] && curPoint[1] >= 0 && curPoint[1] < Viewport[3])
		{
			glReadPixels(curPoint[0], curPoint[1], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &curPoint[2]);
		}
		else return false;

		if (curPoint[2] > 0.99)return false;//如果深度缓存中没有数据返回

		//初始化OpenGL选择模式
		unsigned int * selectBuf = new GLuint[m_customObjects.size() * 4];
		glSelectBuffer((int)m_customObjects.size() * 4, selectBuf);
		glRenderMode(GL_SELECT);//切换选择模式
		glInitNames();//初始化名字栈
		glPushName(-1);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluPickMatrix(curPoint[0], curPoint[1], 6, 6, Viewport);
		glMultMatrixd(ProjMatrix);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		//opengl选择
		std::vector<boost::shared_ptr<SceneObject>>::iterator oIter(m_customObjects.begin());
		int cnt = this->GetObjectCount();
		for (int i = 0; i < cnt; i++)
		{
			glLoadName(i);//命名边界元素
			boost::shared_ptr<SceneObject> obj = this->GetObject(i);
			obj->Render();//render object
		}
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		hits = glRenderMode(GL_RENDER);//切换至绘制模式
		if (hits > 0)
		{
			//GLdouble point[3];
			int index = selectBuf[3];
			SetActiveObject(m_customObjects[index]);
			delete[]selectBuf;
			return true;
		}
		else
		{
			delete[]selectBuf;
			return false;
		}
	}

	void SceneGL3D::setBackGroundMode(BACKGROUND_TYPE type)
	{
		m_backgroundMode = type;
	}

	void SceneGL3D::drawSceneBox()
	{
		glPushAttrib(GL_LIGHTING_BIT | GL_ENABLE_BIT | GL_LINE_BIT);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(2.f);
		glDisable(GL_LIGHTING);
		glColor4f(0.7f, 0.7f, 0.7f, 1.0f);

		sn3DRenderer::DrawBox(m_sceneBox.max, m_sceneBox.min);

		glPopAttrib();
	}

	void SceneGL3D::drawSceneAxis()
	{
		Matrix44<float> r;
		m_trackball.track.rot.ToMatrix(r);
		Transpose(r);

		AxisIcon axis;
		axis.SetRotate(m_trackball.track.rot);
		axis.GLRender(1, 1, 100, 100, &r._a[0]);

	}

	void SceneGL3D::drawBackGroundDefault()
	{
		glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glClearColor(m_bgColor.X(), m_bgColor.Y(), m_bgColor.Z(), 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopAttrib();
	}

	void SceneGL3D::drawBackGroundGradient()
	{
		glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDepthMask(0);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		glBegin(GL_TRIANGLE_STRIP);

		glColor4f(0.51953f, 0.59375f, 0.70703f, 1.0f);		glVertex2f(-1, 1);
		glColor4f(0.51953f, 0.59375f, 0.70703f, 1.0f);		glVertex2f(-1, -1);
		glColor4f(0.51953f, 0.59375f, 0.70703f, 1.0f);		glVertex2f(1, 1);
		glColor4f(0.51953f, 0.59375f, 0.70703f, 1.0f);		glVertex2f(1, -1);

		glEnd();

		glPopMatrix();
		glMatrixMode(GL_PROJECTION_MATRIX);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopAttrib();
	}

	void SceneGL3D::drawBackGroundGradientGrey()
	{
		Color4b ColorBottom;
		Color4b ColorTop;

		ColorBottom = Color4b(64, 64, 128, 1);
		ColorTop = Color4b(0, 0, 0, 1);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(0);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		glBegin(GL_TRIANGLE_STRIP);
		glColor4f(ColorTop._v[0], ColorTop._v[1], ColorTop._v[2], ColorTop._v[3]);				glVertex2f(-1, 1);
		glColor4f(ColorBottom._v[0], ColorBottom._v[1], ColorBottom._v[2], ColorBottom._v[3]);  glVertex2f(-1, -1);
		glColor4f(ColorTop._v[0], ColorTop._v[1], ColorTop._v[2], ColorTop._v[3]);				glVertex2f(1, 1);
		glColor4f(ColorBottom._v[0], ColorBottom._v[1], ColorBottom._v[2], ColorBottom._v[3]);  glVertex2f(1, -1);
		glEnd();

		glPopAttrib();
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	void SceneGL3D::drawBackGroundGrid()
	{
		glPushAttrib(GL_ENABLE_BIT);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		/// 网格背景色
		float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

		/// 绘制网格线
		Point2f size = GetSceneSize();
		float screenRatio = float(size.X()) / float(size.Y()); /*1.f;*/
		float numGrid = 100;
		glBegin(GL_LINES);
		for (int i = 0; i < numGrid; i++)
		{
			if (i % 5 == 0)
			{
				glColor4f(220.0 / 255, 220.0 / 255, 221.0 / 255, 1.0);
			}
			else
			{
				glColor4f(245.0 / 255, 245.0 / 255, 246.0 / 255, 1.0);
			}
			glVertex2f((-1 + (2.0 / numGrid) * i), 1);
			glVertex2f((-1 + (2.0 / numGrid) * i), -1);

			glVertex2f(-1, (1 - (2.0 / numGrid) * i * screenRatio));
			glVertex2f(1, (1 - (2.0 / numGrid) * i * screenRatio));
		}
		glEnd();

		glPopMatrix(); // restore modelview
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopAttrib();
	}

	void SceneGL3D::drawGridPlane()
	{
		glPushAttrib(GL_ENABLE_BIT);

		glDisable(GL_DEPTH_TEST);
		//glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		//设置场景照像机
		InitSceneCamera();

		//应用缩放
		m_trackball.ApplyScale();

		// 场景标准化到-1 - 1
		ScaleSceneModel();

		/// 网格背景色
		float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

		/// 绘制网格线
		// 说明：
		//	1.最小单位 1mm
		//	2.相隔5格子线变粗
		/// 3.一个大格子(粗线格)单位 5mm

		float min = -200.f, max = 200.f;	//范围mm
		float step = 1.f;					//最小格子mm
		int range = (int)(max - min) / step;	//格子数

		// 网格平面Z位置
		float zOrder = m_sceneBox.Center().Z();

		//qDebug("ycx step: %f,  %s", step, __FUNCTION__);
		//qDebug("ycx track.sca: %f . %s",m_trackball.track.sca, __FUNCTION__);

		sn3DCore::Color4f colorThick = sn3DCore::Color4f(1.0f, 0.6f, 0.6f, 0.2f);
		sn3DCore::Color4f colorThin = sn3DCore::Color4f(0.6f, 0.6f, 0.6f, 0.2f);
		float lineWidthThick = 3.f;
		float lineWidthThin = 1.6f;
		
		for (int i = 0; i < range; i++)
		{
			if (i % 5 == 0)
			{
				glLineWidth(lineWidthThick);
				glColor4f(colorThick[0], colorThick[1], colorThick[2], colorThick[3]);
			}
			else
			{
				glLineWidth(lineWidthThin);
				glColor4f(colorThin[0], colorThin[1], colorThin[2], colorThin[3]);
			}

			glBegin(GL_LINES);
			// X向直线
			glVertex3f(min, min + i*step, zOrder);
			glVertex3f(max, min + i*step, zOrder);

			// Y向直线
			glVertex3f(min + i*step, min, zOrder);
			glVertex3f(min + i*step, max, zOrder);
			glEnd();
		}
		
		glPopMatrix(); // restore modelview
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		glPopAttrib();
	}
}