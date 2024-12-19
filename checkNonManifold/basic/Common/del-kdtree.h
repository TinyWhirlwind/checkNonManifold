#ifndef _KDTREE_
#define _KDTREE_

#include "../sn3DGeometry/point3.h"
#include "../sn3DDataModel/sn3DMeshData.h"

namespace sn3DCore
{

class KDtree* CreateKDtree (sn3DMeshData * mesh,int nPts);

class KDtree 
{
private:
  
  int      m_d;  
  float    m_p; 
  Point3f     min, max;
  int      Nhere; 
  KDtree *child[2];
  int       *element;

 
  Point3f    normal; 
  
  float   theta;
  
  float   cos_th_p_pi_over_4;

public:

   KDtree(sn3DMeshData * mesh,int *ind,int n, int first = 1);

  ~KDtree();

    bool _search(sn3DMeshData * mesh,const Point3f &p,int num,std::vector<int> & indices,std::vector<float> & d2) const;
 
};

}

#endif
