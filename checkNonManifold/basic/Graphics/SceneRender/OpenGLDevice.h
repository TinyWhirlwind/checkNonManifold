#ifndef _OPENGL_DEVICE_
#define _OPENGL_DEVICE_

#include "Common/Point3.h"
#include <windows.h> 

/*  OpenGL 设备类

该类实现了OpenGL机制相关的信息管理功能
*/

namespace sn3DCore
{
	class OpenGLDevice
	{
	public:

		////////////////////////////   设备操作   /////////////////////////////////

		bool InitializeGL(float width, float height, HDC hDC);  //  初始化设备

		void DestroyGL();                                     //  销毁设备

		BOOL SetWindowPixelFormat(HDC hDC);  //  设备象素格式                   

		BOOL CreateViewGLContext(HDC hDC);   //  创建OpenGL设备


		///////////////////////////    数据访问   /////////////////////////////////

		HGLRC GetContext()                  //  获取绘制环境
		{
			return m_hGLContext;
		}

		void GetMvMatrix(double* matrix);   //  获取模型变换矩阵

		void GetProjMatrix(double* matrix); //  获取投影变换矩阵

		void GetViewport(int * viewport);   //  获取视口信息

		float* GetDepthBuffer()             //  获取深度缓存
		{
			return (float*)m_depthBuffer;
		}

		unsigned int*  GetSelectBuffer()    //  获取选择缓存
		{
			return (unsigned int*)m_selectBuf;
		}


		///////////////////////////////////////////////////////////////////////////////////

	public:
		OpenGLDevice();

		~OpenGLDevice();

	public:

		HWND	     m_hWnd;            //  窗口句柄

		HDC			 m_hDC;             //  绘制资源

		int			 m_iGLPixelIndex;   //  象素格式索引

		HGLRC		 m_hGLContext;      //  设备环境

		float*		 m_depthBuffer;		//  深度缓存

		unsigned int*m_selectBuf;		//  选择缓存

		double		 m_mvMatrix[16];    //  模型变换矩阵

		double		 m_projMatrix[16];	//  投影变换矩阵

		int			 m_viewport[4];     //  视口
	};
}
#endif