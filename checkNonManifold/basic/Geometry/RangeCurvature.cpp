#include "RangeCurvature.h"

#pragma warning (disable : 4244)
namespace sn3DCore
{
RangeCurvature::RangeCurvature()
{
  m_Frame=NULL;
  m_FaceCornerArea=NULL;
  VertexArea=NULL;
}
RangeCurvature::~RangeCurvature()
{
  Clear();
}
void RangeCurvature::ComputerCurvatrue(sn3DMeshData * model)
{
  if(m_Frame != NULL) delete m_Frame;
  m_Frame   = new LocalFrame[model->vn];
  if(m_FaceCornerArea!=NULL) delete m_FaceCornerArea;
  m_FaceCornerArea = new Point3f[model->fn];
  if(VertexArea!=NULL) delete VertexArea;
  VertexArea     = new float[model->vn];
  memset(VertexArea,0,model->vn*sizeof(float));
  ComputeVertexArea(model);
  VertexCurvature(model);
}
void RangeCurvature::ComputeVertexArea(sn3DMeshData * model)
{
	Point3f edge[3], faceNorm;
	float   lengthSq[3], edgeWeight[3], faceArea;
	
	int end = (int)model->fn;

	for (int faceIndex=0;faceIndex < end;faceIndex++)
	{
	  edge[0]=model->F(faceIndex)->P(2) - model->F(faceIndex)->P(1);
	  edge[1]=model->F(faceIndex)->P(0) - model->F(faceIndex)->P(2);
      edge[2]=model->F(faceIndex)->P(1) - model->F(faceIndex)->P(0);
	
	  lengthSq[0] = edge[0].X()*edge[0].X()+edge[0].Y()*edge[0].Y()+edge[0].Z()*edge[0].Z();
	  lengthSq[1] = edge[1].X()*edge[1].X()+edge[1].Y()*edge[1].Y()+edge[1].Z()*edge[1].Z();
	  lengthSq[2] = edge[2].X()*edge[2].X()+edge[2].Y()*edge[2].Y()+edge[2].Z()*edge[2].Z();

	  faceNorm = edge[0] ^ edge[1];
	  faceArea = 0.5f * faceNorm.Norm();

	  edgeWeight[0] = lengthSq[0] * (lengthSq[1] + lengthSq[2] - lengthSq[0]);
	  edgeWeight[1] = lengthSq[1] * (lengthSq[2] + lengthSq[0] - lengthSq[1]);
	  edgeWeight[2] = lengthSq[2] * (lengthSq[0] + lengthSq[1] - lengthSq[2]);
		
	  if(edgeWeight[0] <= 0.0f)
	  {
		m_FaceCornerArea[faceIndex][1] = - 0.25f * lengthSq[2] * faceArea / (edge[0] * edge[2]);
		m_FaceCornerArea[faceIndex][2] = - 0.25f * lengthSq[1] * faceArea / (edge[0] * edge[1]);
		m_FaceCornerArea[faceIndex][0] = faceArea - m_FaceCornerArea[faceIndex][1] - m_FaceCornerArea[faceIndex][2];
	  }
	  else if(edgeWeight[1] <= 0.0f)
	  {
		m_FaceCornerArea[faceIndex][2] = - 0.25f * lengthSq[0] * faceArea / (edge[1] * edge[0]);
		m_FaceCornerArea[faceIndex][0] = - 0.25f * lengthSq[2] * faceArea / (edge[1] * edge[2]);
		m_FaceCornerArea[faceIndex][1] = faceArea - m_FaceCornerArea[faceIndex][2] - m_FaceCornerArea[faceIndex][0];
	  }
	  else if(edgeWeight[2] <= 0.0f)
	  {
		m_FaceCornerArea[faceIndex][0] = - 0.25f * lengthSq[1] * faceArea / (edge[2] * edge[1]);
		m_FaceCornerArea[faceIndex][1] = - 0.25f * lengthSq[0] * faceArea / (edge[2] * edge[0]);
		m_FaceCornerArea[faceIndex][2] = faceArea - m_FaceCornerArea[faceIndex][0] - m_FaceCornerArea[faceIndex][1];
	  }
	  else
	  {
		float fScale = 0.5f * faceArea / (edgeWeight[0] + edgeWeight[1] + edgeWeight[2]);
		for (int j = 0; j < 3; j ++)
			m_FaceCornerArea[faceIndex][j] = fScale * (edgeWeight[(j+1)%3] + edgeWeight[(j+2)%3]);
	  }
		
	  VertexArea[model->F(faceIndex)->V(0)->Index()] += m_FaceCornerArea[faceIndex][0];
	  VertexArea[model->F(faceIndex)->V(1)->Index()] += m_FaceCornerArea[faceIndex][1];
	  VertexArea[model->F(faceIndex)->V(2)->Index()] += m_FaceCornerArea[faceIndex][2];	
	}
}
void RangeCurvature::VertexCurvature(sn3DMeshData * model)
{
	
	int end = (int)model->vn;

	for (int i=0; i<end; i++)
	{
	   unsigned int index = model->V(i)->Index();
	   if(!model->V(i)->IsD())
	   {
		 m_Frame[index].uDir=model->V(i)->VFp()->V((model->V(i)->VFi()+1)%3)->P()-model->V(i)->P();			
		 m_Frame[index].normal=model->V(i)->N();
		 m_Frame[index].uDir=m_Frame[index].uDir^m_Frame[index].normal;
		 m_Frame[index].uDir /= m_Frame[index].uDir.Norm();
		 m_Frame[index].vDir = m_Frame[index].normal ^ m_Frame[index].uDir;
	   }
	}

	LocalFrame faceFrame,vertexFrame;
	Point3f edge[3];
	end = (int)model->fn;
	for (int faceIndex =0; faceIndex<end;faceIndex++)
	{
	  if(!model->F(faceIndex)->IsD())
	  {
		edge[0]=model->F(faceIndex)->P(2) - model->F(faceIndex)->P(1);
		edge[1]=model->F(faceIndex)->P(0) - model->F(faceIndex)->P(2);
		edge[2]=model->F(faceIndex)->P(1) - model->F(faceIndex)->P(0);
		
		faceFrame.uDir  = edge[0]/edge[0].Norm();
		faceFrame.normal=edge[0]^edge[1];
		faceFrame.normal /= faceFrame.normal.Norm();
		faceFrame.vDir  =faceFrame.normal^faceFrame.uDir;	
		faceFrame.vDir /=faceFrame.vDir.Norm();

		Point3f m;
		m.X()=m.Y()=m.Z()=0;
		double w[3][3];
		for(int temp_i=0;temp_i<3;temp_i++)
			for(int temp_j=0;temp_j<3;temp_j++)
				w[temp_i][temp_j]=0;
		Point3f normalDiff;
		for (int j = 0; j < 3; j ++)
		{
			float u =edge[j]*faceFrame.uDir; 
			float v =edge[j]*faceFrame.vDir; 
			w[0][0] += u * u;
			w[0][1] += u * v;
			w[2][2] += v * v;
			
			normalDiff=model->F(faceIndex)->V((j+2)%3)->N() - model->F(faceIndex)->V((j+1)%3)->N();
			float dnu =normalDiff*faceFrame.uDir;
			float dnv =normalDiff*faceFrame.vDir; 
			m[0] += dnu * u;
			m[1] += dnu * v + dnv * u;
			m[2] += dnv * v;
		}
		w[1][1] = w[0][0] + w[2][2];
		w[1][2] = w[0][1];
		Point3f diag;
		choldc(w, diag);
		cholsl( w, diag, m, m);

		faceFrame.k1  =m[0];
		faceFrame.k12 =m[1];
		faceFrame.k2  =m[2];

		for(int j = 0; j < 3; j ++)
		{
			unsigned int index = model->F(faceIndex)->V(j)->Index();

			vertexFrame.normal=m_Frame[index].normal;
			vertexFrame.uDir  =m_Frame[index].uDir;
			vertexFrame.vDir  =m_Frame[index].vDir;

			ProjectCurvature(faceFrame, vertexFrame);
		
			float wt = m_FaceCornerArea[faceIndex][j] / VertexArea[index];
			m_Frame[index].k1  += wt*vertexFrame.k1;
			m_Frame[index].k12 += wt*vertexFrame.k12;
			m_Frame[index].k2  += wt*vertexFrame.k2;
		}
	}
	}	
  
	end = (int)model->vn;
	for (int i=0; i<end; i++)
	{
	  if(!model->V(i)->IsD())
	  {
	   DiagonalizeCurvature(m_Frame[model->V(i)->Index()]);
	  }
	}
	
}

void RangeCurvature::ProjectCurvature(LocalFrame & faceFrame,LocalFrame & vertexFrame)							
{
	
	Point3f r_uDir,r_vDir;//vertex标架旋转后的u,v单位向量
	
	/* 将两局部标架旋转到同一平面 */
	r_uDir = vertexFrame.uDir;
	r_vDir = vertexFrame.vDir;
	
	float ndot = vertexFrame.normal * faceFrame.normal;
	if (ndot <= -1.0f)
	{
		r_uDir = -r_uDir;
		r_vDir = -r_vDir;
	}
	else{
	Point3f perp_old = faceFrame.normal - vertexFrame.normal * ndot;
	Point3f dperp =  (vertexFrame.normal + faceFrame.normal) * (1.0f / (1 + ndot));
	r_uDir =r_uDir - dperp *(r_uDir * perp_old); 
	r_vDir =r_vDir - dperp *(r_vDir * perp_old); 
	}
	
    /* 在同一平面内变换曲率张量  */
	float uu =r_uDir * faceFrame.uDir;
	float uv =r_uDir * faceFrame.vDir; 
	float vu =r_vDir * faceFrame.uDir;
	float vv =r_vDir * faceFrame.vDir;
	
	vertexFrame.k1  = faceFrame.k1 * uu * uu + faceFrame.k12 * (2.0f  * uu * uv) + faceFrame.k2* uv * uv;
	vertexFrame.k12 = faceFrame.k1 * uu * vu + faceFrame.k12 * (uu * vv + vu * uv) + faceFrame.k2 * uv * vv;
	vertexFrame.k2  = faceFrame.k1 * vu * vu + faceFrame.k12 * (2.0f  * vu * vv ) + faceFrame.k2 * vv * vv;
    
}
void RangeCurvature::DiagonalizeCurvature(LocalFrame & frame)
{
	Point3f pdir1,pdir2;
	float k1,k2;
	float c = 1, s = 0, tt = 0;
	if (frame.k12 != 0.0f)
	{
		const float h = 0.5f * (frame.k2 - frame.k1) / frame.k12;
		tt = (h < 0.0f) ? 1.0f / (h - sqrt(1.0f + h * h)) : 1.0f / (h + sqrt(1.0f + h * h));
		c = 1.0f / sqrt(1.0f + tt * tt);
		s = tt * c;
	}
	k1 = frame.k1 - tt * frame.k12;
	k2 = frame.k2 + tt * frame.k12;
	if (k1 >= k2)
	{
		pdir1 = frame.uDir * c  - frame.vDir * s ;
	}
	else
	{
		float temp = k1;
		k1=k2;k2=temp;

		pdir1 = frame.uDir * s  + frame.vDir * c ;
	}
	pdir2=frame.normal^pdir1;
	frame.k1=k1;
	frame.k2=k2;
	frame.k12=0;
	frame.uDir=pdir1;
	frame.vDir=pdir2;
}
void RangeCurvature::choldc(double a[][3], Point3f & p)
{
	int i,j,k;
	float sum;
	for (i=0;i<3;i++) {
		for (j=i;j<3;j++) {
			for (sum=a[i][j],k=i-1;k>=0;k--) sum -= a[i][k]*a[j][k];
			if (i == j) p[i]=sqrt(sum);
			else a[j][i]=sum/p[i];
		}
	}
}

void RangeCurvature::cholsl(double a[][3], Point3f &p, Point3f &b, Point3f &x)
{
	int i,k;
	float sum;	
	for (i=0;i<3;i++) {
		for (sum=b[i],k=i-1;k>=0;k--) sum -= a[i][k]*x[k];
		x[i]=sum/p[i];
	}
	for (i=2;i>=0;i--) {
		for (sum=x[i],k=i+1;k<3;k++) sum -= a[k][i]*x[k];
		x[i]=sum/p[i];
	}
}
void RangeCurvature::Clear()
{
  if(m_Frame!=NULL)
   delete m_Frame;
  if(m_FaceCornerArea!=NULL)
  delete m_FaceCornerArea;
  if(VertexArea!=NULL)
   delete VertexArea;
}
}
  