#ifndef _SceneGL_
#define _SceneGL_

#include "Camera.h"
#include "LightColorGL.h"
#include "OpenGLDevice.h"

/***********************************************************************
*类 SceneGL：主要封装场景相关OpenGL操作功能                        *
*                                                                      *
* 1 支持OpenGL状态设置、底层功能封装                                   *
* 2 定义绘制接口绘制样式                                               *
* 3 管理OpenGL材质、灯光                                               *
* 4 管理OpenGL照相机                                                   *
***********************************************************************/

namespace sn3DCore
{
	class SceneGL
	{
	public:
		//======场景绘制方法
		void Invalidate();				// 场景重绘 调用PaintBegin（）和 PaintEnd（）
		void PaintBegin();				// 开始绘制 调用PaintScene（）
		virtual void PaintEnd();		// 结束绘制 调用PaintMotion（）
		virtual void PaintScene();		// 场景绘制
		virtual void PaintStatic();		// 背景绘制
		virtual void PaintMotion() {};	// 场景控件绘制
		virtual void Resize(int x, int y, int w, int h);// 缩放窗口
		virtual void UpdateSceneBox();					// 更新场景包围盒  

		//======场景设置方法
		virtual void SetSceneState(int state);							// 设置场景状态
		virtual void InitCamera(int width, int height);					// 初始化照相机
		virtual void SetLightModel();
	
		void CheckShowBox() { m_ShowBox = !m_ShowBox; }					// 是否显示包围盒
		void SetShowBox() { m_ShowBox = true; }
		void ClearShowBox() { m_ShowBox = false; }
		bool GetShowBox() { return m_ShowBox; }

		void SetShowAxis(bool bShow = true) { m_ShowBox = bShow; }
		bool GetShowAxis() { return m_ShowAxis; }

		virtual bool Is3DScene() { return false; };
		//======设备操作
		virtual void Initialize(float width, float height, HWND hWnd);	// 初始化
		virtual void Destroy();											// 销毁设备 
		virtual bool InitializeGL(float width, float height, HDC hDC);	// 初始化设备
		void	GlMakeCurrent();
		HGLRC	GetContext();								//  获取绘制环境
		void	GetMvMatrix(double* matrix);				//  获取模型变换矩阵
		void	GetProjMatrix(double* matrix);				//  获取投影变换矩阵
		void	GetViewport(int * viewport);				//  获取视口信息
		float*	GetDepthBuffer();							//  获取深度缓存
		unsigned int* GetSelectBuffer();					//  获取选择缓存
		OpenGLDevice* GetDevice() { return &m_device; }

		//======构造、析构函数
		SceneGL();
		virtual ~SceneGL();

	public:

		//======场景属性	
		OpenGLDevice    m_device;       //  OpenGL设备
		Camera			m_camera;		//  OpenGL视角照相机
		bool			m_ShowBox;      //  包围盒显示标志
		bool			m_ShowAxis;		//	场景坐标轴显示标志
	};
}
#endif