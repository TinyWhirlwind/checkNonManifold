#ifndef _SceneObject_
#define _SceneObject_

/*     SceneObject

 该类定义了场景绘制对象基类，实现场景层次架构  */

#include "SceneRender\SceneBaseObject.h"
#include "GeoData/face.h"
#include "Common/Box3.h"
#include "ObjectMotion.h"


namespace sn3DCore
{
	class SceneObject : public SceneBaseObject
	{
	public:
		typedef enum
		{
			TYPE_OBJECT = 0,
			TYPE_RANGE,
			TYPE_MESH,
			TYPE_POINTS
		} ObjectType;

		///////////////  绘制函数   //////////////////////////////////////////////////////

		virtual void  Render() {}			//  绘制模型

		virtual void  RenderDynamic() {}	//  动态绘制

		virtual void  RenderSelected() {}

		virtual void  RenderAlign() {}

		virtual void  RenderBox() {}

		//////////////  数据获取函数   /////////////////////////

		virtual int GetPointSize() { return 0; }

		virtual int GetFaceSize() { return 0; }

		virtual Point3f GetPoint(int index) 
		{
			(void)index;
			return Point3f(0.0, 0.0, 0.0); 
		}

		Point3f GetCenter() { return m_center; }

		/////////////////////////////   标志位方法   //////////////////////////////////////

		bool  IsH() { return (m_flag & INVISIBLE) != 0; }	//  隐藏位 
		void  SetH() { m_flag |= INVISIBLE; }
		void  ClearH() { m_flag &= (~INVISIBLE); }

		bool  IsM() { return (m_flag & MODIFIED) != 0; }	//  修改位	
		void  SetM() { m_flag |= MODIFIED; }
		void  ClearM() { m_flag &= (~MODIFIED); }

		bool  IsShowBox() { return (m_flag & SHOWBOX) != 0; } //  包围盒位
		void  SetShowBox() { m_flag |= SHOWBOX; }
		void  ClearShowBox() { m_flag &= (~SHOWBOX); }

		bool IsS() { return (m_flag & SELECTED) != 0; }	//  选择位
		void SetS() { m_flag |= SELECTED; }
		void ClearS() { m_flag &= (~SELECTED); }

		bool IsNoRender()const { return (m_flag & NORENDER) != 0; }
		void SetNoRender() { m_flag |= NORENDER; }
		void ClearNoRender() { m_flag &= (~NORENDER); }


		//////////////////////////////////////////////////////////////////////////////////////////////

		virtual void UpdateAll() {}  //基类更新方法

		virtual void UpdateDisplay() {}

		int GetID() { return m_index; }     //   获取模型标识
		void SetID(unsigned int id) { m_index = id; };       //   设置模型标识

		ObjectType GetType()const { return m_type; }
		bool		IsOfType(const ObjectType &tp) { return tp == m_type; }

		// 包含后缀名Range (.rge); Mesh(.obj,.stl,...)
		void		SetName(const std::string& szName) { m_strName = szName; }
		const char* GetName() { return m_strName.c_str(); }
		void		SetTypeName(const std::string& szName) { m_typeName = szName; }
		const char* GetTypeName() { return m_typeName.c_str(); }
		void		SetPath(std::string  name) { m_strPath = name; };
		const char* GetPath() { return m_strPath.c_str(); };
		void		SetExtName(const std::string& szName) { m_strExt = szName; }
		virtual const char *GetExtName() { return m_strExt.c_str(); };

		virtual int GetPointNum() { return 0; };
		virtual int GetFaceNum() { return 0; };

		virtual void  GetMatrixGL(float* matrix) { (void)matrix; };
		virtual void  ReleaseData(bool all) { (void)all; };
		virtual void  UpdateBox() {};
		virtual void  UpdateNormal() {};

	public:
		SceneObject();
		virtual ~SceneObject();

		void Cylinder(int slices, float lenght, float width);
		void Cone(int slices, float lenght, float width);

	public:
		Box3f	m_bbox;				//  几何模型包围盒
		Point3f m_center;			//  模型中心
		ObjectMotion m_motion;		//  变换对象
		int m_meshType;

	protected:

		std::string		m_strName;
		std::string		m_strPath;
		std::string     m_strExt;
		ObjectType		m_type;
		std::string     m_typeName;

		int		m_index;			//  模型编号
		int		m_flag;				//  标志位
		enum
		{
			INVISIBLE = 0x01,	//  是否隐藏
			SHOWBOX = 0x02,		//  是否显示包围盒 
			MODIFIED = 0x04,	//  是否被修改
			SELECTED = 0x08,	//  是否被选择
			NORENDER = 0x10		// 是否绘制 
		};
	};
}
#endif