#ifndef _SN3DSELECT_
#define _SN3DSELECT_



/*   sn3DSelect 类

   三维场景选择功能实现类

*/

#include "sn3DMeshPick.h"
#include "sn3DRangePick.h"
#include "../../SceneRender/sn3DSceneGL3D.h"
#include "../../SceneGraph/sn3DObject.h"
namespace sn3DCore
{

// 选择模式码
#define PAINT_SELECT  1
#define POLYGON_SELECT 2
#define RECT_SELECT 3
#define REFPOINT_SELECT 4

class sn3DSelect 
{
public:

	void   Draw(sn3DSceneGL3D *,int feature);               //  场景接口,辅助绘制以及交互时使用  
	
	bool   MouseMove(sn3DSceneGL3D *,unsigned int,int,int); //  鼠标移动消息,选择拖动
	
	bool   LeftClick(sn3DSceneGL3D*,unsigned int,int,int);  //  鼠标左键消息,进入选择拖动状态 
	
	bool   LeftUp(sn3DSceneGL3D *,int,int);                 //  鼠标左键弹起消息，选择拖动结束

	void   SetMode(int mode);                      //   设置选择模式

	void   SetMesh(sn3DObject * mesh);              //   设置待选择对象

	void   SetScene(sn3DSceneGL3D * scene);          //   设置待选择场景

	void   SetPick(sn3DPick * pick);                //   设置选择功能对象

	bool   IsDragging();                           // 是否在拖动

	void   Store(){m_LastState = m_SelectMode;}

	void   Restore(){if(m_LastState!=0)m_SelectMode = m_LastState;}

	void   SetSelectRadius(int selectRadius);

	void   Clear();

public:

	sn3DSelect();
	
	~sn3DSelect();

	int m_feature;

	int	m_State;

	int	m_SelectMode;

	bool isPicking;

	sn3DObject * m_object;

	sn3DPick *		m_pick;

protected:	

	int	m_LastState;
	
	

	
};

}
#endif