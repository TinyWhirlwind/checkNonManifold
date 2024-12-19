#include "sn3DPointSampling.h"
#include "RangeCurvature.h"
#include "../sn3DDataModel/sn3DTopology.h"
#pragma warning (disable : 4244)
#pragma warning (disable : 4018)

namespace sn3DCore
{
void sn3DPointSampling::Smapling(sn3DMeshData * data,int sampleRatio,int h,int w,int * pntIds)
{
	if( sampleRatio >= 100) return;

	float perc = (float)sampleRatio/100.0;

	RangeCurvature rangeCurvature;

	//sn3DMeshData * data = model->GetMeshData();

	Topology::VertexFace(*data); 

	for(int i=0;i<data->vn;i++)
	 if(data->V(i)->VFp()==NULL)data->V(i)->SetD();

	data->UpdateNormal();

	rangeCurvature.ComputerCurvatrue(data);

	for(int i=0;i<(int)data->vn;i++) // 特殊情况处理
	{  
	  if(data->V(i)->N()==Point3f(0.0,0.0,0.0))
	  { 
		rangeCurvature.m_Frame[i].k2 = 0.0;
		rangeCurvature.m_Frame[i].k1 = 0.0;
		rangeCurvature.VertexArea[i] = 0.0;
	  }
	}
	///////////////////////////  采样  //////////////////////////

	double sumWeight =0.0;
	std::vector<double> values;
	values.resize(data->vn);

	for(int i=0;i<(int)data->vn;i++)
    data->V(i)->SetD();

	for(int i=0;i<(int)data->vn;i++)
	{  
		float value1 = (float)(rangeCurvature.m_Frame[i].k1+rangeCurvature.m_Frame[i].k2)/2.0;
		if(value1<0)
			value1 = -value1;

		float value2 = rangeCurvature.VertexArea[i];
		if(value2<0)
			value2 = -value2;

		values[i] = sqrt(value1)*value2;
		sumWeight+= values[i];

	}
	
	float pointnum = perc*data->vn;
	double interv = sumWeight/pointnum;

	double cursum = 0;
	
	for(int i=0;i<h;i++)
		for(int j=0;j<w;j++)
		{
		  int index =  pntIds[i*w+j];
		  if(index<0)continue;
		  cursum += values[index];
		  if(cursum>interv)
		  {
			data->V(index)->ClearD();//采样。。
			cursum -= interv;
		  }
		}

		////// release resource....
		std::vector<double>().swap(values);
		data->ClearFaceTopology();
		data->ClearVertexTopology();
		data->Compact();
}
}