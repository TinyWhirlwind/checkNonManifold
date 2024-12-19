//#include "stdafx.h"

#include "TriMeshOperation.h"
#include "../Common/Random.h"
#include "../Common/Median.h"



#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif
#pragma warning (disable : 4244)
float TriMeshOperation::MedianEdgeLength(sn3DMeshData * mesh,int percentile)
{
	Median<float> med(percentile, mesh->fn);

	float dis2;

	Point3f diff;

	for (unsigned int i=0; i<mesh->fn; i++) 
	{
     diff = mesh->F(i)->P(0) - mesh->F(i)->P(1);
     dis2 = diff.X()*diff.X()+diff.Y()*diff.Y()+diff.Z()*diff.Z();
	 med += dis2;
	 diff = mesh->F(i)->P(2) - mesh->F(i)->P(1);
	 dis2 = diff.X()*diff.X()+diff.Y()*diff.Y()+diff.Z()*diff.Z();
	 med += dis2;
	 diff = mesh->F(i)->P(0) - mesh->F(i)->P(2);
	 dis2 = diff.X()*diff.X()+diff.Y()*diff.Y()+diff.Z()*diff.Z();
	 med += dis2;
	} 

	return sqrtf(med.find());
}
void TriMeshOperation::RemoveLongEdge(sn3DMeshData * m,int  factor,int  percentile)
{
	// calculate the threshold
	float thr = MedianEdgeLength(m, percentile);

	thr *= thr* float(factor);// * factor);

	float dis2;
	Point3f diff;


	// could make faster by not recalculating edges!

	int fCount = m->fn;

	for(unsigned int i=0; i<m->fn; i++) 
	{
    
	  diff = m->F(i)->P(0) - m->F(i)->P(1);
	  dis2 = diff.X()*diff.X()+diff.Y()*diff.Y()+diff.Z()*diff.Z();
	  if (dis2>thr)
	  {
        m->F(i)->SetD();
	  }
	  else 
	  {
		diff = m->F(i)->P(2) - m->F(i)->P(1);
		dis2 = diff.X()*diff.X()+diff.Y()*diff.Y()+diff.Z()*diff.Z();
       if (dis2>thr)
       {
		 m->F(i)->SetD();
       }
	   else
	   {
		 diff = m->F(i)->P(0) - m->F(i)->P(2);
		 dis2 = diff.X()*diff.X()+diff.Y()*diff.Y()+diff.Z()*diff.Z();
		 if (dis2>thr)
		 {
		  m->F(i)->SetD();
		 }

	   }

	  }
   }

	int cnt = 0;
	for (unsigned int i=0;i<m->fn;i++)
	{
	  if(!m->F(i)->IsD())
	  {
	   // m->F(cnt)->V(0) = m->F(i)->V(0);
		//m->F(cnt)->V(1) = m->F(i)->V(1);
		//m->F(cnt)->V(2) = m->F(i)->V(2);
		cnt++;
	  }
	}

	std::vector<Face>::iterator iter;
	iter  =m->m_faces.begin();
	for(int o=0;o<cnt;o++)iter++;

	m->m_faces.erase(iter,m->m_faces.end());
	m->fn = (int)m->m_faces.size();
	for(unsigned int i=0;i<m->fn;i++)
	m->F(i)->Index() = i;
}

void TriMeshOperation::ComputeVertexBoundary(sn3DMeshData * mesh)
{

}
void TriMeshOperation::RandomSamplePoint(sn3DMeshData * mesh,float rate,std::vector<Point3f> &points, vector<Point3f> &normals)
{
   int n = rate*mesh->vn;

    Random rnd;

	points.clear(); points.reserve(n);

	normals.clear(); normals.reserve(n);

	double nv = mesh->vn;

	if (n > nv) return ;

	double np = n;

	int end = nv;

	for (int i = 0; i<end; i++) 
	{
		if (rnd(nv) <= np) 
		{
			points.push_back(mesh->V(i)->m_coord);
			normals.push_back(mesh->V(i)->N());
			np--;
		}
		nv--;
	}
	assert(np == 0);

}
void TriMeshOperation::RemoveUnreferenceVertex(sn3DMeshData * mesh)
{

   for(unsigned int i=0;i<mesh->vn;i++)mesh->V(i)->Index() = i;
	
   std::vector<int> indices(mesh->vn,-1);

	for(unsigned int i=0;i<mesh->fn;i++)
	{
	  indices[mesh->F(i)->V(0)->Index()] = mesh->F(i)->V(0)->Index();
	  indices[mesh->F(i)->V(1)->Index()] = mesh->F(i)->V(1)->Index();
	  indices[mesh->F(i)->V(2)->Index()] = mesh->F(i)->V(2)->Index();
	}

	int cnt = 0;
	for (unsigned int i=0; i<mesh->vn; i++) 
	{
		if (indices[i] != -1) 
		{
			indices[i] = cnt;
			mesh->V(cnt)->m_coord = mesh->V(i)->m_coord;
			cnt++;
		}
	}

	for (unsigned int i=0; i<mesh->fn; i+=3) 
	{
	  //mesh->F(i)->V(0) = mesh->V(indices[mesh->F(i)->V(0)->m_Index]);
	  //mesh->F(i)->V(1) = mesh->V(indices[mesh->F(i)->V(1)->m_Index]);
	  //mesh->F(i)->V(2) = mesh->V(indices[mesh->F(i)->V(2)->m_Index]);	
	}

	std::vector<Vertex>::iterator iter;

	iter = mesh->m_vertices.begin();
	for(int o=0;o<cnt;o++)iter++;
	mesh->m_vertices.erase(iter, mesh->m_vertices.end());

	mesh->vn = (int)mesh->m_vertices.size();
	for(unsigned int i=0;i<mesh->vn;i++)mesh->V(i)->Index() = i;
}
void TriMeshOperation::RemoveDegenerateFace(sn3DMeshData * mesh)
{
	int end = mesh->m_faces.size();
	for (unsigned int i=0;i<mesh->fn;i++)
	{

		if (mesh->F(i)->V(0)==mesh->F(i)->V(1)||mesh->F(i)->V(0)==mesh->F(i)->V(2)||mesh->F(i)->V(2)==mesh->F(i)->V(1))
		{
			mesh->DeleteFace(i);
		}
	}
}