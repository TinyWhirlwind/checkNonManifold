#ifndef _KDTREE_2_H
#define _KDTREE_2_H

#include "Common/point3.h"
#include "GeoData/MeshData.h"

/* KDTree基于Mesh */
namespace sn3DCore
{
	class KDTree2* CreateKDtree(sn3DMeshData * mesh, int nPts);

	class KDTree2
	{
	private:
		int      m_d;
		float    m_p;
		Point3f     min, max;
		int      Nhere;
		KDTree2 *child[2];
		int       *element;
		Point3f    normal;
		float   theta;
		float   cos_th_p_pi_over_4;

	public:
		KDTree2(sn3DMeshData * mesh, int *ind, int n, int first = 1);
		~KDTree2();
		bool _search(sn3DMeshData * mesh, const Point3f &p, int num, std::vector<int> & indices, std::vector<float> & d2) const;
	};
}

#endif
