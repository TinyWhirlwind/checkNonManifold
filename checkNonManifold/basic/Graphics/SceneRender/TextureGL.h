#ifndef _TextureGL_
#define _TextureGL_

/*  TextureGL类

	OpenGL纹理实现
*/

#include <vector>
#include <windows.h>
//#include <gl/GL.h>
#include <gl/glew.h>
#include "Common/Point2.h"
#include "Common/Point3.h"

namespace sn3DCore
{
	class TextureGL
	{
	public:

		void BuildTexture();   //  构建纹理(opengl操作,调用后方能使用纹理)

		int  GetWidth();       //  获取图像宽度

		const int GetWidth()const;

		int  GetHeight();      //  获取图像高度

		const int GetHeight() const;

		int  GetChannel();     //  获取图像通道数

		void SetChannel(int channel);

		void SetImage(int w, int h, int channel, void * data);  //设置图像

		void * GetImageData();  //  获取图像数据

		const void * GetImageData()const;


		//////////////////////    标志位     //////////////////////

		bool  IsRenderTexture() { return(m_flag & RENDERTEXTURE) != 0; } // 是否绘制纹理
		void  SetRenderTexture() { m_flag |= RENDERTEXTURE; }
		void  ClearRenderTecture() { m_flag &= (~RENDERTEXTURE); }

		bool  IsHasTexture() { return(m_flag & HASTEXTURE) != 0; }       // 是否有纹理
		void  SetHasTexture() { m_flag |= HASTEXTURE; }
		void  ClearHasTexture() { m_flag &= (~HASTEXTURE); }

		bool  IsColorImage() { return(m_flag & COLORIMAGE) != 0; }	   // 彩图位
		void  SetColorImage() { m_flag |= COLORIMAGE; }
		void  ClearColorImage() { m_flag &= (~COLORIMAGE); }

		//////////////////////////////////////////////////////////


	public:

		TextureGL();

	public:

		enum
		{
			RENDERTEXTURE = 0x01,  //  是否绘制纹理
			HASTEXTURE = 0x02,  //  是否有纹理
			COLORIMAGE = 0x04,  //  彩图位
		};

		int		m_flag;          //  标志位

		GLuint m_TextureId;        //  openGL纹理

		unsigned char * m_Data;    //  图像数据

		int m_w;                   //  图像宽度

		int m_h;                   //  图像高度

		int m_channel;             //  图像通道

		Point2f m_MaxCoord;

		Point2f m_MinCoord;

		//sn3DMeshData *  m_model;

	  /////////////////////////  以下 不再使用 ////////////////////////////////////

		//void BuildTopology(){}

		int m_Height;
		int m_Width;
		Point3f m_Center;
		Point3f m_Dir;
		std::vector<Point3f> m_RangeCoord;
		std::vector<int> m_RangeIndexI;
		std::vector<int> m_RangeIndexJ;
		std::vector<int> m_RangeMap;
		std::vector<int> m_Face0;
		std::vector<int> m_Face1;
		std::vector<int> m_Face2;
	};
}
#endif