#ifndef _SN3DRANGEPICK_
#define _SN3DRANGEPICK_


/*  sn3DRangePick 类

   针对深度图数据的场景拾取类  */


#include"sn3DPick.h"
#include"../../Geometry/SpaceBox.h"
#include"../../SceneGraph/sn3DRangeModel.h"

namespace sn3DCore
{

class sn3DRangePick : public sn3DPick
{
public:

	void PaintVertex(sn3DObject * mesh,Quaternionf rot,int flag);
	
	void RectVertex(sn3DObject * mesh,Quaternionf rot,int flag);
	
	void SelectPolygonVertex(sn3DObject * m,Quaternionf rot,int flag);

	void SelectRefPolygonVertex(sn3DObject * m,Quaternionf rot,int flag);
	
	bool CheckInArea(Point3f point,std::vector<Point3f> &  line);

	void Clear();

public:

	sn3DRangePick(){}
	
	sn3DRangePick(sn3DRangeModel * model);

protected:

	CSpaceBox m_SpaceBox;

};
}
#endif