#ifndef _InteractorGl3D_
#define _InteractorGl3D_

#include <windows.h>
#include "SceneGL.h"

/*      Interactor类

该类定义了OpenGL场景交互,主要包括:

1.鼠标消息

2.键盘消息

*/

namespace sn3DCore
{
	class SceneGL3D;
	class InteractorGL3D //: public sn3DInteractor
	{
	public:
		InteractorGL3D();
		bool SetScene(SceneGL3D *pScene);

		// 继承父类接口的方法
		virtual bool OnMouseMove(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio = 1.0);
		virtual bool OnNCMouseMove(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio = 1.0);

		virtual bool OnRButtonDown(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio = 1.0);
		virtual bool OnRButtonDblDown(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio = 1.0);
		virtual bool OnRButtonUp(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio = 1.0);

		virtual bool OnMButtonDown(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio = 1.0);
		virtual bool OnMButtonDblDown(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio = 1.0);
		virtual bool OnMButtonUp(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio = 1.0);

		virtual bool OnLButtonDown(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio = 1.0);
		virtual bool OnLButtonDblDown(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio = 1.0);
		virtual bool OnLButtonUp(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio = 1.0);
		virtual void OnSize(HWND wnd, UINT nType, int X, int Y);
		virtual bool OnKeyDown(HWND wnd, UINT nChar, UINT nRepCnt, UINT nFlags);
		virtual bool OnKeyUp(HWND wnd, UINT nChar, UINT nRepCnt, UINT nFlags);
		virtual bool OnChar(HWND wnd, UINT nChar, UINT nRepCnt, UINT nFlags);
		virtual bool OnMouseWheel(HWND wnd, UINT nFlags, short zDelta, int X, int Y, double DPIRatio = 1.0);
		virtual void PreDraw(SceneGL * scene, int feature);
		virtual void Draw(SceneGL * scene, int feature);
		virtual void SetState(int state);
		virtual bool ClearState();
		virtual void Continue();// 继续场景操作
		virtual void Stop(); // 暂停场景操作
		void SetSelectRadius(int selectRadius);
		void SetRenderState(int state);
	public:
		int m_state; // 场景操作状态
		int m_renderState; // 绘制状态
		int m_lastState;	// 上次操作状态 
		int m_buttonState;
		HWND m_hwnd;
		SceneGL3D *m_pScene;
	};
}
#endif