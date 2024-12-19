#ifndef _BASEIMPORTER_
#define _BASEIMPORTER_

#include <vector>

#include"Common/Point3.h"
#include"Common/Point2.h"
#include"Common/Color4.h"
#include"SceneObject/SceneObject.h"

using namespace sn3DCore;

class BaseImporter
{
public:

  virtual void Update(){};         //  输入后更新操作

  virtual	SceneObject * GetObjectPointer(){return 0;}

  virtual void ReserveFace(int num) {};
	
  virtual void ReserveVertex(int num) {};

  virtual void AddVertex(Point3f& _point) {};     //  增加顶点

  virtual void AddVertex(Point3f& _point, Color4f& _color) {};     //  增加顶点

  virtual void AddFace(std::vector<int> & _vertices) {};//  增加面  

  virtual void AddFace(int v0,int v1,int v2) {};        //  增加面

  virtual void AddFace(int v0,int v1,int v2,bool mult) {};        //  增加面

  virtual void AddFaceTexCoord(Point2f p0,Point2f p1,Point2f p2){};

  virtual void AddVertexNormal(Point3f normal){};

  virtual void AddVertexColor(Color4f & color){};

  virtual size_t n_vertices()   {return 0;}      //  获取点数
  
  virtual size_t n_faces()      {return 0;}      //  获取面数

  virtual ~BaseImporter() {}

  bool  IsRedundant()	{return (m_flag & REDUNDANT)!=0;}	//  冗余位 

  void  SetRedundant()	{ m_flag |= REDUNDANT;}	                     	
  
  void  ClearRedundant(){m_flag &= (~REDUNDANT);}

  virtual void SetTexId(unsigned int id){m_texId = id;}

  enum 
  { 		
    REDUNDANT          = 0x01,  //  是否去除冗余点	
   };
		
  int	m_flag;            //  标志位

  unsigned int   m_texId;

  int m_w;

  int m_h;

  unsigned char * m_imageData;

  int m_imageChannel;
};
#endif

