#ifndef _SN3D_MESH_MODEL_
#define _SN3D_MESH_MODEL_

#include "GeoData/Face.h"
#include "SceneObject.h"
#include "sn3DRenderer.h"

#include <boost/smart_ptr.hpp>

/*************************************************
 sn3DMeshModel 类
 该类封装了用于三角网格模型的各类信息,提供了创建、修
 改、快速访问三角网格模型的各类基础功能。
 主要包括:
 1.几何数据, 点、面、及其拓扑结构
 2.变换数据,平移向量、旋转四元数、齐次变换矩阵
 3.绘制对象
***************************************************/
class CObjTriMesh;
class CACADMeshGrid;
struct MeshBoundary
{
	MeshBoundary() { num = 0; }
	std::vector<int> vlist;
	std::vector<int> bridgeInLoop;
	int num;
	void Clear()
	{
		num = 0;
		vlist.clear();
	}
};

// 定向包围盒
class OrientedBox
{
public:
	void Draw() {}
	float Diag()
	{
		Point3f p0 = center + normal*(zLen / 2.0f) + uDir*(uLen / 2.0f) + vDir*(vLen / 2.0f);
		Point3f p1 = center - normal*(zLen / 2.0f) - uDir*(uLen / 2.0f) - vDir*(vLen / 2.0f);
		float distance = (p0 - p1).Norm();
		return distance;
	}

	Point3f center = Point3f(0, 0, 0);
	Point3f normal = Point3f(1, 0, 0);
	Point3f uDir = Point3f(0, 1, 0);
	Point3f vDir = Point3f(0, 0, 1);
	float zLen = 0.0f;
	float uLen = 0.0f;
	float vLen = 0.0f;
};

namespace sn3DCore
{
	class sn3DMeshData;
	class MeshGrid;
	class sn3DMeshRenderer;
	class sn3DMeshModel : public SceneObject
	{
	public:
		static boost::shared_ptr<sn3DMeshModel> New();              //   创建模型

	public:
		sn3DMeshModel();

		sn3DMeshModel(boost::shared_ptr<sn3DMeshModel> model);
		sn3DMeshModel(boost::shared_ptr<sn3DMeshData> mesh);

		virtual ~sn3DMeshModel();

	public:
		typedef  Face FaceType;

		//====== 绘制方法
		virtual void  Render();   //  绘制模型
		virtual void  RenderDynamic();
		virtual void  RenderSelected();	  //  绘制选择顶点
		virtual void  RenderBox();                //  绘制模型包围盒

		virtual void RenderWire(); // 绘制线框
		void RenderForSelection();

		//////////////////  模型更新方法  ///////////////////
		virtual void  UpdateAll();			// 总体更新，导入时使用
		virtual void  UpdateDisplay();		// 更新点面显示列表 同时更新法线
		//void  UpdatePointDisplay();			// 更新点显示列表 同时更新法线 
		//void  UpdateFaceDisplay();			// 更新面显示列表 同时更新法线
		void  UpdateBox();					// 更新模型包围盒
		void  UpdateNormal();				// 更新法向		
		void  UpdateDirection();			// 更新模型几何方向
		void  UpdateGeometry();				// 更新几何
		void  UpdateGeometryWithoutTranslation();
		void  BuildPreviewList();
		
		virtual inline void setLocalScale(Point3f ptScale, bool bUpdateMatrix = true)
		{
			SceneBaseObject::setLocalScale(ptScale);
			setNeedUpdateGrid();
			if (bUpdateMatrix)
			{
				updateALLMatrix44f();
			}
		}


		virtual inline void setLocalScale(float fScale)
		{
			setLocalScale(Point3f(fScale, fScale, fScale));
			setNeedUpdateGrid();
		}

		virtual inline void setLocalScale(float fX, float fY, float fZ)
		{
			setLocalScale(Point3f(fX, fY, fZ));
			setNeedUpdateGrid();
		}


		//////////////////    数据访问方法   ///////////////////////
		/// 设置模型几何数据  
		virtual void SetMesh(boost::shared_ptr<sn3DMeshData> m);
		/// 增加默认几何数据
		boost::shared_ptr<sn3DMeshData> AddMesh();
		boost::shared_ptr<sn3DMeshData> AddMesh(
			int vNum,
			int faceNum,
			const std::vector<Point3f>& points,			//点坐标
			const std::vector<unsigned int>& faces,		//面片
			bool bTexture = false,
			const std::vector<Point2f>& mapPoints = std::vector<Point2f>());		//2d纹理

		int imark;
		inline void Mark(Face * f) const { f->IMark() = imark; }
		inline bool IsMarked(Face * f) const { return f->IMark() == imark; }

		/// Unmark the mesh
		inline void UnMarkAll() { ++imark; }
		boost::shared_ptr<sn3DMeshData> GetMeshData() { return m_mesh; }
		boost::shared_ptr<sn3DMeshData> GetMeshData() const { return m_mesh; }

		boost::shared_ptr<sn3DMeshData> GetMeshDataForGrid() 
		{ 
			if (m_meshForGrid ==nullptr || m_bNeedUpdateGrid)
			{
				InitGrid();
			}
			return m_meshForGrid; 
		}
		//boost::shared_ptr<sn3DMeshData> GetMeshDataForGrid() const { return m_meshForGrid; }

		virtual int GetPointSize();
		virtual int GetFaceSize();

		boost::shared_ptr<sn3DMeshRenderer> GetRenderer() { return m_render; };

		Point3f GetMainAxis() { return m_curMainAxis; }
		Point3f GetSecoAxis() { return m_curSecoAxis; }
		Point3f GetThirAxis() { return m_curThirAxis; }

		void  SetCenter(Point3f center) { m_center = center; }

		void  SetMainAxis(Point3f mainAxis) { m_curMainAxis = mainAxis; }
		void  SetSecoAxis(Point3f secoAxis) { m_curSecoAxis = secoAxis; }
		void  SetThirAxis(Point3f thirAxis) { m_curThirAxis = thirAxis; }

		float GetMainLength() { return m_mainLength; }
		float GetSecoLength() { return m_secoLength; }
		float GetThirLength() { return m_thirLength; }

		void  SetDrawMode(GLW::DrawMode dm);
		void  SetColorMode(int flag);

		void Clear();                      //   清除模型数据    

		const char *GetFullPath();

		void ReleaseData(bool all);

		OrientedBox m_boundingBox; // 定向包围盒

		std::vector<Face *>   m_selectedFace;    //   已选择面,暂保留

		std::vector<Vertex *> m_selectedVertex;  //   已选择顶点,暂保留

		float m_avgEdgeLen;

		Matrix44f  Tr;

		boost::shared_ptr<sn3DMeshRenderer> m_render;//   绘制对象
		//boost::shared_ptr<sn3DMeshRenderer> m_Printrender;//   绘制对象


		inline void setNeedUpdateGrid() // 通知刷新模型碰撞体
		{
			m_bNeedUpdateGrid = true;
		}

		inline void setNeedUpdateModel() { m_bNeedUpdateDisplay = true; } // 通知刷新模型
		inline bool getNeedUpdateModel() { return m_bNeedUpdateDisplay; }

		// 反转模型面片
		virtual void MirrorFace();

		virtual void GetTransformForOpengl(float* mat);
	protected:
		// 模型被镜像
		bool m_bIsMirrored = false;

	protected:

		///////////////////  基本几何信息  //////////////////

		boost::shared_ptr<sn3DMeshData> m_mesh;    //  几何数据
		boost::shared_ptr<sn3DMeshData> m_meshForGrid;    //  几何数据
		boost::shared_ptr<MeshGrid> m_Grid;    // 层次包围盒

		Point3f m_originMainAxis = Point3f(0,0,0);	//初始主轴方向 
		Point3f m_originSecoAxis = Point3f(0, 0, 0);		//初始次轴方向 
		Point3f m_originThirAxis = Point3f(0, 0, 0);		//初始第三轴方向 

		Point3f m_curMainAxis = Point3f(0, 0, 0);			//当前主轴方向 
		Point3f m_curSecoAxis = Point3f(0, 0, 0);			//当前次轴方向 
		Point3f m_curThirAxis = Point3f(0, 0, 0);			//当前第三轴方向 

		Point3f m_mainAxis = Point3f(0, 0, 0);				//  模型主轴
		Point3f m_secoAxis = Point3f(0, 0, 0);				//  模型次轴
		Point3f m_thirAxis = Point3f(0, 0, 0);			//  模型三轴
		float m_mainLength = 0.0f;			//  模型主轴投影长度
		float m_secoLength = 0.0f;			//  模型次轴投影长度
		float m_thirLength = 0.0f;		//  模型三轴投影长度

		bool m_bNeedUpdateDisplay = true;

		bool m_bNeedUpdateGrid = true;

	public:
		// 临时函数,等坐标系完全替换后删除
		virtual void SetWorldRotateVector(Point3f ptNormal, Point3f ptHeading)
		{
			setWorldRotate(ptNormal, ptHeading);
		}

		virtual void SetWorldPos(Point3f pos)
		{
			setWorldPosition(pos);
		}

		// 射线求交--通用
		virtual bool RayCrossPoint(const Point3f& orgin, const Point3f& dir, Point3f& hitPoint, int& hitTri);
		// 最近点--通用
		virtual bool ClosetPoint(const Point3f& point, Point3f& closetPt, Point3f& closetN);
		// 判断点是否在牙齿内部--通用
		virtual bool IsPointIn(const Point3f& point);

		virtual void InitGrid();
		virtual boost::shared_ptr<MeshGrid> GetGrid();
		bool IntersectModelBox(boost::shared_ptr<sn3DCore::sn3DMeshModel> pModel);
		virtual std::string OutputInformation()
		{
			return "Information!";
		}
	};
}
#endif