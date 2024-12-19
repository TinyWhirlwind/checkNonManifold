#ifndef _SN3DPOINTSAMPLING_
#define _SN3DPOINTSAMPLING_

/*  sn3DPointSampling
   点云抽样类，接受采样比例，输出采样后的点云

*/
#include"../sn3DDataModel/sn3DMeshModel.h"
namespace sn3DCore
{
class sn3DPointSampling
{
public:
 void Smapling(sn3DMeshData * data,int sampleRatio,int h,int w,int * pntIds);

};
}

#endif