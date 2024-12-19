#ifndef _SN3DMESHPICK_
#define _SN3DMESHPICK_

#include"sn3DPick.h"
#include"../../Geometry/SpaceBox.h"
#include"../../SceneGraph/sn3DMeshModel.h"

namespace sn3DCore
{
 
class sn3DMeshPick : public sn3DPick
{
public:

    void PaintVertex(sn3DObject * mesh,Quaternionf rot,int flag);
	
	void RectVertex(sn3DObject * mesh,Quaternionf rot,int flag);
	
	void SelectPolygonVertex(sn3DObject * m,Quaternionf rot,int flag);
	
	bool CheckInArea(Point3f point,std::vector<Point3f> & line);

	void Clear();
public: 

	sn3DMeshPick(){};
	sn3DMeshPick(sn3DMeshModel * model);

protected:

	CSpaceBox m_SpaceBox;
	
};
}
#endif