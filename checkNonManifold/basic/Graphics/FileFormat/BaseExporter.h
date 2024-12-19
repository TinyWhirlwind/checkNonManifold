#ifndef _BASE_EXPORTER_
#define _BASE_EXPORTER_

#include <vector>
#include "Common/point2.h"
#include "Common/point3.h"
#include "Common/point4.h"


namespace sn3DCore
{
	class BaseExporter
	{
	public:

		// 获取点数据
		virtual Point3f  point(int index)          {return Point3f(0.0,0.0,0.0);}
		virtual Point3f  VertexNormal(int index)   {return Point3f(0.0,0.0,0.0);}
		virtual Point3f  VertexColor(int index)    {return Point3f(0.0,0.0,0.0);}
		virtual Point4f  VertexColorA(int index)   {return Point4f(0.0,0.0,0.0,0.0);}
		virtual Point2f  VertexTexcoord(int index) {return Point2f(0.0,0.0);}

		// 获取面数据
		virtual unsigned int GetVertices(int _findex,std::vector<int>& _vIndexs){return 0;}

		virtual bool IsMultPolygon(int _findex){return false;}

		virtual void GetTextureImage(unsigned char ** data,int & w,int & h,int & channel)
		{
         data = 0;w = 0;h=0;channel=0;
		}

		virtual Point3f  FaceNormal(int index)    {return Point3f(0.0,0.0,0.0);}
		virtual Point3f  FaceColor (int index)    {return Point3f(0.0,0.0,0.0);}
		virtual Point4f  FaceColorA(int index)    {return Point4f(0.0,0.0,0.0,0.0);}

		virtual size_t n_texcoords(){return 0;}

		virtual size_t n_normals() {return 0;}

		virtual size_t n_vertices()   {return 0;}

		virtual size_t n_faces()      {return 0;}

		virtual ~BaseExporter() { }

	};
}

#endif

