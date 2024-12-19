#ifndef _SN3DPICK_
#define _SN3DPICK_

#include <vector>

#include"../../SceneGraph/sn3DObject.h"
#include"../../SceneRender/SceneData.h"
#include"../../../Common/Point3.h"
#include"../../../Common/Quaternion.h"

/*       sn3DPick类

实现了二维屏幕鼠标操作到三维场景点选的交户算法*/

namespace sn3DCore
{

class sn3DPick
{
public:

	virtual void PaintVertex(sn3DObject * obj,Quaternionf rot,int flag);  //画刷选点
	
	virtual void RectVertex(sn3DObject * obj,Quaternionf rot,int flag);   // 矩形选点
	
	virtual void SelectPolygonVertex(sn3DObject * obj,Quaternionf rot,int flag); // 多边形选点

	virtual	void SelectRefPolygonVertex(sn3DObject * m,Quaternionf rot,int flag);//标志点选择

	virtual void UpdateMatrix();        //  更新场景矩阵信息,在需要场景信息之前调用，及时获得最新的场景变换矩阵

	virtual void DrawXORPaintCircle();  //  画刷选择圆圈绘制
	
	virtual void DrawPolygon();         //  多边形选择多边形绘制
	
	virtual void DrawRect();            //  矩形选择矩形绘制

	virtual void Clear();

public:
	
	sn3DPick();
	virtual ~sn3DPick();

public:

	std::vector<Point2i> m_Polygon; 
	
	std::vector<Point2i> m_Rect;

	bool m_IsDragging;		//是否正在拖动中
	
	bool m_IsSelected;		//是否拾取

	Point2f m_CurPoint;	//鼠标点击当前点
	
	Point2f m_PrevPoint;	//鼠标点击前一点

	double	m_MvMatrix[16]; //模型视点矩阵

	double	m_ProjMatrix[16]; //投影矩阵
	
	int	m_Viewport[4];	//当前视口
	
	float * m_Pixels;		// 当前z-buffer
	
    unsigned int*	m_SelectBuf;  // 选择缓存
	
	double  m_PaintRadius;  // 平面画刷半径
	
	double  m_PaintRadius2; // 平面画刷半径的平方

	int     m_SelectRadius;   // 绘制使用的半径值    

	std::vector<Point3f> points; //测试用

};
}
#endif
