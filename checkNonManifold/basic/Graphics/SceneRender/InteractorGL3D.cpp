//#include"stdafx.h"
#include "InteractorGL3D.h"
#include "SceneGL3D.h"
#include "Common/Point2.h"

namespace sn3DCore
{
	InteractorGL3D::InteractorGL3D()
	{
		m_hwnd = NULL;
		m_pScene = NULL;
		//m_state = DEFAULT_OPERATION;
		//m_command=NULL;
		m_buttonState = 0;

	}
	bool InteractorGL3D::SetScene(SceneGL3D *pScene)
	{
		m_pScene = pScene;
		m_hwnd = pScene->m_device.m_hWnd;
		return true;
	}

	// 继承父类接口的方法
	bool InteractorGL3D::OnMouseMove(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio)
	{
		Point2f p;
		if (m_pScene && m_hwnd == wnd)
		{
			p[0] = X;
			p[1] = m_pScene->m_camera.viewport[3] - Y;
			p = p*DPIRatio;
			m_pScene->MouseMove(X, m_pScene->m_camera.viewport[3] - Y);
		}
		return true;
	}

	bool InteractorGL3D::OnNCMouseMove(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio)
	{
		if (m_pScene && m_hwnd == wnd)
		{
		}
		return true;
	}

	bool InteractorGL3D::OnRButtonDown(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio)
	{
		if (m_pScene && m_hwnd == wnd)
		{
		}
		return true;
	}

	bool InteractorGL3D::OnRButtonDblDown(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio)
	{
		if (m_pScene && m_hwnd == wnd)
		{
		}
		return true;
	}

	bool InteractorGL3D::OnRButtonUp(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio)
	{
		if (m_pScene && m_hwnd == wnd)
		{

		}
		return true;
	}

	bool InteractorGL3D::OnMButtonDown(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio)
	{
		if (m_pScene && m_hwnd == wnd)
		{
		}
		return true;
	}

	bool InteractorGL3D::OnMButtonDblDown(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio)
	{
		if (m_pScene && m_hwnd == wnd)
		{
		}
		return true;
	}

	bool InteractorGL3D::OnMButtonUp(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio)
	{
		if (m_pScene && m_hwnd == wnd)
		{
		}
		return true;
	}

	bool InteractorGL3D::OnLButtonDown(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio)
	{
		if (m_pScene && m_hwnd == wnd)
		{
			Point2f p;
			p[0] = X;
			p[1] = m_pScene->m_camera.viewport[3] - Y;//此处转换窗口坐标
			p = p*DPIRatio;
												
			if (nFlags & MK_CONTROL)
				m_pScene->MouseDown(X, m_pScene->m_camera.viewport[3] - Y, 0x0001 | 0x0020);
			else
				m_pScene->MouseDown(X, m_pScene->m_camera.viewport[3] - Y, 0x0001);
					
			if (nFlags & MK_CONTROL)
				m_buttonState = 0x0001 | 0x0020;
			else
				m_buttonState = 0x0001;
		}
		return true;
	}

	bool InteractorGL3D::OnLButtonDblDown(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio)
	{
		if (m_pScene && m_hwnd == wnd)
		{
			Point2f p;
			p[0] = X;
			p[1] = m_pScene->m_camera.viewport[3] - Y;//此处转换窗口坐标
			p = p*DPIRatio;
		}
		return true;
	}

	bool InteractorGL3D::OnLButtonUp(HWND wnd, UINT nFlags, int X, int Y, double DPIRatio)
	{
		if (m_pScene && m_hwnd == wnd)
		{
			Point2f p;
			p[0] = X;
			p[1] = m_pScene->m_camera.viewport[3] - Y;
			p = p*DPIRatio;
				
			m_pScene->MouseUp(X, m_pScene->m_camera.viewport[3] - Y, m_buttonState);
		}
		return true;
	}

	void InteractorGL3D::OnSize(HWND wnd, UINT nType, int X, int Y)
	{
		if (m_pScene && m_hwnd == wnd)
		{
		}
	}

	bool InteractorGL3D::OnKeyDown(HWND wnd, UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (m_pScene && m_hwnd == wnd)
		{

		}
		return true;
	}

	bool InteractorGL3D::OnKeyUp(HWND wnd, UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (m_pScene && m_hwnd == wnd)
		{
		}
		return true;
	}

	bool InteractorGL3D::OnChar(HWND wnd, UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if (m_pScene && m_hwnd == wnd)
		{
		}
		return true;
	}

	bool InteractorGL3D::OnMouseWheel(HWND wnd, UINT nFlags, short zDelta, int X, int Y, double DPIRatio)
	{
		if (m_pScene && m_hwnd == wnd)
		{
			m_pScene->MouseWheel(zDelta / float(120));
		}
		return true;
	}

	void InteractorGL3D::PreDraw(SceneGL * scene, int feature)
	{

	}
	void InteractorGL3D::Draw(SceneGL * scene, int feature)
	{
	}

	void  InteractorGL3D::SetState(int state)
	{
		m_state = state;
	}

	void InteractorGL3D::SetSelectRadius(int selectRadius)
	{
		
	}

	void  InteractorGL3D::SetRenderState(int state)
	{
		m_renderState = state;
	}
	bool  InteractorGL3D::ClearState()
	{
		m_state = 0; //重置状态
		m_renderState = 0;
		m_lastState = 0;

		return true;
	}

	void  InteractorGL3D::Stop()
	{
		if (m_state != 0)
		{
			m_lastState = m_state;
			m_state = 0;
		}
	}

	void  InteractorGL3D::Continue()
	{
		m_state = m_lastState;
	}
}