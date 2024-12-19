#include "stdafx.h"
#include "MotionWidget.h"
#include <math.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include "../../SceneModel/sn3DMeshModel.h"

namespace sn3DCore
{
MotionWidget::MotionWidget()
{
  for(int i=0;i<6;i++) m_Active[i]=false;
  m_IsActive = false;
  m_SelectBuf = NULL;//分配选择缓存
  m_IsDragging = false;
  m_S = 1;m_QX = m_QY = m_QZ = 0;
  for(int i=0;i<16;i++) m_Matrix[i]=0;

  m_Quaternion.X() = m_Quaternion.Y() = m_Quaternion.Z() = 0.0;
  m_Quaternion.W() = 1.0;

  m_Matrix[0]=1;
  m_Matrix[5]=1;
  m_Matrix[10]=1;
  m_Matrix[15]=1;

  m_object = NULL;
  m_SelectBuf = new unsigned int[400];
}
MotionWidget::~MotionWidget()
{
 delete m_SelectBuf;
}
void  MotionWidget::SetData(sn3DObject * obj)
{
   Point3f mainAxis,secoAxis,thirAxis,center;
   if(obj->IsOfType(sn3DObject::TYPE_MESH))
   {
    sn3DMeshModel * model = (sn3DMeshModel *)obj;
    m_object = obj;
	mainAxis = model->GetMainAxis();
	secoAxis = model->GetSecoAxis();
	thirAxis = model->GetThirAxis();
	center = model->GetCenter();
	mainAxis = Point3f(1.0,0.0,0.0);
	secoAxis = Point3f(0.0,1.0,0.0);
	thirAxis = Point3f(0.0,0.0,1.0);
	Initialize(mainAxis,secoAxis,thirAxis,center);
	m_Size = model->BBox.Diag()/3;

   }
}
void  MotionWidget::Initialize(Point3f mainAxis,Point3f secoAxis,Point3f thirAxis,Point3f center)
{
  m_OriginMainAxis = mainAxis;
  m_OriginSecoAxis = secoAxis;
  m_OriginThirAxis = thirAxis;
  m_CurMainAxis = mainAxis;
  m_CurSecoAxis = secoAxis;
  m_CurThirAxis = thirAxis;
  m_Center = center;
  m_Quaternion.X() = m_Quaternion.Y() = m_Quaternion.Z() = 0.0;
  m_Quaternion.W() = 1.0;
  for(int i=0;i<16;i++) m_Matrix[i]=0;
  m_Matrix[0]=1;
  m_Matrix[5]=1;
  m_Matrix[10]=1;
  m_Matrix[15]=1;
  
  float degree1,degree2,degree3;//弧度
  float co;
  Point3f dir;
  
  dir = Point3f(1.0,0.0,0.0);
  m_Axis1 = dir ^ m_OriginMainAxis;//旋转轴
  co = dir * m_OriginMainAxis;
  if(co ==1)m_Degree1 = 0;
  else if(co == -1) m_Degree1 = 180;
  else m_Degree1 = 180 *acos(co)/3.1415927;
  degree1 = m_Degree1*3.1415927/180;
  //计算绕任意轴旋转矩阵
  m_Mat1[0] = cos(degree1)+(1-cos(degree1))*m_Axis1.X()*m_Axis1.X();
  m_Mat1[1] = (1-cos(degree1))*m_Axis1.X()*m_Axis1.Y() - m_Axis1.Z()*sin(degree1);
  m_Mat1[2] = (1-cos(degree1))*m_Axis1.X()*m_Axis1.Z() + m_Axis1.Y()*sin(degree1);
  m_Mat1[3] = (1-cos(degree1))*m_Axis1.X()*m_Axis1.Y() + m_Axis1.Z()*sin(degree1);
  m_Mat1[4] = cos(degree1) + (1-cos(degree1))*m_Axis1.Y()*m_Axis1.Y();
  m_Mat1[5] = (1-cos(degree1))*m_Axis1.Y()*m_Axis1.Z() - m_Axis1.X()*sin(degree1);
  m_Mat1[6] = (1-cos(degree1))*m_Axis1.X()*m_Axis1.Z() - m_Axis1.Y()*sin(degree1);
  m_Mat1[7] = (1-cos(degree1))*m_Axis1.Y()*m_Axis1.Z() + m_Axis1.X()*sin(degree1);
  m_Mat1[8] = cos(degree1) + (1-cos(degree1))*m_Axis1.Z()*m_Axis1.Z(); 


  dir = Point3f(0.0,1.0,0.0);
  m_Axis2 = dir ^ m_OriginSecoAxis;//旋转轴
  co = dir * m_OriginSecoAxis;
  if(co ==1)m_Degree2 = 0;
  else if(co == -1) m_Degree2 = 180;
  else m_Degree2 = 180 *acos(co)/3.1415927;
  degree2 = m_Degree2*3.1415927/180;
  //计算绕任意轴旋转矩阵
  m_Mat2[0] = cos(degree2)+(1-cos(degree2))*m_Axis2.X()*m_Axis2.X();
  m_Mat2[1] = (1-cos(degree2))*m_Axis2.X()*m_Axis2.Y() - m_Axis2.Z()*sin(degree2);
  m_Mat2[2] = (1-cos(degree2))*m_Axis2.X()*m_Axis2.Z() + m_Axis2.Y()*sin(degree2);
  m_Mat2[3] = (1-cos(degree2))*m_Axis2.X()*m_Axis2.Y() + m_Axis2.Z()*sin(degree2);
  m_Mat2[4] = cos(degree2) + (1-cos(degree2))*m_Axis2.Y()*m_Axis2.Y();
  m_Mat2[5] = (1-cos(degree2))*m_Axis2.Y()*m_Axis2.Z() - m_Axis2.X()*sin(degree2);
  m_Mat2[6] = (1-cos(degree2))*m_Axis2.X()*m_Axis2.Z() - m_Axis2.Y()*sin(degree2);
  m_Mat2[7] = (1-cos(degree2))*m_Axis2.Y()*m_Axis2.Z() + m_Axis2.X()*sin(degree2);
  m_Mat2[8] = cos(degree2) + (1-cos(degree2))*m_Axis2.Z()*m_Axis2.Z(); 

  dir = Point3f(0.0,0.0,1.0);
  m_Axis3 = dir ^ m_OriginThirAxis;//旋转轴
  co = dir * m_OriginThirAxis;
  if(co ==1)m_Degree3 = 0;
  else if(co == -1) m_Degree3 = 180;
  else m_Degree3 = 180 *acos(co)/3.1415927;
  degree3 = m_Degree3*3.1415927/180;
  //计算绕任意轴旋转矩阵
  m_Mat3[0] = cos(degree3)+(1-cos(degree3))*m_Axis3.X()*m_Axis3.X();
  m_Mat3[1] = (1-cos(degree3))*m_Axis3.X()*m_Axis3.Y() - m_Axis3.Z()*sin(degree3);
  m_Mat3[2] = (1-cos(degree3))*m_Axis3.X()*m_Axis3.Z() + m_Axis3.Y()*sin(degree3);
  m_Mat3[3] = (1-cos(degree3))*m_Axis3.X()*m_Axis3.Y() + m_Axis3.Z()*sin(degree3);
  m_Mat3[4] = cos(degree3) + (1-cos(degree3))*m_Axis3.Y()*m_Axis3.Y();
  m_Mat3[5] = (1-cos(degree3))*m_Axis3.Y()*m_Axis3.Z() - m_Axis3.X()*sin(degree3);
  m_Mat3[6] = (1-cos(degree3))*m_Axis3.X()*m_Axis3.Z() - m_Axis3.Y()*sin(degree3);
  m_Mat3[7] = (1-cos(degree3))*m_Axis3.Y()*m_Axis3.Z() + m_Axis3.X()*sin(degree3);
  m_Mat3[8] = cos(degree3) + (1-cos(degree3))*m_Axis3.Z()*m_Axis3.Z(); 

}
void  MotionWidget::DrawWidget()
{
  glPushAttrib (GL_LINE_BIT|GL_TRANSFORM_BIT |GL_ENABLE_BIT | GL_CURRENT_BIT );
  glEnable (GL_LINE_SMOOTH);
  glLineWidth (5.5);
  glEnable (GL_BLEND);
  glEnable(GL_COLOR_MATERIAL);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix ();
  
  glTranslatef (m_Center.X(),m_Center.Y(),m_Center.Z()); //运动系旋转、平移、缩放变换
  glScalef(m_Size,m_Size,m_Size);
  glMultMatrixf (m_Matrix);//四元数旋转矩阵
   
  // x方向
  glPushMatrix();
  glRotatef(m_Degree1,m_Axis1.X(),m_Axis1.Y(),m_Axis1.Z());
  if(m_Active[0])
  glColor4f(1.0,1.0,0.0,1.0);
  else glColor4f(1.0,0.0,0.0,1.0);
  DrawCircle ();
  if(m_Active[1])
  glColor4f(1.0,1.0,0.0,1.0);
  else glColor4f(1.0,0.0,0.0,1.0);
  DrawSegment();
  glTranslatef(1.3,0.0,0.0);
  m_SceneObject.Cone(50,0.2,0.05);
  glPopMatrix(); 
 
  //y方向
  glPushMatrix();
  glRotatef(m_Degree2,m_Axis2.X(),m_Axis2.Y(),m_Axis2.Z());
  glRotatef(90,0,0,1);
  if(m_Active[2])
  glColor4f(1.0,1.0,0.0,1.0);
  else glColor4f(0.0,1.0,0.0,1.0);
  DrawCircle ();
  if(m_Active[3])
  glColor4f(1.0,1.0,0.0,1.0);
  else glColor4f(0.0,1.0,0.0,1.0);
  DrawSegment();
  glTranslatef(1.3,0.0,0.0);
  m_SceneObject.Cone(50,0.2,0.05);
  glPopMatrix();

  //z方向
  glPushMatrix();
  glRotatef(m_Degree3,m_Axis3.X(),m_Axis3.Y(),m_Axis3.Z());
  glRotatef(-90,0,1,0);
  if(m_Active[4])
  glColor4f(1.0,1.0,0.0,1.0);
  else glColor4f(0.0,0.0,1.0,1.0);
  DrawCircle ();
  if(m_Active[5])
  glColor4f(1.0,1.0,0.0,1.0);
  else glColor4f(0.0,0.0,1.0,1.0);
  DrawSegment();
  glTranslatef(1.3,0.0,0.0);
  m_SceneObject.Cone(50,0.2,0.05);
  glPopMatrix();

  glPopMatrix ();
  glPopAttrib ();

}


void  MotionWidget::DrawCircle ()
{
  int nside = 99;
  const double pi2 = 3.14159265 * 2.0;
  glBegin (GL_LINE_LOOP);
  for (double i = 0; i < nside; i++) {
    glNormal3d (0.0,cos (i * pi2 / nside), sin (i * pi2 / nside));
    glVertex3d (0.0,cos (i * pi2 / nside), sin (i * pi2 / nside));
  }
  glEnd ();
}
void  MotionWidget::DrawSegment()
{
   glPushMatrix();
   m_SceneObject.Cylinder(50,1.3,0.01);
   glPopMatrix();
   glPushMatrix();
   glRotatef(180,0,1,0);
   m_SceneObject.Cylinder(50,1.3,0.01);
   glPopMatrix();

}
void  MotionWidget::HitTest()
{
  //HitTest函数判断是否激活控件
   int nside = 99;
   const double pi2 = 3.14159265 * 2.0;
   float angle;
   float P[2];
   float Q[2];

   //获取选择缓存

   for(int i=0;i<6;i++) m_Active[i]=false;
   m_IsActive = false;
   glSelectBuffer(400, m_SelectBuf);
   glRenderMode(GL_SELECT);
   glInitNames();//初始化名字栈
   glPushName(-1);
		
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();	
   gluPickMatrix(m_CurPoint.x, m_CurPoint.y, 6, 6, m_Viewport);
   glMultMatrixd(m_ProjMatrix);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix(); 
   glTranslatef (m_Center.X(),m_Center.Y(),m_Center.Z());
   glScalef(m_Size,m_Size,m_Size);
   glMultMatrixf(m_Matrix);

   // x方向
   P[0] =cosf(0.0);Q[0] =sinf(0.0);
   angle = 0;
   glPushMatrix();
   glRotatef(m_Degree1,m_Axis1.X(),m_Axis1.Y(),m_Axis1.Z());
   for (double i = 1; i <=nside; i++)
  {
    angle = i * pi2 / nside;
    if(i==nside) angle=0;
    P[1] = cos(angle);
	Q[1] = sin(angle);
	glLoadName(100+i);
	glBegin (GL_LINES);
    glVertex3f(0.0,P[0],Q[0]);//右手关系
	glVertex3f(0.0,P[1],Q[1]);
	glEnd ();
	P[0] = P[1];
	Q[0] = Q[1];
  }
   glLoadName(1);
   DrawSegment();
   glTranslatef(1.3,0.0,0.0);
   m_SceneObject.Cone(50,0.2,0.05);
   glPopMatrix(); 

  //y方向
   P[0] =cosf(0.0);Q[0] =sinf(0.0);
   angle = 0;
   glPushMatrix();
   glRotatef(m_Degree2,m_Axis2.X(),m_Axis2.Y(),m_Axis2.Z());
   for (double i = 1; i <=nside; i++)
  {
    angle = i * pi2 / nside;
    if(i==nside) angle=0;
    P[1] = cos(angle);
	Q[1] = sin(angle);
	glLoadName(200+i);
	glBegin (GL_LINES);
    glVertex3f(P[0],0.0,Q[0]);
	glVertex3f(P[1],0.0,Q[1]);
	glEnd ();
	P[0] = P[1];
	Q[0] = Q[1];
  }
   glLoadName(2);
   glRotatef(90,0,0,1);
   DrawSegment();
   glTranslatef(1.3,0.0,0.0);
   m_SceneObject.Cone(50,0.2,0.05);
   glPopMatrix();

 
  //z方向
   P[0] =cosf(0.0);Q[0] =sinf(0.0);
   angle = 0;
   glPushMatrix();
   glRotatef(m_Degree3,m_Axis3.X(),m_Axis3.Y(),m_Axis3.Z());
   for (double i = 1; i <=nside; i++)
  {
    angle = i * pi2 / nside;
    if(i==nside) angle=0;
    P[1] = cos(angle);
	Q[1] = sin(angle);
	glLoadName(300+i);
	glBegin (GL_LINES);
    glVertex3f(P[0],Q[0],0.0);
	glVertex3f(P[1],Q[1],0.0);
	glEnd ();
	P[0] = P[1];
	Q[0] = Q[1];
  }
   glLoadName(3);
   glRotatef(-90,0,1,0);
   DrawSegment();
   glTranslatef(1.3,0.0,0.0);
   m_SceneObject.Cone(50,0.2,0.05);
   glPopMatrix();

   glPopMatrix ();

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
		
   long hits = glRenderMode(GL_RENDER);
    
   Point3f point,point1;
   float len,diffX,diffY,diffZ;
	//如果拾取到,寻找旋转点
	if(hits>0)
	{
	  m_IsActive = true;//控件被激活

     if(m_SelectBuf[3]>100&&m_SelectBuf[3]<200)//绕x轴
	 {
       m_Active[0] = true;

	   angle = (m_SelectBuf[3]-100) * pi2/nside;
	  
	   point1 = Point3f(0.0,cos(angle),sin(angle));

	   point.X() = m_Mat1[0]*point1.X() + m_Mat1[1]*point1.Y() + m_Mat1[2]*point1.Z();
	   point.Y() = m_Mat1[3]*point1.X() + m_Mat1[4]*point1.Y() + m_Mat1[5]*point1.Z();
	   point.Z() = m_Mat1[6]*point1.X() + m_Mat1[7]*point1.Y() + m_Mat1[8]*point1.Z();

	   m_RotatePoint.X() = m_Matrix[0]*point.X() + m_Matrix[4]*point.Y() + m_Matrix[8]*point.Z();//旋转
	   m_RotatePoint.Y() = m_Matrix[1]*point.X() + m_Matrix[5]*point.Y() + m_Matrix[9]*point.Z();
	   m_RotatePoint.Z() = m_Matrix[2]*point.X() + m_Matrix[6]*point.Y() + m_Matrix[10]*point.Z();

       diffX = m_RotatePoint.X()*m_RotatePoint.X();diffY = m_RotatePoint.Y()*m_RotatePoint.Y();diffZ = m_RotatePoint.Z()*m_RotatePoint.Z();
       len = sqrt(diffX+diffY+diffZ);
	   m_RotatePoint.X() /= len; m_RotatePoint.Y() /= len; m_RotatePoint.Z() /= len;
       

	   m_RotatePoint.X() *= m_Size;  //缩放
	   m_RotatePoint.Y() *= m_Size;
	   m_RotatePoint.Z() *= m_Size;
	   m_RotatePoint.X() += m_Center.X();//平移
	   m_RotatePoint.Y() += m_Center.Y();
	   m_RotatePoint.Z() += m_Center.Z();
	 }
	 else if(m_SelectBuf[3]>200&&m_SelectBuf[3]<300)//绕y轴
	 {
	   m_Active[2] = true;

	   angle = (m_SelectBuf[3]-200)* pi2/nside;
	  
	   point1 = Point3f(cos(angle),0.0,sin(angle));

	   point.X() = m_Mat2[0]*point1.X() + m_Mat2[1]*point1.Y() + m_Mat2[2]*point1.Z();
	   point.Y() = m_Mat2[3]*point1.X() + m_Mat2[4]*point1.Y() + m_Mat2[5]*point1.Z();
	   point.Z() = m_Mat2[6]*point1.X() + m_Mat2[7]*point1.Y() + m_Mat2[8]*point1.Z();

	   m_RotatePoint.X() = m_Matrix[0]*point.X() + m_Matrix[4]*point.Y() + m_Matrix[8]*point.Z();//旋转
	   m_RotatePoint.Y() = m_Matrix[1]*point.X() + m_Matrix[5]*point.Y() + m_Matrix[9]*point.Z();
	   m_RotatePoint.Z() = m_Matrix[2]*point.X() + m_Matrix[6]*point.Y() + m_Matrix[10]*point.Z();

	   diffX = m_RotatePoint.X()*m_RotatePoint.X();diffY = m_RotatePoint.Y()*m_RotatePoint.Y();diffZ = m_RotatePoint.Z()*m_RotatePoint.Z();
       len = sqrt(diffX+diffY+diffZ);
	   m_RotatePoint.X() /= len; m_RotatePoint.Y() /= len; m_RotatePoint.Z() /= len;

	   m_RotatePoint.X() *= m_Size; //缩放
	   m_RotatePoint.Y() *= m_Size;
	   m_RotatePoint.Z() *= m_Size;
	   m_RotatePoint.X() += m_Center.X();//平移
	   m_RotatePoint.Y() += m_Center.Y();
	   m_RotatePoint.Z() += m_Center.Z();

	 }
	 else if(m_SelectBuf[3]>300)//绕z轴
	 {
      m_Active[4] = true;

	   angle = (m_SelectBuf[3]-300)* pi2/nside;
	  
	   point1 = Point3f(cos(angle),sin(angle),0.0);

	   point.X() = m_Mat3[0]*point1.X() + m_Mat3[1]*point1.Y() + m_Mat3[2]*point1.Z();
	   point.Y() = m_Mat3[3]*point1.X() + m_Mat3[4]*point1.Y() + m_Mat3[5]*point1.Z();
	   point.Z() = m_Mat3[6]*point1.X() + m_Mat3[7]*point1.Y() + m_Mat3[8]*point1.Z();

	   m_RotatePoint.X() = m_Matrix[0]*point.X() + m_Matrix[4]*point.Y() + m_Matrix[8]*point.Z();//旋转
	   m_RotatePoint.Y() = m_Matrix[1]*point.X() + m_Matrix[5]*point.Y() + m_Matrix[9]*point.Z();
	   m_RotatePoint.Z() = m_Matrix[2]*point.X() + m_Matrix[6]*point.Y() + m_Matrix[10]*point.Z();

	   diffX = m_RotatePoint.X()*m_RotatePoint.X();diffY = m_RotatePoint.Y()*m_RotatePoint.Y();diffZ = m_RotatePoint.Z()*m_RotatePoint.Z();
       len = sqrt(diffX+diffY+diffZ);
	   m_RotatePoint.X() /= len; m_RotatePoint.Y() /= len; m_RotatePoint.Z() /= len;

	   m_RotatePoint.X() *= m_Size;//缩放
	   m_RotatePoint.Y() *= m_Size;
	   m_RotatePoint.Z() *= m_Size;
	   m_RotatePoint.X() += m_Center.X();//平移
	   m_RotatePoint.Y() += m_Center.Y();
	   m_RotatePoint.Z() += m_Center.Z();

	 }
	 else//如果是平移
	 {
      if(m_SelectBuf[3]==1)
		  m_Active[1] =true;
	  if(m_SelectBuf[3]==2)
		  m_Active[3]=true;
	  if(m_SelectBuf[3]==3)
		  m_Active[5]=true;
	  m_RotatePoint = m_Center;
	 }
	}  
}
void  MotionWidget::Transform()
{
 if(m_object==NULL)return;

 if(m_object->IsOfType(sn3DCore::sn3DObject::TYPE_MESH) )
 {
   sn3DMeshModel * model = (sn3DMeshModel *)m_object;
   
   /////  变换模型

   Point3f mainAxis,secoAxis,thirAxis;
   Point3f center;

   Motion(mainAxis,secoAxis,thirAxis,center);

   model->m_motion.SetTransform(m_Matrix);

   Point3f translate;

   center = model->GetCenter();

   translate = m_Center - center;

   model->m_motion.SetTranslate(translate);
  }
}
void  MotionWidget::Motion(Point3f & mainAxis,Point3f & secoAxis,Point3f & thirAxis,Point3f & center)
{
  float distance;
  Point3f dir,side;
  float length;

  if(m_PrevPoint == m_CurPoint) return;

  if(m_Active[0]) //绕x转
  {
   side = m_RotatePoint - m_Center; 
   length =sqrt(side.X()*side.X() + side.Y()*side.Y() + side.Z()*side.Z());
   side.X() /= length; side.Y() /= length; side.Z() /= length;
   dir = m_CurMainAxis ^ side; //计算旋转切线方向

   distance = UnProjectDistance(dir); //计算旋转弧长
   Rotate(distance/m_Size,1);//旋转轴方向
  }
  if(m_Active[1])//沿x平移
  {
   distance = UnProjectDistance(m_CurMainAxis);
   Move(distance,1);
  }
  if(m_Active[2])//绕y转
  {
   side = m_RotatePoint - m_Center; 
   length =sqrt(side.X()*side.X() + side.Y()*side.Y() + side.Z()*side.Z());
   side.X() /= length; side.Y() /= length; side.Z() /= length;
   dir = m_CurSecoAxis ^ side; //计算旋转切线方向

   distance = UnProjectDistance(dir); //计算旋转弧长
   Rotate(distance/m_Size,2);//旋转轴方向
  }
  if(m_Active[3])//沿y平移
  {
   distance = UnProjectDistance(m_CurSecoAxis);
   Move(distance,2);
  }
  if(m_Active[4])//绕z转
  {
   side = m_RotatePoint - m_Center; 
   length =sqrt(side.X()*side.X() + side.Y()*side.Y() + side.Z()*side.Z());
   side.X() /= length; side.Y() /= length; side.Z() /= length;
   dir = m_CurThirAxis ^ side; //计算旋转切线方向

   distance = UnProjectDistance(dir); //计算旋转弧长
   Rotate(distance/m_Size,3); //旋转轴方向
  }
  if(m_Active[5])//沿z平移
  {
   distance = UnProjectDistance(m_CurThirAxis);
   Move(distance,3);

  }

  mainAxis = m_CurMainAxis;
  secoAxis = m_CurSecoAxis;
  thirAxis = m_CurThirAxis;
  center = m_Center;
}
void  MotionWidget::Move(float distance,int axis)
{
  //平移中心点位置
  switch(axis)
  {
  case 1:
	  m_Center = m_Center +  m_CurMainAxis * distance;
	  break;
  case 2:
	  m_Center = m_Center +  m_CurSecoAxis * distance;
	  break;
  case 3:
	  m_Center = m_Center +  m_CurThirAxis * distance;
	  break;
  }
}
void  MotionWidget::Rotate(float degree,int axis) //degree是弧度
{

	float si;//正弦值

	Quaternion<float> originQ,curQ,tempQ; 

	//从绕轴旋转导出旋转四元数
	si = sin(degree/2);
	curQ.W()  = cos(degree/2);//计算旋转四元数,绕原轴的旋转角

	switch(axis)
	{
	case 1:
		curQ.X() = m_OriginMainAxis.X()*si;//计算旋转四元数
		curQ.Y() = m_OriginMainAxis.Y()*si;
		curQ.Z() = m_OriginMainAxis.Z()*si;	   
		break;

	case 2:
		curQ.X() = m_OriginSecoAxis.X()*si;//计算旋转四元数
		curQ.Y() = m_OriginSecoAxis.Y()*si;
		curQ.Z() = m_OriginSecoAxis.Z()*si;	   
		break;

	case 3:
		curQ.X() = m_OriginThirAxis.X()*si;//计算旋转四元数
		curQ.Y() = m_OriginThirAxis.Y()*si;
		curQ.Z() = m_OriginThirAxis.Z()*si;	   
		break;
	}

	originQ = m_Quaternion;

	QuaternionMultiply(m_Quaternion,originQ,curQ);

	m_Quaternion.Normalize();//四元数单位化

	//计算新的轴方向   
	Multiply(tempQ.W(),tempQ.X(),tempQ.Y(),tempQ.Z(),0.0,m_OriginMainAxis.X(),m_OriginMainAxis.Y(),m_OriginMainAxis.Z(),
		m_Quaternion.W(),-m_Quaternion.X(),-m_Quaternion.Y(),-m_Quaternion.Z());
	Multiply(tempQ.W(),m_CurMainAxis.X(),m_CurMainAxis.Y(),m_CurMainAxis.Z(),m_Quaternion.W(),m_Quaternion.X(),m_Quaternion.Y(),m_Quaternion.Z(),
		tempQ.W(),tempQ.X(),tempQ.Y(),tempQ.Z());

	Multiply(tempQ.W(),tempQ.X(),tempQ.Y(),tempQ.Z(),0.0,m_OriginSecoAxis.X(),m_OriginSecoAxis.Y(),m_OriginSecoAxis.Z(),
		m_Quaternion.W(),-m_Quaternion.X(),-m_Quaternion.Y(),-m_Quaternion.Z());
	Multiply(tempQ.W(),m_CurSecoAxis.X(),m_CurSecoAxis.Y(),m_CurSecoAxis.Z(),m_Quaternion.W(),m_Quaternion.X(),m_Quaternion.Y(),m_Quaternion.Z(),
		tempQ.W(),tempQ.X(),tempQ.Y(),tempQ.Z());

	Multiply(tempQ.W(),tempQ.X(),tempQ.Y(),tempQ.Z(),0.0,m_OriginThirAxis.X(),m_OriginThirAxis.Y(),m_OriginThirAxis.Z(),
		m_Quaternion.W(),-m_Quaternion.X(),-m_Quaternion.Y(),-m_Quaternion.Z());
	Multiply(tempQ.W(),m_CurThirAxis.X(),m_CurThirAxis.Y(),m_CurThirAxis.Z(),m_Quaternion.W(),m_Quaternion.X(),m_Quaternion.Y(),m_Quaternion.Z(),
		tempQ.W(),tempQ.X(),tempQ.Y(),tempQ.Z());

	//计算局部坐标架总旋转矩阵：四元数导出旋转矩阵
	float q00 = m_Quaternion.X()*m_Quaternion.X();
	float q01 = m_Quaternion.X()*m_Quaternion.Y();
	float q02 = m_Quaternion.X()*m_Quaternion.Z();
	float q03 = m_Quaternion.X()*m_Quaternion.W();
	float q11 = m_Quaternion.Y()*m_Quaternion.Y();
	float q12 = m_Quaternion.Y()*m_Quaternion.Z();
	float q13 = m_Quaternion.Y()*m_Quaternion.W();
	float q22 = m_Quaternion.Z()*m_Quaternion.Z();
	float q23 = m_Quaternion.Z()*m_Quaternion.W();

	m_Matrix[0]  = (float)(1.0-(q11 + q22)*2.0);//OpenGL方式存储
	m_Matrix[4]  = (float)((q01 - q23)*2.0);
	m_Matrix[8]  = (float)((q02 + q13)*2.0);
	m_Matrix[12] = (float)0.0;

	m_Matrix[1]  = (float)((q01 + q23)*2.0);
	m_Matrix[5]  = (float)(1.0-(q22 + q00)*2.0);
	m_Matrix[9]  = (float)((q12 - q03)*2.0);
	m_Matrix[13] = (float)0.0; 

	m_Matrix[2]  = (float)((q02 - q13)*2.0);
	m_Matrix[6]  = (float)((q12 + q03)*2.0);
	m_Matrix[10] = (float)(1.0-(q11 + q00)*2.0);
	m_Matrix[14] = (float)0.0;

	m_Matrix[3]  = (float)0.0;
	m_Matrix[7]  = (float)0.0;
	m_Matrix[11] = (float)0.0;
	m_Matrix[15] = (float)1.0;

}
void  MotionWidget::UpdateMatrix()
{
   glGetIntegerv(GL_VIEWPORT, m_Viewport);
   glGetDoublev (GL_MODELVIEW_MATRIX, m_MvMatrix);
   glGetDoublev (GL_PROJECTION_MATRIX, m_ProjMatrix);
}
void  MotionWidget::Multiply(float & newS,float & newQX,float & newQY,float & newQZ,
							 float  S,float  QX, float  QY , float  QZ,
							 float  s, float  qx, float qy , float  qz)
{
   //该函数为四元数乘法
   newS  = S*s - QX*qx - QY*qy - QZ*qz;
   newQX = QY*qz - QZ*qy + QX*s + S*qx;
   newQY = QZ*qx - QX*qz + QY*s + S*qy;
   newQZ = QX*qy - QY*qx + QZ*s + S*qz;

}
void  MotionWidget::DrawPoint()
{
  glPushAttrib (GL_COLOR_BUFFER_BIT |GL_TRANSFORM_BIT |GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
  glEnable (GL_LINE_SMOOTH);
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_COLOR_MATERIAL);
  glColor4f (1.0,0.0,0.3125,0.8125);
  
  glDisable(GL_TEXTURE_2D);
  glPushMatrix();

  glBegin(GL_LINES);
  glVertex3f(m_Center.X(),m_Center.Y(),m_Center.Z());
  glVertex3f(m_RotatePoint.X(),m_RotatePoint.Y(),m_RotatePoint.Z());

  glEnd();

  glTranslatef(m_RotatePoint.X(),m_RotatePoint.Y(),m_RotatePoint.Z());

  glutSolidSphere(0.2,10,10);
  glPopMatrix();
  glPopAttrib();

}
float MotionWidget::UnProjectDistance(Point3f dir)
{
  //该函数将窗口坐标平移转化为深度平面上平移在dir方向上的投影
  GLdouble endPoint[3];
  GLdouble wndPointBegin[3];
  GLdouble wndPointEnd[3];
  Point3f projDir;//dir在深度平面内的投影

  //计算dir在深度平面内的投影
  gluProject(m_RotatePoint.X(),m_RotatePoint.Y(),m_RotatePoint.Z(),m_MvMatrix,
		       m_ProjMatrix,m_Viewport,&wndPointBegin[0],&wndPointBegin[1],&wndPointBegin[2]);

  gluProject(m_RotatePoint.X()+dir.X(),m_RotatePoint.Y()+dir.Y(),m_RotatePoint.Z()+dir.Z(),m_MvMatrix,
		       m_ProjMatrix,m_Viewport,&wndPointEnd[0],&wndPointEnd[1],&wndPointEnd[2]);

  wndPointEnd[2] = wndPointBegin[2];

  gluUnProject(wndPointEnd[0],wndPointEnd[1],wndPointEnd[2],m_MvMatrix,m_ProjMatrix,m_Viewport,
	           &endPoint[0],&endPoint[1],&endPoint[2]);

  projDir.X() = endPoint[0] - m_RotatePoint[0];
  projDir.Y() = endPoint[1] - m_RotatePoint[1];
  projDir.Z() = endPoint[2] - m_RotatePoint[2];
  
  float length = sqrt(projDir.X()*projDir.X()+projDir.Y()*projDir.Y()+projDir.Z()*projDir.Z());

  projDir.X() /= length; projDir.Y() /= length; projDir.Z() /= length;
  
  //计算当前在深度平面内的操作位移
  GLdouble prevPoint[3];
  GLdouble curPoint[3];

  m_PrevPoint[2] = wndPointBegin[2];
  m_CurPoint[2] = wndPointBegin[2];

  gluUnProject(m_PrevPoint[0],m_PrevPoint[1],m_PrevPoint[2],m_MvMatrix,m_ProjMatrix,m_Viewport,
	           &prevPoint[0],&prevPoint[1],&prevPoint[2]);

  gluUnProject(m_CurPoint.x,m_CurPoint.y,m_CurPoint.z,m_MvMatrix,m_ProjMatrix,m_Viewport,
	           &curPoint[0],&curPoint[1],&curPoint[2]);

  Point3f moveDir;
  moveDir.X() = curPoint[0] - prevPoint[0];
  moveDir.Y() = curPoint[1] - prevPoint[1];
  moveDir.Z() = curPoint[2] - prevPoint[2];
  length = sqrt(moveDir.X()*moveDir.X()+moveDir.Y()*moveDir.Y()+moveDir.Z()*moveDir.Z());

  if(length<0.0000001) return 0;
  moveDir.X() /= length; moveDir.Y() /= length; moveDir.Z() /= length; 

  float distance;

  distance = length * (moveDir * projDir)/(projDir *dir);
  
  return distance;
}


}
