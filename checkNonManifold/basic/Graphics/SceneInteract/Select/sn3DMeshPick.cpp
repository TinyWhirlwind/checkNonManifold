#pragma warning (disable : 4244)
#include"sn3DMeshPick.h"
#include"../../../Common/Plane3.h"
#include"../../SceneGraph/sn3DMeshModel.h"
#include <windows.h>
#include<GL/gl.h>
#include<GL/glu.h>

namespace sn3DCore
{
sn3DMeshPick::sn3DMeshPick(sn3DMeshModel * model)
{
	m_SpaceBox.Initialize(model->GetMeshData(),50);
}
void sn3DMeshPick::PaintVertex(sn3DObject * mesh,Quaternionf rot,int flag)
{

	sn3DMeshModel * model = (sn3DMeshModel*)mesh;
	sn3DMeshData * data = model->GetMeshData();

	Point3f dir;
	Point3f dir1,dir2,dir3;
	std::vector<Plane3f> plane;
	std::vector<Point3f> points;
	Quaternionf rotate;
	Quaternionf newQ,reverse;
	Quaternionf oDir,nDir;


	GLdouble p[3];
	gluUnProject(m_CurPoint.x,m_CurPoint.y,0.0,m_MvMatrix,m_ProjMatrix,m_Viewport,&p[0],&p[1],&p[2]);


	Point3f center;

	center.X() = p[0];
	center.Y() = p[1];
	center.Z() = p[2];

	rotate.W()= rot.V(0);
	rotate.X()= rot.V(1);
	rotate.Y()= rot.V(2);
	rotate.Z()= rot.V(3);

	reverse.X() = -rotate.X();
	reverse.Y() = -rotate.Y();
	reverse.Z() = -rotate.Z();
	reverse.W() = rotate.W();

	dir = Point3f(0.0,0.0,-1.0);

	oDir.X() = dir.X();
	oDir.Y() = dir.Y();
	oDir.Z() = dir.Z();
	oDir.W() = 0.0;

	QuaternionMultiply(newQ,oDir,reverse);
	QuaternionMultiply(nDir,rotate,newQ);

	dir.X() = nDir.X();
	dir.Y() = nDir.Y();
	dir.Z() = nDir.Z();
	dir.Normalize();


	int scale = m_SpaceBox.m_DivisionNum;

	for (int u=0;u<scale;u++)
		for(int v=0;v<scale;v++)
			for (int w=0;w<scale;w++)
			{
				if (m_SpaceBox.GetEntrySize(u,v,w)==-1)continue;

				int size;

				bool inspace[8];

				size = m_SpaceBox.GetEntrySize(u,v,w);

				bool skip = true;

				int position = u*m_SpaceBox.m_DivisionNum*m_SpaceBox.m_DivisionNum+v*m_SpaceBox.m_DivisionNum+w;
				for(int j=0;j<size;j++)
				{
					int item;
					//item = mesh->m_SpaceBox.GetEntryItem(u,v,w,j);
					int pos = m_SpaceBox.m_EntryMap[position];
                    item = m_SpaceBox.m_DataPool[m_SpaceBox.m_SpaceBox[pos].m_BeginIndex+j];		
					if (!data->V(item)->IsS())
					{
						skip = false;
						break;
					}
				}
				if (skip)continue;

				Point3f minPoint,maxPoint;

				Point3f inter;
				inter.X() = 1/m_SpaceBox.m_Delta.X();
				inter.Y() = 1/m_SpaceBox.m_Delta.Y();
				inter.Z() = 1/m_SpaceBox.m_Delta.Z();

				minPoint.X() = m_SpaceBox.m_MinCorner.X()  + u*inter.X();
				minPoint.Y() = m_SpaceBox.m_MinCorner.Y()  + v*inter.Y();
				minPoint.Z() = m_SpaceBox.m_MinCorner.Z()  + w*inter.Z();

				maxPoint.X() = m_SpaceBox.m_MinCorner.X()  + (u+1)*inter.X();
				maxPoint.Y() = m_SpaceBox.m_MinCorner.Y()  + (v+1)*inter.Y();
				maxPoint.Z() = m_SpaceBox.m_MinCorner.Z()  + (w+1)*inter.Z();

				Point3f corner[8];

				corner[0].X() = minPoint.X();
				corner[0].Y() = minPoint.Y();
				corner[0].Z() = minPoint.Z();
				corner[1].X() = minPoint.X();
				corner[1].Y() = minPoint.Y();
				corner[1].Z() = maxPoint.Z();
				corner[2].X() = minPoint.X();
				corner[2].Y() = maxPoint.Y();
				corner[2].Z() = minPoint.Z();
				corner[3].X() = minPoint.X();
				corner[3].Y() = maxPoint.Y();
				corner[3].Z() = maxPoint.Z();

				corner[4].X() = maxPoint.X();
				corner[4].Y() = minPoint.Y();
				corner[4].Z() = minPoint.Z();
				corner[5].X() = maxPoint.X();
				corner[5].Y() = minPoint.Y();
				corner[5].Z() = maxPoint.Z();
				corner[6].X() = maxPoint.X();
				corner[6].Y() = maxPoint.Y();
				corner[6].Z() = minPoint.Z();
				corner[7].X() = maxPoint.X();
				corner[7].Y() = maxPoint.Y();
				corner[7].Z() = maxPoint.Z();

				Point3f diff;
				float dis1;
				float dis2;

				for (int l=0;l<8;l++)
				{
					inspace[l] = true;

					diff= corner[l] - center;
					dis1 = diff.X()*diff.X()+diff.Y()*diff.Y()+diff.Z()*diff.Z();
				    dis2 = dir.X()*diff.X()+dir.Y()*diff.Y()+dir.Z()*diff.Z();
					dis2*=dis2;

				    if(dis1 - dis2> m_PaintRadius2)
					{
					  inspace[l] =false;
					  break;
					}
				}

				if(inspace[0]&&inspace[1]&&inspace[2]&&inspace[3]&&inspace[4]&&inspace[5]&&inspace[6]&&inspace[7])
				{
					int position = u*m_SpaceBox.m_DivisionNum*m_SpaceBox.m_DivisionNum+v*m_SpaceBox.m_DivisionNum+w;
					for(int j=0;j<size;j++)
					{
					  int item;
					   //item = mesh->m_SpaceBox.GetEntryItem(u,v,w,j);
					  int pos = m_SpaceBox.m_EntryMap[position];
                      item = m_SpaceBox.m_DataPool[m_SpaceBox.m_SpaceBox[pos].m_BeginIndex+j];	

					  if(flag==1)
				      data->V(item)->SetS();
				  

					}
				}
				else if(inspace[0]||inspace[1]||inspace[2]||inspace[3]||inspace[4]||inspace[5]||inspace[6]||inspace[7])
				{
					int position = u*m_SpaceBox.m_DivisionNum*m_SpaceBox.m_DivisionNum+v*m_SpaceBox.m_DivisionNum+w;
					for(int j=0;j<size;j++)
					{
						int item;
						//item = mesh->m_SpaceBox.GetEntryItem(u,v,w,j);
						int pos = m_SpaceBox.m_EntryMap[position];
                        item = m_SpaceBox.m_DataPool[m_SpaceBox.m_SpaceBox[pos].m_BeginIndex+j];		

						if(data->V(item)->IsS())continue;

						Point3f pp = data->V(item)->m_coord;

						bool pointinspace = true;
						
						diff= pp - center;
						dis1 = diff.X()*diff.X()+diff.Y()*diff.Y()+diff.Z()*diff.Z();
						dis2 = dir.X()*diff.X()+dir.Y()*diff.Y()+dir.Z()*diff.Z();
						dis2*=dis2;

						 if(dis1 - dis2> m_PaintRadius2)pointinspace=false;

						if (pointinspace)
						//if (mesh->V(item)->N()*dir<0)	
						{
				
						 if(flag==1)
				         data->V(item)->SetS();
				      
						}
					}
				}

			}

}
void sn3DMeshPick::SelectPolygonVertex(sn3DObject * m,Quaternionf rot,int flag)
{
    sn3DMeshModel * model = (sn3DMeshModel*)m;
	sn3DMeshData * data = model->GetMeshData();

    Point3f dir;
	Point3f dir1,dir2,dir3;
	Quaternionf rotate;
	Quaternionf newQ,reverse;
	Quaternionf result;
	std::vector<Point2f> polygon;
	Point3f pp;
	Point2i pos;
	int n;
	int end = (int)m_Polygon.size();
	Point2f wmin,wmax;

	for(int i=0;i<end-1;i++)
	{
      dir.X() = m_Polygon[i+1].X() - m_Polygon[i].X();
	  dir.Y() = m_Polygon[i+1].Y() - m_Polygon[i].Y();
	  n  = abs(dir.X())>abs(dir.Y())?abs(dir.X()):abs(dir.Y());

	  dir.X()/=((float)n);
	  dir.Y()/=((float)n);

	  pp.X() = m_Polygon[i].X();
	  pp.Y() = m_Polygon[i].Y();
	   
	  for(int j=0;j<n;j++)
	  {
	   pp.X() += dir.X();
	   pp.Y() += dir.Y();
	   //pos.X() = pp.X();
	   //pos.Y() = pp.Y();
       polygon.push_back(Point2f(pp.X(),pp.Y()));
	  }
	}
	
	int num;

	std::vector<Point3f> rpoints;
	num = polygon.size();

	wmax = wmin = polygon[0];
	for (int k=1;k<num;k++)
	{
      if(wmax.X()<polygon[k].X())  wmax.X() = polygon[k].X();
	  if(wmax.Y()<polygon[k].Y())  wmax.Y() = polygon[k].Y();
	  if(wmin.X()>polygon[k].X())  wmin.X() = polygon[k].X();
	  if(wmin.Y()>polygon[k].Y())  wmin.Y() = polygon[k].Y();
	}

	points.resize(num);
	for (int k=0;k<num;k++)
	{
	  GLdouble p[3];
	  gluUnProject(polygon[k].X(),polygon[k].Y(),0.0,m_MvMatrix,m_ProjMatrix,m_Viewport,&p[0],&p[1],&p[2]);

	  points[k].X() = p[0];
	  points[k].Y() = p[1];
	  points[k].Z() = p[2];
	}
	points.push_back(points.front());

	rotate.W()= rot.V(0);
	rotate.X()= rot.V(1);
	rotate.Y()= rot.V(2);
	rotate.Z()= rot.V(3);

	reverse.X() = -rotate.X();
	reverse.Y() = -rotate.Y();
	reverse.Z() = -rotate.Z();
	reverse.W() = rotate.W();


	rpoints = points;
	num = rpoints.size();
	for(int i=0;i<num;i++)
	{
     Quaternionf  point;
	 point.X() = rpoints[i].X();
	 point.Y() = rpoints[i].Y();
	 point.Z() = rpoints[i].Z();
	 QuaternionMultiply(newQ,point,rotate);
	 QuaternionMultiply(result,reverse,newQ);
	 rpoints[i].X() = result.X();
	 rpoints[i].Y() = result.Y();
	 rpoints[i].Z() = result.Z();
	}

	Quaternionf oDir,nDir;

	std::vector<Plane3f> plane;
	plane.resize(4);
	GLdouble p[3];
	Point3f ki[5];
	gluUnProject(wmin.X(),wmin.Y(),0.0,m_MvMatrix,m_ProjMatrix,m_Viewport,&p[0],&p[1],&p[2]);
	ki[0].X() = p[0];ki[0].Y() = p[1];ki[0].Z() = p[2];
	gluUnProject(wmax.X(),wmin.Y(),0.0,m_MvMatrix,m_ProjMatrix,m_Viewport,&p[0],&p[1],&p[2]);
	ki[1].X() = p[0];ki[1].Y() = p[1];ki[1].Z() = p[2];
	gluUnProject(wmax.X(),wmax.Y(),0.0,m_MvMatrix,m_ProjMatrix,m_Viewport,&p[0],&p[1],&p[2]);
	ki[2].X() = p[0];ki[2].Y() = p[1];ki[2].Z() = p[2];
	gluUnProject(wmin.X(),wmax.Y(),0.0,m_MvMatrix,m_ProjMatrix,m_Viewport,&p[0],&p[1],&p[2]);
	ki[3].X() = p[0];ki[3].Y() = p[1];ki[3].Z() = p[2];

	ki[4] = ki[0];

	dir1 = ki[1] - ki[0];
	dir2 = ki[2] - ki[1];
	dir3 = dir1 ^ dir2;
	oDir.X() = 0.0;
	oDir.Y() = 0.0;
	oDir.Z() = 1.0;
	oDir.W() = 0.0;
	QuaternionMultiply(newQ,oDir,reverse);
	QuaternionMultiply(nDir,rotate,newQ);
	dir.X() = nDir.X();
	dir.Y() = nDir.Y();
	dir.Z() = nDir.Z();
	dir.Normalize();
	if(dir3*dir<0)dir=-dir;
	
	for (int k=0;k<4;k++)
	{
     Point3f edge = ki[k+1] - ki[k];
	 plane[k]._dir = dir ^edge;
	 plane[k]._dir.Normalize();
	 plane[k]._offset = -(plane[k]._dir.X()*ki[k].X()+plane[k]._dir.Y()*ki[k].Y()+plane[k]._dir.Z()*ki[k].Z());
	}

	int scale = m_SpaceBox.m_DivisionNum;
    Point3f inter;
	inter.X() = 1/m_SpaceBox.m_Delta.X();
	inter.Y() = 1/m_SpaceBox.m_Delta.Y();
	inter.Z() = 1/m_SpaceBox.m_Delta.Z();
	for(int u=0;u<scale;u++)
	  for(int v=0;v<scale;v++)
		for (int w=0;w<scale;w++)
		{
		  if(m_SpaceBox.GetEntrySize(u,v,w)==-1)continue;
		  Point3f minPoint,maxPoint;
		  minPoint.X() = m_SpaceBox.m_MinCorner.X()  + u*inter.X();
		  minPoint.Y() = m_SpaceBox.m_MinCorner.Y()  + v*inter.Y();
		  minPoint.Z() = m_SpaceBox.m_MinCorner.Z()  + w*inter.Z();
		  maxPoint.X() = m_SpaceBox.m_MinCorner.X()  + (u+1)*inter.X();
		  maxPoint.Y() = m_SpaceBox.m_MinCorner.Y()  + (v+1)*inter.Y();
		  maxPoint.Z() = m_SpaceBox.m_MinCorner.Z()  + (w+1)*inter.Z();
		  Point3f corner[8];
		  corner[0].X() = minPoint.X();
		  corner[0].Y() = minPoint.Y();
		  corner[0].Z() = minPoint.Z();
		  corner[1].X() = minPoint.X();
		  corner[1].Y() = minPoint.Y();
		  corner[1].Z() = maxPoint.Z();
		  corner[2].X() = minPoint.X();
		  corner[2].Y() = maxPoint.Y();
		  corner[2].Z() = minPoint.Z();
		  corner[3].X() = minPoint.X();
		  corner[3].Y() = maxPoint.Y();
		  corner[3].Z() = maxPoint.Z();
		  corner[4].X() = maxPoint.X();
		  corner[4].Y() = minPoint.Y();
		  corner[4].Z() = minPoint.Z();
		  corner[5].X() = maxPoint.X();
		  corner[5].Y() = minPoint.Y();
		  corner[5].Z() = maxPoint.Z();
		  corner[6].X() = maxPoint.X();
		  corner[6].Y() = maxPoint.Y();
		  corner[6].Z() = minPoint.Z();
		  corner[7].X() = maxPoint.X();
		  corner[7].Y() = maxPoint.Y();
		  corner[7].Z() = maxPoint.Z();
		  int size;
		  bool inspace[8];
		  size = m_SpaceBox.GetEntrySize(u,v,w);
		  for (int l=0;l<8;l++)
		  {
			inspace[l] = true;
			for(int k=0;k<4;k++)
			{
			  if(plane[k]._dir.X()*corner[l].X()+plane[k]._dir.Y()*corner[l].Y()+plane[k]._dir.Z()*corner[l].Z()+plane[k]._offset<0)
			  {
				inspace[l] = false;
				break;
			  }
				
			}
		  }

		  if(inspace[0]||inspace[1]||inspace[2]||inspace[3]||inspace[4]||inspace[5]||inspace[6]||inspace[7])
		  {
			 for(int j=0;j<size;j++)
			 {
				int item;
				item = m_SpaceBox.GetEntryItem(u,v,w,j);

				Point3f pp = data->V(item)->m_coord;

				bool pointinspace = true;
			

				 Quaternionf  point;

	             point.X() = data->V(item)->m_coord.X();
	             point.Y() = data->V(item)->m_coord.Y();
	             point.Z() = data->V(item)->m_coord.Z();
	             point.W() = 1.0;

	             QuaternionMultiply(newQ,point,rotate);
	             QuaternionMultiply(result,reverse,newQ);

	             Point3f p;
	             p.X() = result.X();
	             p.Y() = result.Y();
	             p.Z() = result.Z();

	             if(CheckInArea(p,rpoints))
				 {
				  if(flag==1)
				  data->V(item)->SetS();
				  else if(flag==2)
					  data->V(item)->SetM();
				  else if(flag==3)
					  data->V(item)->SetM1();
				  	  else if(flag==0)
					  {
					   data->V(item)->ClearS();
					   data->V(item)->ClearM();
					   data->V(item)->ClearM1();
					  }

				 }
			 }
		  }
		}

		m_Polygon.clear();

}
void sn3DMeshPick::RectVertex(sn3DObject * m,Quaternionf rot,int flag)
{
	//sn3DMeshModel * model = (sn3DMeshModel*)m;
	//sn3DMeshData * data = model->GetMeshData();

	//Point3f dir;
	//Point3f dir1,dir2,dir3;
	//std::vector<Plane3f> plane;
	//std::vector<Point3f> points;
	//Quaternionf rotate;
	//Quaternionf newQ,reverse;
	//Quaternionf oDir,nDir;

	//int num = 4;

	//if(m_Rect[0].X()==m_Rect[1].X()&&m_Rect[0].Y()==m_Rect[1].Y())return;
	//points.resize(num);
	//plane.resize(num);

	//
	//GLdouble p[3];
	//gluUnProject(m_Rect[0].X(),m_Rect[0].Y(),0.0,m_MvMatrix,m_ProjMatrix,m_Viewport,&p[0],&p[1],&p[2]);
	//points[0].X() = p[0];
	//points[0].Y() = p[1];
	//points[0].Z() = p[2];
	//gluUnProject(m_Rect[1].X(),m_Rect[0].Y(),0.0,m_MvMatrix,m_ProjMatrix,m_Viewport,&p[0],&p[1],&p[2]);
	//points[1].X() = p[0];
	//points[1].Y() = p[1];
	//points[1].Z() = p[2];
	//gluUnProject(m_Rect[1].X(),m_Rect[1].Y(),0.0,m_MvMatrix,m_ProjMatrix,m_Viewport,&p[0],&p[1],&p[2]);
	//points[2].X() = p[0];
	//points[2].Y() = p[1];
	//points[2].Z() = p[2];
	//gluUnProject(m_Rect[0].X(),m_Rect[1].Y(),0.0,m_MvMatrix,m_ProjMatrix,m_Viewport,&p[0],&p[1],&p[2]);
	//points[3].X() = p[0];
	//points[3].Y() = p[1];
	//points[3].Z() = p[2];
	//

	//rotate.W()= rot.V(0);
	//rotate.X()= rot.V(1);
	//rotate.Y()= rot.V(2);
	//rotate.Z()= rot.V(3);

	//reverse.X() = -rotate.X();
	//reverse.Y() = -rotate.Y();
	//reverse.Z() = -rotate.Z();
	//reverse.W() = rotate.W();

	//dir1 = points[1] - points[0];
	//dir2 = points[2] - points[1];
	//dir3 = dir1 ^ dir2;

	//oDir.X() = 0.0;
	//oDir.Y() = 0.0;
	//oDir.Z()=1.0;
	//oDir.W() = 0.0;
	//QuaternionMultiply(newQ,oDir,reverse);
	//QuaternionMultiply(nDir,rotate,newQ);

	//dir.X() = nDir.X();
	//dir.Y() = nDir.Y();
	//dir.Z() = nDir.Z();
	//dir.Normalize();

	//if(dir3*dir<0)
	//{
 //    dir = -dir;
	//}

	//for (int k=0;k<num-1;k++)
	//{
	//	Point3f edge = points[k+1] - points[k];
	//	plane[k]._dir = dir ^edge;
	//	plane[k]._dir.Normalize();
	//	plane[k]._offset = -(plane[k]._dir.X()*points[k].X()+plane[k]._dir.Y()*points[k].Y()+plane[k]._dir.Z()*points[k].Z());
	//}
	//
	//Point3f edge = points[0] - points[num-1];
	//plane[num-1]._dir = dir ^edge;
	//plane[num-1]._dir.Normalize();
	//plane[num-1]._offset = -(plane[num-1]._dir.X()*points[num-1].X()+plane[num-1]._dir.Y()*points[num-1].Y()+plane[num-1]._dir.Z()*points[num-1].Z());

	//int scale = m_SpaceBox.m_DivisionNum;

	//for (int u=0;u<scale;u++)
	//	for(int v=0;v<scale;v++)
	//		for (int w=0;w<scale;w++)
	//		{
	//			if (m_SpaceBox.GetEntrySize(u,v,w)==-1)continue;

	//			Point3f minPoint,maxPoint;

	//			Point3f inter;
	//			inter.X() = 1/m_SpaceBox.m_Delta.X();
	//			inter.Y() = 1/m_SpaceBox.m_Delta.Y();
	//			inter.Z() = 1/m_SpaceBox.m_Delta.Z();

	//			minPoint.X() = m_SpaceBox.m_MinCorner.X()  + u*inter.X();
	//			minPoint.Y() = m_SpaceBox.m_MinCorner.Y()  + v*inter.Y();
	//			minPoint.Z() = m_SpaceBox.m_MinCorner.Z()  + w*inter.Z();

	//			maxPoint.X() = m_SpaceBox.m_MinCorner.X()  + (u+1)*inter.X();
	//			maxPoint.Y() = m_SpaceBox.m_MinCorner.Y()  + (v+1)*inter.Y();
	//			maxPoint.Z() = m_SpaceBox.m_MinCorner.Z()  + (w+1)*inter.Z();

	//			Point3f corner[8];

	//			corner[0].X() = minPoint.X();
	//			corner[0].Y() = minPoint.Y();
	//			corner[0].Z() = minPoint.Z();
	//			corner[1].X() = minPoint.X();
	//			corner[1].Y() = minPoint.Y();
	//			corner[1].Z() = maxPoint.Z();
	//			corner[2].X() = minPoint.X();
	//			corner[2].Y() = maxPoint.Y();
	//			corner[2].Z() = minPoint.Z();
	//			corner[3].X() = minPoint.X();
	//			corner[3].Y() = maxPoint.Y();
	//			corner[3].Z() = maxPoint.Z();

	//			corner[4].X() = maxPoint.X();
	//			corner[4].Y() = minPoint.Y();
	//			corner[4].Z() = minPoint.Z();
	//			corner[5].X() = maxPoint.X();
	//			corner[5].Y() = minPoint.Y();
	//			corner[5].Z() = maxPoint.Z();
	//			corner[6].X() = maxPoint.X();
	//			corner[6].Y() = maxPoint.Y();
	//			corner[6].Z() = minPoint.Z();
	//			corner[7].X() = maxPoint.X();
	//			corner[7].Y() = maxPoint.Y();
	//			corner[7].Z() = maxPoint.Z();

	//			int size;

	//			bool inspace[8];

	//			size = m_SpaceBox.GetEntrySize(u,v,w);

	//			for (int l=0;l<8;l++)
	//			{
	//				inspace[l] = true;
	//				for(int k=0;k<num;k++)
	//				{

	//					if(plane[k]._dir.X()*corner[l].X()+plane[k]._dir.Y()*corner[l].Y()+plane[k]._dir.Z()*corner[l].Z()+plane[k]._offset<0)
	//					{
	//						inspace[l] =false;
	//						break;
	//					}
	//				}
	//			}

	//			if(inspace[0]&&inspace[1]&&inspace[2]&&inspace[3]&&inspace[4]&&inspace[5]&&inspace[6]&&inspace[7])
	//			{
	//				for(int j=0;j<size;j++)
	//				{
	//					int item;
	//					item = m_SpaceBox.GetEntryItem(u,v,w,j);
	//				    data->V(item)->SetS();
	//				}
	//			}
	//			else if(inspace[0]||inspace[1]||inspace[2]||inspace[3]||inspace[4]||inspace[5]||inspace[6]||inspace[7])
	//			{
	//				for(int j=0;j<size;j++)
	//				{
	//					int item;
	//					item = m_SpaceBox.GetEntryItem(u,v,w,j);

	//					Point3f pp = data->V(item)->m_coord;

	//					bool pointinspace = true;
	//					for(int k=0;k<num;k++)
	//					{

	//						if(plane[k]._dir.X()*pp.X()+plane[k]._dir.Y()*pp.Y()+plane[k]._dir.Z()*pp.Z()+plane[k]._offset<0)
	//						{
	//							pointinspace =false;
	//							break;
	//						}
	//					}

	//							data->V(item)->SetS();
	//				}
	//			}

	//		}

if(m_Rect[0].X()==m_Rect[1].X()&&m_Rect[0].Y()==m_Rect[1].Y())return;


 m_Polygon.clear();

 m_Polygon.push_back(Point2i(m_Rect[0].X(),m_Rect[0].Y()));
 m_Polygon.push_back(Point2i(m_Rect[1].X(),m_Rect[0].Y()));
 m_Polygon.push_back(Point2i(m_Rect[1].X(),m_Rect[1].Y()));
 m_Polygon.push_back(Point2i(m_Rect[0].X(),m_Rect[1].Y()));
 
 SelectPolygonVertex(m,rot,flag);
}




bool sn3DMeshPick::CheckInArea(Point3f point,std::vector<Point3f> &  line)
{
  int end = (int)line.size();

  int countx=0;
  int county=0;

  for(int i=0;i<end-1;i++)
  {
   if(line[i].Y()>point.Y()||line[i+1].Y()>point.Y())
   {
   if(line[i].X()<point.X()&&line[i+1].X()>point.X())countx++;
   if(line[i].X()>point.X()&&line[i+1].X()<point.X())countx++;
   }

   if(line[i].X()>point.X()||line[i+1].X()>point.X())
   {
   if(line[i].Y()<point.Y()&&line[i+1].Y()>point.Y())county++;
   if(line[i].Y()>point.Y()&&line[i+1].Y()<point.Y())county++;
   }

   }

  if(countx%2!=0&&county%2!=0) return true;

  else return  false;
 
}
void sn3DMeshPick::Clear()
{
}
}