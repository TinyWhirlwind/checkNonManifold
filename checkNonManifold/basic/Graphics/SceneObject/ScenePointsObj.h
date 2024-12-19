#ifndef _SN3D_POINTS_MODEL_
#define _SN3D_POINTS_MODEL_

/*         sn3DPointsModel 类

  该类封装了点云数据模型，适合大规模点云处理 */

#include"SceneObject.h"
#include"GeoData/PointsData.h"
#include"sn3DPointsRenderer.h"

namespace sn3DCore
{
	class sn3DPointsModel : public SceneObject
	{
	public:

		////////////////////  绘制方法  /////////////////////

		void  Render();   //  绘制模型
		void  RenderBox();                //  绘制模型包围盒
		void  RenderSelected();

		//////////////////  模型更新方法  ///////////////////

		void  UpdateAll();               //   总体更新，导入时使用
		void  UpdateDisplay();           //   更新点面显示列表 同时更新法线
		void  UpdateSelectionDisplay();
		void  UpdatePointDisplay();      //   更新点显示列表 同时更新法线 
		void  UpdateBox();               //   更新模型包围盒
		void  UpdateDirection();         //   更新模型几何方向

		//////////////////    数据访问方法   ///////////////////////

		void SetPoints(int vn, float *points); // 设置点云数据

		int GetPointSize();

		Point3f GetPoint(int index);

		boost::shared_ptr<sn3DPointsData> AddPointCloud();          //   增加默认几何数据

		boost::shared_ptr<sn3DPointsData> AddPointCloud(size_t);

		boost::shared_ptr<sn3DPointsData> GetPointCloud() { return m_pointCloud; }

		boost::shared_ptr<sn3DPointsData> GetPointCloud() const { return m_pointCloud; }

		boost::shared_ptr<sn3DPointsRenderer> GetRenderer() { return m_render; }

		virtual bool IsSelected();

		//sn3DCore::sn3DTextureGL * GetTexture() {return &m_texture;}

		sn3DPointsModel();

		sn3DPointsModel(boost::shared_ptr<sn3DPointsModel> pointmodel);

		void Clear();

		////////////////////  points flag  /////////////////////////

			/// Point flag
		enum
		{
			POINTINVISIBLE = 0x01,  // 是否隐藏
			POINTDELETE = 0x02   // 是否删除
		};

		bool  IsPointH(int index) { return (m_pointCloud->Flag(index) & POINTINVISIBLE) != 0; }	//  隐藏位 
		void  SetPointH(int index) { m_pointCloud->Flag(index) |= POINTINVISIBLE; }
		void  ClearPointH(int index) { m_pointCloud->Flag(index) &= (~POINTINVISIBLE); }

		bool  IsPointD(int index) { return (m_pointCloud->Flag(index) & POINTDELETE) != 0; }	//  隐藏位 
		void  SetPointD(int index) { m_pointCloud->Flag(index) |= POINTDELETE; }
		void  ClearPointD(int index) { m_pointCloud->Flag(index) &= (~POINTDELETE); }

	private:
		boost::shared_ptr<sn3DPointsData>		m_pointCloud;    // 几何数据
		boost::shared_ptr<sn3DPointsRenderer>	m_render;		//  绘制对象
	};
}

#endif