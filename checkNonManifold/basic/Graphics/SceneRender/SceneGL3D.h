#ifndef _SceneGL3D_
#define _SceneGL3D_

#include "SceneGL.h"
#include "SceneObject/SceneObject.h"
#include "SceneInteract/Navigation/Trackball.h" 

#include "boost/smart_ptr.hpp"

/***********************************************************************
*类 SceneGL3D：实现了显示网格、点云模型的三维场景                  *
*                                                                      *
* 1 支持鼠标旋转、平移、缩放                                           *
* 2 支持多种绘制样式:线绘制、点绘制、着色绘制、带边着色绘制            *
* 3 材质、灯光管理                                                     *
***********************************************************************/

namespace sn3DCore
{
	//======视点类型定义
	typedef enum
	{
		VIEWPOINT_FRONT		= 0,
		VIEWPOINT_BACK		= 1,
		VIEWPOINT_LEFT		= 2,
		VIEWPOINT_RIGHT		= 3,
		VIEWPOINT_TOP		= 4,
		VIEWPOINT_BOTTOM	= 5
	}VIEWPOINT_TYPE;

	//===== 背景类型定义
	typedef enum
	{
		BACKGROUND_DEFAULT	= 0,
		BACKGROUND_GRADIENT,
		BACKGROUND_GRADIENT_GREY,
		BACKGROUND_GRID,
	}BACKGROUND_TYPE;

	class SceneGL3D : public SceneGL
	{
	public:

		//======外部可自定义方法
		virtual void InitSceneParameter();	// 初始化场景参数设置 

		virtual void MouseDown(int x, int y, int button); //处理场景交互
		virtual void MouseMove(int x, int y);
		virtual void MouseUp(int x, int y, int button);
		virtual void MouseWheel(float notch);


		//======场景绘制方法
		virtual void PaintStatic();			// 绘制场景背景
		virtual void PaintMotion();			// 绘制场景控件及自定义实体
		virtual void PaintScene();			// 绘制场景自定义模型


		//======场景操作、设置方法
		void Reset();									// 重置场景(重置轨迹球) 
		void UpdateSceneBox();							// 更新场景包围盒,即自定义模型的范围
		void SetBackColor(Color4f color);				// 设置背景颜色
		void setBackGroundMode(BACKGROUND_TYPE mode = BACKGROUND_DEFAULT);				//设置背景模式
		BACKGROUND_TYPE getBackGroundMode() { return m_backgroundMode;  }
		void SetViewMode(VIEWPOINT_TYPE mode);			// 设置观察模式
		virtual bool Is3DScene() { return true; };
		virtual Point2f GetSceneSize() = 0;				//获取场景区域
		void GetLookAt(Point3f& eye, Point3f& center, Point3f& up);						// 获取视点

		//======场景数据方法
		int  GetObjectCount();							 // 获取场景自定义模型数目
		boost::shared_ptr<SceneObject> GetObject(int index);							// 获取场景自定义模型
		void AddObject(boost::shared_ptr<SceneObject> model, bool boxupdate = true);	// 增加场景自定义模型
		void PushObject(boost::shared_ptr<SceneObject> model);							// 加入场景自定义模型，不进行显示更新
		void RemoveObject(boost::shared_ptr<SceneObject> model);						// 删除场景自定义模型  
		boost::shared_ptr<SceneObject> GetActiveObject();								// 获取当前模型
		bool SetActiveObject(boost::shared_ptr<SceneObject> object);					// 设置当前模型
		void ClearScene();								//清空场景数据

		void RenderDepthBuffer();

		void GetRotate(Quaternionf &rot);	// 获取旋转参数

		bool  HitObj(int X, int Y);

		//======构造、析构函数
		SceneGL3D(void);
		virtual ~SceneGL3D();

		bool m_showTexture;
		bool noRender;
		bool m_isDragging;

	public:

		//======内部函数
		void InitSceneCamera();    //设置场景照像机
		void InitViewPoint();      //设置视点位置
		void InitLightModel();     //设定场景灯光
		void ScaleSceneModel();    // 缩放场景中的模型至标准范围(CAD模式)
		void EnlargeSceneBox(boost::shared_ptr<SceneObject> obj);  // 扩大场景包围盒

		void RenderCustomObject(); //绘制自定义CAD模型
		void RenderCustomEntity(); //绘制自定义绘制对象

		//======场景包围盒绘制
		void drawSceneBox();

		//======场景坐标轴绘制
		void drawSceneAxis();

		// =====背景绘制
		void drawBackGroundDefault();
		void drawBackGroundGradient();
		void drawBackGroundGradientGrey();
		void drawBackGroundGrid();
		BACKGROUND_TYPE			m_backgroundMode;

		// =====格栅平面
		/// \note
		///		1.最小格子0.02
		///		2.相隔5格子线变粗 单位0.1
		///		3.随模型同步放大缩小
		void drawGridPlane();

		//======场景数据
		bool m_drawDepth;
		std::vector<Color4f>	m_materials;
		VIEWPOINT_TYPE			m_viewMode;			// 视点模式
		Color4f					m_bgColor;			// 背景颜色
		Box3f					m_sceneBox;			// 场景自定义模型包围盒
		Trackball				m_trackball;		// ARCBALL
		boost::shared_ptr<SceneObject>					m_curObject;	// 当前处理的几何模型
		std::vector<boost::shared_ptr<SceneObject>>	m_customObjects;	// 自定义CAD模型或几何数据
		std::vector<boost::shared_ptr<SceneObject>>	m_customEntitys;	// 自定义绘制对象
		float                  m_ChangeRatio=1.0f;
	};
}
#endif
