#ifndef _SN3D_SCENEGL2D_
#define _SN3D_SCENEGL2D_

#include "SceneGL.h" 
#include "SceneInteract/Navigation/Trackball.h" 

namespace sn3DCore
{
	class sn3DSceneGL2D : public SceneGL
	{
	public:

		/////////////////  绘制方法   ///////////////

		virtual void PaintStatic();

		virtual void PaintScene();

		virtual void PaintMotion();

		void Initialize(float width, float height, HWND hWnd);

		void Destroy() {};

		virtual void Resize(int x, int y, int w, int h);
		virtual void UpdateSceneBox() {}
		virtual int  GetCount() { return 0; };

		virtual void MouseDown(int x, int y, int button); //处理场景交互
		virtual void MouseMove(int x, int y);
		virtual void MouseUp(int x, int y, int button);
		virtual void MouseWheel(float notch);

	public:
		void InitSceneParameter();// 初始化场景参数设置
		void InitCamera(int width, int height); //  初始化照相机
		void InitSceneCamera();    //设置场景照像机
		void InitViewPoint();      //设置视点位置
		void ScaleSceneModel();    // 缩放场景中的模型至标准范围(CAD模式)
	public:

		sn3DSceneGL2D();
		virtual ~sn3DSceneGL2D();

		Box3f			m_sceneBox;		   // 场景自定义模型包围盒
		Trackball		m_trackball;	   // ARCBALL
	};
}

#endif