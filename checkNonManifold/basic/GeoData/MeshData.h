#ifndef  _SN3D_MESH_DATA_
#define  _SN3D_MESH_DATA_

/*            sn3DMeshData 类

该类封装了三角网格模型几何数据的表示以及访问方法
主要包括:

1.点、面、边数据,及其拓扑访问结构

2.点、面属性(法向、颜色、纹理坐标、标志)

3.存储方法(内存到文件、删除方式、更新方式)        */

#include "GeometryData.h"
#include "Face.h"
#include "Common/box3.h"
#include "Common/Point3.h"

#include <boost/shared_ptr.hpp>

namespace sn3DCore
{
	class sn3DMeshData : public sn3DGeometryData
	{
	public:
		static boost::shared_ptr<sn3DMeshData> New();

	public:
		sn3DMeshData();
		sn3DMeshData(boost::shared_ptr<sn3DMeshData> pData);
		virtual ~sn3DMeshData();

		//////////////////////////////   几何数据访问方法    /////////////////////////////

		int n_vertices();

		int n_faces();

		Vertex * V(int index);                  //   获取顶点

		const Vertex * V(int index) const;

		Edge * E(int index);                    //   获取边

		Edge * FE(int f, int i);

		Face * F(int index);                    //   获取面

		const Face * F(int index) const;        //   获取面

		size_t n_vertices() const               //   获取顶点数目
		{
			return vn;
		}

		size_t n_faces() const                  //   获取面数目
		{
			return fn;
		}

		size_t n_texcoords() const              //   获取顶点纹理数
		{
			return this->m_vertexTexCoord.size();
		}

		size_t n_normals() const                //   获取顶点法向数
		{
			return this->m_vertexNormal.size();
		}

		Vertex * AddVertex();                       //   增加默认顶点

		Vertex * AddVertex(const Point3f & point, const Color4f &color = Color4f(1.0f, 1.0f, 1.0f, 1.0f), bool update = true);  //   增加顶点

		Vertex * AddVertex(const Point3d & point, const Color4f &color = Color4f(1.0f, 1.0f, 1.0f, 1.0f), bool update = true);  //   增加顶点

		void AddVertex(int n);		// 增加顶点数(占位)

		Face * AddFace();	                       //   增加默认面

		Face * AddFace(std::vector<int>& vIndices);//   增加面

		Face * AddFace(int v0, int v1, int v2, bool bReverse = false);      //   增加面

		void AddFace(int n);		// 增加面片数(占位)

		void  AddFaceTexCoord(Point2f p1, Point2f p2, Point2f p3);

		void  AddFaceTopology();

		void  AddFaceNormal();

		void  DeleteVertex(int index);                 //   删除顶点

		void  DeleteFace(int index);	               //   删除面

		void  operator = (boost::shared_ptr<sn3DMeshData> copy);        //   拷贝函数
		boost::shared_ptr<sn3DMeshData> Copy();

		/////////////////////////    几何操作方法     /////////////////////////////////////

		void  SetAttrib(std::vector<float> & attribs);

		float GetAvgEdgeLen();  //平均边长

		void  UpdateNormal();                          //  计算法向	
		void  ClearNormal();                           //  删除法向

		void  UpdateEdge();                            //  计算边数据

		void  ClearEdge();

		void  CopyGeometry(boost::shared_ptr<sn3DMeshData> gm);

		bool  HasFaceTexture();

		void  UpdateVertexTopology();                  //  更新点拓扑

		void  UpdateFaceTopology();                    //  更新面拓扑

		void  ClearVertexTopology();                   //  删除顶点拓扑

		void  ClearFaceTopology();	                   //  删除面拓扑

		void  UpdateFaceIdentity();                    //  更新面标志

		void  UpdateFaceIdentity1();

		void  UpdateFaceIdentity2();

		void  UpdateVertexIdentity();                  //  更新顶点标志

		void  ClearVertexIdentity();

		void  UpdateFaceTexCoord();

		void  UpdateVertexTexCoord();

		void  UpdateFaceMark();

		void  ClearFaceMark();

		void  UpdateVertexMark();

		void  ClearVertexMark();

		void  UpdateVertexAttrib();

		void  ClearVertexAttrib();

		///////////////////////////  存储操作方法      ///////////////////////////

		void  Compact();                               //  压缩内存

		void  Clear();	                               //  清除几何数据

		///////////////////////// 数据更新 //////////////////////////////////////
		/// 更新拓扑VertFace，FaceFace
		void DirtyTopology();
		/// 识别边界
		void DirtyBoundary();
		/// 清理冗余顶点
		void RemoveRedundant();
		/// 反转面片
		void MirrorFace();

	public:

		unsigned int vn;       //  点数

		unsigned int fn;       //  面数

		unsigned int en;       //  边数

	};
}

namespace Geom
{
	using namespace sn3DCore;

	struct	OrientedBoundingBox
	{
		Point3f center;

		float ulen;
		float mlen;
		float slen;
		Point3f upDir;
		Point3f mdir;
		Point3f sdir;

		void operator=(OrientedBoundingBox & rhs)
		{
			center = rhs.center;

			ulen = rhs.ulen;
			mlen = rhs.mlen;
			slen = rhs.slen;
			upDir = rhs.upDir;
			mdir = rhs.mdir;
			sdir = rhs.sdir;
		}

	};

	Box3f meshAABB(sn3DMeshData* mesh);

	float faceArea(Face * f);

	Point3f faceNormal(Face * f);

	Point3f PlaneProject(Point3f upDir, Point3f dir); // 平面向量投影

	void  updateBoundingBox(sn3DMeshData* mesh, Point3f upDir, Point3f mdir, Point3f sdir, OrientedBoundingBox & box);

	Point3f faceBarycentric(Face * f, Point3f & ip); // 计算重心坐标

	void latticeTriangulation(sn3DMeshData* mesh, double(*functor)(double, double), float x, float y, float w, float h,
		int precision = 100, double distortion = 0.0);

}
#endif