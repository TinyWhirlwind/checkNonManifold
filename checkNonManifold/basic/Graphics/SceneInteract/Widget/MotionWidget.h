#ifndef _MOTIONWIDGET_
#define _MOTIONWIDGET_

#include "../../../Common/point3.h"
//#include "../../SceneRender/SceneData.h"
#include "../../Entity/SceneObject.h"
#include "../../../Common/Quaternion.h"
#include "../../SceneModel/sn3DObject.h"

namespace sn3DCore
{
class MotionWidget
{
public:

   void  SetData(sn3DObject * obj);
   void  Transform();
   void  Initialize(Point3f mainAxis,Point3f secoAxis,Point3f thirAxis,Point3f center); //初始化局部坐标系
   void  HitTest(); //测试控件是否被激活
   
   void  Motion(Point3f & mainAxis,Point3f & secoAxis,Point3f & thirAxis,Point3f & center);//总体运动函数
   void  Move(float distance ,int axis);//平移函数
   void  Rotate(float degree,int axis);//旋转函数
   
   void  Multiply(float & newS,float & newQX,float & newQY,float & newQZ,float  S,float  QX, 
	             float  QY , float  QZ,float  s, float  qx, float qy , float  qz);//四元数乘法
   
   float UnProjectDistance(Point3f dir); //将用户在屏幕上的鼠标操作转换为场景中的位移
   
   void  DrawWidget();//绘制局部坐标系控件
   void  DrawPoint();//绘制测试点
   void  DrawCircle ();//绘制圆环
   void  DrawSegment();//绘制线段

public:

      MotionWidget();
	  ~MotionWidget();
   
public:

   sn3DObject * m_object;

   //局部坐标系参数
   float m_Mat1[9];//三个方向的任意轴旋转矩阵
   float m_Mat2[9];
   float m_Mat3[9];
   
   float m_Size; //局部运动系标架大小

   Quaternion<float> m_Quaternion;//运动四元数
   
   Point3f m_Center;    //运动架中心
   Point3f m_OriginMainAxis;   //初始主轴方向 
   Point3f m_OriginSecoAxis;   //初始次轴方向 
   Point3f m_OriginThirAxis;   //初始第三轴方向 

   Point3f m_CurMainAxis;   //当前主轴方向 
   Point3f m_CurSecoAxis;   //当前次轴方向 
   Point3f m_CurThirAxis;   //当前第三轴方向 

   float m_S,m_QX,m_QY,m_QZ; //当前旋转四元数,相对于原轴方向

   float m_Matrix[16];//总旋转矩阵
   
   Point3f m_RotatePoint; //旋转操作点

   bool m_IsDragging;
   bool m_IsActive;
   bool m_Active[6];
   
   SceneObject m_SceneObject; //场景绘制
   float m_Degree1,m_Degree2,m_Degree3;
   Point3f m_Axis1,m_Axis2,m_Axis3;
   
  //OpenGL 场景支持
   Point3f m_CurPoint; //鼠标点击当前点
   Point3f m_PrevPoint;//鼠标点击前一点
   double m_MvMatrix[16]; //模型视点矩阵
   double m_ProjMatrix[16]; //投影矩阵
   int  m_Viewport[4]; //当前视口
   unsigned int * m_SelectBuf;//select-buffer
   void UpdateMatrix();

};
}
#endif