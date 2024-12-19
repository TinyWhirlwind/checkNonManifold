#include"sn3DSelect.h"
#include <windows.h>
#include<GL/gl.h>
#include<GL/GLu.h>

namespace sn3DCore
{
sn3DSelect::sn3DSelect()
{
 m_LastState = 0;
 m_SelectMode = 0;
 isPicking = false;
 m_pick = NULL;
 m_object = NULL;
 m_feature = -1;
}
sn3DSelect::~sn3DSelect()
{
	if(m_pick!=NULL)
	{
		m_pick->Clear();
		delete m_pick;
	}
}
void sn3DSelect::SetMesh(sn3DObject * obj)
{	
	m_object = obj;
	if(obj->IsOfType(sn3DObject::TYPE_MESH))
	{
		if(m_pick!=NULL)
		{
			m_pick->Clear();
			delete m_pick;
		}
		m_pick = new sn3DMeshPick((sn3DMeshModel*)obj);
	}
	else if(obj->IsOfType(sn3DObject::TYPE_RANGE))
	{
		sn3DRangeModel* data = (sn3DRangeModel*)obj;

		if(m_pick!=NULL)
		{
			m_pick->Clear();
			delete m_pick;
		}
		m_pick = new sn3DRangePick((sn3DRangeModel*)obj);

	}
	else if(obj->GetType()==sn3DObject::TYPE_POINTS )
	{


	}
}
void  sn3DSelect::SetPick(sn3DPick * pick)
{
 m_pick = pick;
}
void sn3DSelect::Draw(sn3DSceneGL3D *,int feature)
{
   if(m_pick==NULL)return;
   switch (m_SelectMode)
   {
	case PAINT_SELECT:
		
	 m_pick->UpdateMatrix();
	 m_object->RenderSelected();
	
	 if(m_pick->m_IsDragging)
	 {
		m_pick->DrawXORPaintCircle();
	
	 }
			
	 break;
		
	case RECT_SELECT:
		
	m_pick->UpdateMatrix();
	if(m_pick->m_IsDragging)
    {	  
	  m_pick->DrawRect();	  
	}	  
	if(m_feature==1)
	m_object->RenderSelected();
	else
	{
	 if(feature==1)
	 	m_object->RenderSelected();
	 else
		m_object->RenderAlign();
	}
	
	break;
		
	case POLYGON_SELECT:

    m_pick->UpdateMatrix();	
	m_pick->DrawPolygon();

	if(m_feature==1)
	m_object->RenderSelected();
	else
	{
	 if(feature==1)
	 	m_object->RenderSelected();
	 else
		m_object->RenderAlign();
	}
	 break;

	case REFPOINT_SELECT:

	m_pick->UpdateMatrix();	
	m_pick->DrawPolygon();

	/*if(m_feature==1)
		m_object->RenderSelected();
	else
	{
		if(feature==1)
			m_object->RenderSelected();
		else
			m_object->RenderAlign();
	}*/
	break;
		
   }
	
}
bool sn3DSelect::MouseMove(sn3DSceneGL3D * scene,unsigned int nFlags,int pX,int pY)
{
	if(m_pick==NULL)return false;
	//CPoint pos; 
	Point2i pos;

	pos.X() = pX;
	pos.Y() = pY;

	Point2f point = Point2f(pX, pY);
	Point3f mainAxis,secoAxis,thirAxis,center;

	switch(m_SelectMode)
	{
	 case PAINT_SELECT:
		
	 if(m_pick->m_IsDragging)
	 if((nFlags&MK_SHIFT)||isPicking)
	 { 
	   m_pick->m_CurPoint = point;
	   Quaternionf rot;
	   scene->GetRotate(rot);
       m_pick->PaintVertex(m_object,rot,1);
	  }
        
	 break;

		
	 case RECT_SELECT:

	 if(m_pick->m_IsDragging)
	 if((nFlags&MK_SHIFT)||isPicking)
	 {
	   m_pick->m_Rect[1].X() = (int)point.x;
	   m_pick->m_Rect[1].Y() = (int)point.y;
	 }
	
	 break;
        
	 case POLYGON_SELECT:
		
	 if(m_pick->m_IsDragging)
	 if((nFlags&MK_SHIFT)||isPicking)
	 {
	   pos.X() = (int)point.x;
	   pos.Y() = (int)point.y;
	   m_pick->m_Polygon.push_back(pos);	
	 }

	  break;

	 case REFPOINT_SELECT:

	 if(m_pick->m_IsDragging)
		 if((nFlags&MK_SHIFT)||isPicking)
		 {
			 pos.X() = (int)point.x;
			 pos.Y() = (int)point.y;
			 m_pick->m_Polygon.push_back(pos);	
		 }

	 break;

	 }


	return true;
}
bool sn3DSelect::LeftClick(sn3DSceneGL3D * scene,unsigned int nFlags,int pX,int pY)
{
	
	if(m_pick==NULL)return false;

	Point2f point = Point2f(pX, pY);

	Point2i pos;
	//CPoint pos; 

	pos.X() = pX;
	pos.Y() = pY;

		switch(m_SelectMode)
		{

		case POLYGON_SELECT:

			if(!m_pick->m_IsDragging)
			if(nFlags&MK_SHIFT)
			{
				m_pick->m_IsDragging = true;
				pos.X() = (int)point.x;
				pos.Y() = (int)point.y;
				m_pick->m_Polygon.clear();
				m_pick->m_Polygon.push_back(pos);
			}
		
			break;

		case RECT_SELECT:
			if(!m_pick->m_IsDragging)
			if(nFlags&MK_SHIFT)
			{

              m_pick->m_IsDragging  =true;
			  m_pick->m_CurPoint = point;
			  pos.X() = (int)point.x;
			  pos.Y() = (int)point.y;
			  m_pick->m_Rect[0] = m_pick->m_Rect[1] = pos;
			}
			break;

		case PAINT_SELECT:
			if(!m_pick->m_IsDragging)
				if(nFlags&MK_SHIFT)
			{
             GLdouble w1[3];
	         GLdouble w2[3];
	         GLdouble p1[3];
	         GLdouble p2[3];

	         w1[0] = 0; w2[0] = m_pick->m_SelectRadius;
	         w1[1] = 0; w2[1] = 0;
	         w1[2] = 0; w2[2] = 0;

	         gluUnProject(w1[0],w1[1],w1[2],m_pick->m_MvMatrix,m_pick->m_ProjMatrix,m_pick->m_Viewport,&p1[0],&p1[1],&p1[2]);
	         gluUnProject(w2[0],w2[1],w2[2],m_pick->m_MvMatrix,m_pick->m_ProjMatrix,m_pick->m_Viewport,&p2[0],&p2[1],&p2[2]);

	         Point3d diff;
	         diff.X() = p2[0] -p1[0];
	         diff.Y() = p2[1] -p1[1];
	         diff.Z() = p2[2] -p1[2];

	         m_pick->m_PaintRadius = diff.Norm();
			 m_pick->m_PaintRadius2 = m_pick->m_PaintRadius*m_pick->m_PaintRadius;


			 m_pick->m_IsDragging  =true;
			 m_pick->m_CurPoint = point;

			 Quaternionf rot;
			 scene->GetRotate(rot);
			 m_pick->PaintVertex(m_object,rot,m_feature);
			}
			break;

		case  REFPOINT_SELECT:

			if(!m_pick->m_IsDragging)
				if(nFlags&MK_SHIFT)
				{
					m_pick->m_IsDragging = true;
					pos.X() = (int)point.x;
					pos.Y() = (int)point.y;
					m_pick->m_Polygon.clear();
					m_pick->m_Polygon.push_back(pos);
				}

			break;


		}

		isPicking = true;

	
	return true;
}

bool sn3DSelect::LeftUp(sn3DSceneGL3D * scene,int pX,int pY)
{
    if(m_pick==NULL)return false;
	Point2i pos;
	pos.X() = pX;
	pos.Y() = pY;

	Point2f dir;
	Point2f pp;
	//int n;

	//int end;
	Point2f point = Point2f(pX, pY);
	std::vector<Point2i> points;
	
	switch(m_SelectMode)
	{
	 case PAINT_SELECT:
			
	 if(m_pick->m_IsDragging)
	 {
	   m_pick->m_IsDragging = false;//ÍË³öÍÏ¶¯×´Ì¬
			
	 }
			
	 break;
		
	 case POLYGON_SELECT:

	 if(m_pick->m_IsDragging)
	 {			
	  pos.X() = (int)point.x;
	  pos.Y() = (int)point.y;
	  m_pick->m_IsDragging = false;
	 
	  m_pick->m_Polygon.push_back(pos);
	  m_pick->m_Polygon.push_back(m_pick->m_Polygon.front());
	
	

	  if (m_pick->m_Polygon.size()>=3)
	  {
	    Quaternionf rot;
		scene->GetRotate(rot);
		m_pick->SelectPolygonVertex(m_object,rot,m_feature);
	  }		
	 }

	 break;
		
	 case RECT_SELECT:
	 if(m_pick->m_IsDragging)
	 {
	    m_pick->m_IsDragging = false;//ÍË³öÍÏ¶¯×´Ì¬
		Quaternionf rot;
		scene->GetRotate(rot);
	    m_pick->RectVertex(m_object,rot,m_feature);
	 }
	
     break;

	 case REFPOINT_SELECT:

	 if(m_pick->m_IsDragging)
	 {			
		 pos.X() = (int)point.x;
		 pos.Y() = (int)point.y;
		 m_pick->m_IsDragging = false;

		 m_pick->m_Polygon.push_back(pos);
		 m_pick->m_Polygon.push_back(m_pick->m_Polygon.front());



		 if (m_pick->m_Polygon.size()>=3)
		 {
			 Quaternionf rot;
			 scene->GetRotate(rot);
			 int objnum = scene->GetObjectCount();
			 for (int i=0; i<objnum; i++)
			 {
				 m_pick->SelectRefPolygonVertex(scene->GetObject(i),rot,m_feature);
			 }
			 m_pick->m_Polygon.clear();
		 }		
	 }

	 break;

	}

	isPicking = false;

	return true;
}
bool sn3DSelect::IsDragging()
{
  if(m_pick==NULL)return false;
  return m_pick->m_IsDragging;
}
void sn3DSelect::SetMode( int mode)
{
  m_SelectMode = mode;
  if (mode == REFPOINT_SELECT)
  {
	  if(m_pick!=NULL)
	  {
		  m_pick->Clear();
		  delete m_pick;
	  }
	  m_pick = new sn3DRangePick();
  }
}
void sn3DSelect::SetSelectRadius(int selectRadius)
{
  if(m_pick!=NULL)
	m_pick->m_SelectRadius = selectRadius; 
}
void sn3DSelect::Clear()
{
	if( m_pick!=NULL )
	{
		m_pick->Clear();
		delete m_pick;
		m_pick = NULL;
	}


}
}