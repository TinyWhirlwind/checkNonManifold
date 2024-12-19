#ifndef _RANGECURVATURE_
#define _RANGECURVATURE_
#include<math.h>
#include<vector>
#include"Point3.h"
#include"../sn3DDataModel/sn3DMeshModel.h"
namespace sn3DCore
{
class LocalFrame//局部标架
{
public:
	LocalFrame()
	{k1=k2=k12=0;
	uDir.X()=uDir.Y()=uDir.Z()=0;
	vDir.X()=vDir.Y()=vDir.Z()=0;
	normal.X()=normal.Y()=normal.Z()=0;
	}
	Point3f uDir;//局部标架
	Point3f vDir;
	Point3f normal;
	float k1;  //曲率张量
	float k2;
	float k12;
};

class RangeCurvature
{
public:
    
   void ComputerCurvatrue(sn3DMeshData * model);
   
   void VertexCurvature(sn3DMeshData * model);
   
   void ComputeVertexArea(sn3DMeshData * model);
   
   void ProjectCurvature(LocalFrame & faceFrame ,LocalFrame & vertexFrame);
   
   void DiagonalizeCurvature(LocalFrame & frame);

   void choldc(double a[][3], Point3f & p);
   
   void cholsl(double a[][3], Point3f & p, Point3f & b, Point3f & x);

   void Clear();

public:
     
	RangeCurvature();
	
	~RangeCurvature();

public:

   LocalFrame * m_Frame;//曲率信息
   
   Point3f * m_FaceCornerArea;
   
   float *  VertexArea;
};

}
#endif