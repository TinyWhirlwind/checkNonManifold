#ifndef _SN3D_RENDERER_
#define _SN3D_RENDERER_

/*    sn3DRenderer 类  

该类实现了常用OpenGL渲染绘制功能

*/

#include "GeoData/GeometryData.h"
#include <GeoData/vertex.h>
#include <GeoData/face.h>
#include <map>

namespace sn3DCore
{
	// 增加了动态显示列表功能
	class ListType
	{
	public:
		// 定义显示列表类型
		enum Type{LTBox, LTPoint , LTFlat , LTSmooth};

	public:
		// 构造函数
		ListType():type(LTSmooth),detail(2),needDynamic(false),
			typeDynamic(LTPoint),detailDynamic(4){};
		bool operator == ( ListType const & other ) const
		{ 
			if( needDynamic!=needDynamic)
				return false;
			else if( needDynamic==true)
				return type==other.type && detail==other.detail 
				&& typeDynamic==other.typeDynamic && detailDynamic==other.detailDynamic;
			else
				return type==other.type && detail==other.detail;
		}
		bool operator != (ListType const & other ) const
		{
			return ((*this)==other)==false;
		}
		
	public:
		// 属性
		bool	needDynamic;

		Type	type;			// 静态列表（vbo）类型
		int		detail;			// 静态列表（vbo）分辨率1/4，1/16
		
		Type	typeDynamic;	// 动态列表（vbo）类型
		int		detailDynamic;	// 动态列表（vbo）类型
	};

	class DisplayList
	{
	public:
		DisplayList():list(-1),memory(0){};
		~DisplayList();

		int			list;
		ListType	type;
		int			memory;
	};

	/////////////////////////////////////////////////////////////////////////////
	//
	// class sn3DRenderer
	//
	/////////////////////////////////////////////////////////////////////////////
	class GLW
	{
	public:
		virtual ~GLW(){}
		//  绘制模式
		enum DrawMode	{ DMPoints, DMWire, DMHidden, DMFill, DMFillWire};   
		//  着色模式
		enum ShadeMode  { SMFlat , SMSmooth, SMNormalSplit,SMPhone };
		//  渲染方法
		enum RenderMethod { RMGeneral, RMDisplayList, RMBufferObject, RMShader }; 
		//  颜色模式
		enum ColorMode	{ CMPerMesh, CMPerVertex, CMPerFace, CMMaterial};
	};

	class sn3DRenderer: public GLW
	{
	public:
		enum 
		{
			HASTEX        = 0x01,		// 是否有纹理
			MULTTEX       = 0x02,		// 是否多纹理
			SELECTED      = 0x04,		// 是否被选择
			POINTCLOUD    = 0x08,		// 是否是点云
		};	
	public:

		sn3DRenderer();
		virtual ~sn3DRenderer();

		/// 清理
		virtual void	Clear();

		//////////////////////////  常用OpenGL绘制方法   ///////////////////////
		static  void	DrawBox(Point3f maxPoint,Point3f minPoint);
		static  void	DrawBox(std::vector<Point3f> corner);
		static  void	DrawSolidBox(Point3f maxPoint,Point3f minPoint);
		static  void	Cone(int slices,float lenght,float width);
		static  void	Cylinder(int slices,float lenght,float width);			

		virtual void	RenderSelected() {}
		virtual void	SetPointRender() {}

		void			SetModify() { m_bModified = true; }
		void			SetVertexColor(bool color) { m_bVetexColor = color; }		// 顶点绘制是否带颜色

		// 在使用UpdatList（）之前，得调用SetListType（）设置列表类型
		virtual void	SetListType(ListType type);
		ListType&		GetListType()				{return m_list.type;}	
		ListType		m_listType;					// 要重建的显示列表类型(静态/动态)

		void			BuildList(ListType type);		// 直接建立显示列表，效果与SetListType和UpdatList一样
		virtual int		NeedMemory(){ return 0;};


		void			NewList();
		void			SetDisplayList(int list);
		void			ClearDisplayList();				//  清除显示列表
		void			BuildList();
		virtual void	BuildPointList(/*int detail =0*/) {};              //  生成顶点显示列表 //detail可不要 
		virtual void	BuildFaceList(/*int detail =0*/) {};               //  生成面显示列表   //detail可不要 

		// =======设置绘制模式
		void		SetDrawMode(DrawMode dm)		{ m_drawMode = dm;}
		DrawMode &	GetDrawMode()					{ return m_drawMode;}
		void		SetRenderMethod(RenderMethod rm)	{ m_renderMethod = rm;  }
		RenderMethod& GetRednerMethod()				{ return m_renderMethod;  }
		void		SetShadeMode(ShadeMode sm)		{ m_shadeMode = sm; }
		ShadeMode&	GetShadeMode()					{ return m_shadeMode; }
		void		SetColorMode(ColorMode cm)		{ m_colorMode = cm;  }
		ColorMode&  GetColorMode()					{ return m_colorMode; }
		void		SetDrawDynamic(bool dy=true)	{ m_bDrawDynamic=dy;}
		bool		IsDrawDynamic()					{ return m_bDrawDynamic;}
		// =======设置绘制属性
		void		SetBlend(bool bEnable)	{ m_bTransparent = bEnable; }
		bool		IsBlend()					{ return m_bTransparent;  }
		void		SetDepthMask(bool bEnable) { m_bDepthMask = bEnable; }
		bool		IsDepthMask() { return m_bDepthMask; }
		void		SetDepthTest(bool bEnable) { m_bDepthTest = bEnable; }
		bool		IsDepthTest() { return m_bDepthTest; }
		void		SetTwoSideLight(bool bEnable)	{ m_bTwoSideLight = bEnable; }
		bool		IsTwoSideLight()				{ return m_bTwoSideLight; }
		void		SetLighting(bool bEnable)		{ m_bLighting = bEnable; }
		bool		IsLighting()					{ return m_bLighting; }

		// =======材质属性
		struct MaterialInfo
		{
			Point4f ambient;  //环境光照
			Point4f diffuse;  //漫反射光照
			Point4f specular;  //镜面反射光照
			float shininess;    //反光度
			MaterialInfo()
			{
				ambient.Zero();
				diffuse.Zero();
				specular.Zero();
				ambient[3] = diffuse[3] = specular[3] = 1.f;
				shininess = 0.0f;
			}
		};
		enum MaterialFace
		{
			MF_FRONT	=1,
			MF_BACK,
			MF_FRONT_AND_BACK,
		};
		std::map<MaterialFace, MaterialInfo> m_material;
		void		AddMaterial(MaterialFace mf, MaterialInfo& info);
		void		ClearMaterial();
		

		// ========访问属性
		bool IsHasTex()const	{return (m_flag & HASTEX)!=0;}	  
		void SetHasTex()		{m_flag |= HASTEX;}	  
		void ClearHasTex()		{m_flag &= (~HASTEX);}
		void SetMultTex()		{m_flag |= MULTTEX;}	  
		void ClearMultTex()		{m_flag &= (~MULTTEX);}

		bool IsS()				{return (m_flag & SELECTED)!=0;}	//  选择位
		void SetS()				{m_flag |= SELECTED;}                
		void ClearS()			{m_flag &= (~SELECTED);}                 

		bool IsPointCloud()const{return (m_flag & POINTCLOUD)!=0;}
	    void SetPointCloud()	{m_flag |= POINTCLOUD;}
	    void ClearPointCloud()	{m_flag &= (~POINTCLOUD);}
	  
		int		m_flag;            //  标志位

	public:
		virtual void Update() = 0;
		virtual void Draw() = 0;

		virtual void DrawFill() = 0;							//  着色绘制
		virtual void DrawWire() = 0;							//  线框绘制
		virtual void DrawPoints() =0;							//  点绘制
		virtual void DrawHidden();								//  隐藏线绘制
		virtual void DrawFillWire();							//  带边绘制

		virtual void StartProperty();
		virtual void EndProperty();

		void DrawGeneral();										//  管线渲染方式

	public:
		RenderMethod m_renderMethod;							// 渲染方式
		DrawMode	m_drawMode;									// 绘制模式
		ShadeMode	m_shadeMode;								// 着色模式(单调/平滑)
		ColorMode	m_colorMode;								// 颜色模式

		bool		m_bModified;
		bool		m_bVetexColor;
		bool		m_bDrawDynamic;
		bool		m_bTransparent;								// 透明模式
		bool		m_bDepthMask;
		bool		m_bDepthTest;
		bool		m_bTwoSideLight;							// 是否双面光照
		bool		m_bLighting;								// 是否开启光照

		DisplayList	m_list;										// 当前显示的列表
	};
}
#endif
