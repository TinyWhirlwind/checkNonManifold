#include "KDTree2.h"

#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846264
#endif

#define MEDIAN_SPLIT 0

#if MEDIAN_SPLIT
#include "Median.h"
static Median<float> med;
#endif

#pragma warning (disable : 4244)

KDTree2* CreateKDtree(sn3DMeshData * mesh, int nPts)
{

	if (!nPts) return NULL;

	int *inds = new int[nPts];

	for (int i = 0; i < nPts; i++) inds[i] = i;

	KDTree2* kdtree = new KDTree2(mesh, inds, mesh->vn);

	delete[] inds;

	return kdtree;
}
static int divisionsorts(sn3DMeshData * mesh, int *p, int n, int dim, float med)
{
	int left = 0, right = n - 1;
	while (1)
	{
		while (mesh->V(p[left])->P()[dim] <= med && right > left) left++;
		while (mesh->V(p[right])->P()[dim] > med && right > left) right--;
		int tmp = p[left];  // swap
		p[left] = p[right]; p[right] = tmp;
		if (left == right)
		{
			if (mesh->V(p[right])->P()[dim] <= med) right++;
			break;
		}
		left++;
	}
	return right;
}

void merge_normal_cone(const Point3f &n1, float th1, const Point3f &n2, float th2, Point3f &n3, float &th3)
{
	float val = n1[0] * n2[0] + n1[1] * n2[1] + n1[2] * n2[2];
	// first, get the angle between the normals
	if (val > 1)
	{
		val = 1;
	}
	if (val < -1)val = -1;

	th3 = (float)(.5 * (th1 + th2) + acos(val));

	// full sphere?
	if (th3 >= M_PI)
	{
		th3 = (float)M_PI;
		n3[0] = 0;
		n3[1] = 0;
		n3[2] = 1;

		return;
	}

	// figure how much n1 should be rotated to become n3
	float half_ang = float(.25*(th3 - th1));
	// create quaternion (real part and imaginary part)
	// for rotation
	float qr = cos(half_ang);
	//Pnt3 qim = cross(n1,n2);

	Point3f qim(n1[1] * n2[2] - n1[2] * n2[1],
		n1[2] * n2[0] - n1[0] * n2[2],
		n1[0] * n2[1] - n1[1] * n2[0]);

	float si = sin(half_ang);

	qim[0] *= si;
	qim[1] *= si;
	qim[2] *= si;
	//qim *= sin(half_ang);

	Point3f pim(qim[1] * n1[2] - qim[2] * n1[1],
		qim[2] * n1[0] - qim[0] * n1[2],
		qim[0] * n1[1] - qim[1] * n1[0]);

	// apply rotation
	(((n3 = n1) *= (qr*qr - (qim[0] * qim[0] + qim[1] * qim[1] + qim[2] * qim[2]))) += (pim *= (2.0*qr))) += (qim *= (2.0*(n1[0] * qim[0] + n1[1] * qim[1] + n1[2] * qim[2])));

}
KDTree2::KDTree2(sn3DMeshData * mesh, int *ind, int n, int first)
	: Nhere(0), element(NULL)
{

	// find the dimension of maximum range
	//min[0] =  pts[ind[0]][0];
	//min[1] =  pts[ind[0]][1];
	//min[2] =  pts[ind[0]][2];

	max = min = mesh->V(ind[0])->P();

	/*int *end = ind+n;

	for(int *ip=ind+1; ip<end; ip++)
	{
	  const Pnt3 &p = pts[*ip];

	  if (p[0] < min[0]) min[0] = p[0];
	  if (p[1] < min[1]) min[1] = p[1];
	  if (p[2] < min[2]) min[2] = p[2];

	  if (p[0] > max[0]) max[0] = p[0];
	  if (p[1] > max[1]) max[1] = p[1];
	  if (p[2] > max[2]) max[2] = p[2];

	}*/

	int end = (int)mesh->m_vertices.size();

	for (int i = 1; i < n; i++)
	{
		Point3f p = mesh->V(ind[i])->P();

		if (p[0] < min[0]) min[0] = p[0];
		if (p[1] < min[1]) min[1] = p[1];
		if (p[2] < min[2]) min[2] = p[2];

		if (p[0] > max[0]) max[0] = p[0];
		if (p[1] > max[1]) max[1] = p[1];
		if (p[2] > max[2]) max[2] = p[2];

	}


	////////////////////  decide dimesion ......
	float dist = max[0] - min[0];
	m_d = 0;
	float tmp;
	if ((tmp = max[1] - min[1]) > dist)
	{
		m_d = 1; dist = tmp;
	}
	if ((tmp = max[2] - min[2]) > dist)
	{
		m_d = 2; dist = tmp;
	}

	if (dist == 0.0) n = 1; // a single point several times

	if (n > 16) {
#if MEDIAN_SPLIT
#define DATA(i) (pts[ind[i]])[m_d]
		// find the median within that dimension
		med.clear();
		for (i = 0; i < n; i++) med += DATA(i);
		m_p = med.find();
		int right = divisionsort(pts, ind, n, m_d, m_p);
		if (right == n) {
			// the median is also the largest, need new "median"
			// find the next largest item for that
			float nm = -9.e33;
			for (i = 0; i < n; i++) {
				if (DATA(i) != m_p && DATA(i) > nm) nm = DATA(i);
			}
			right = divisionsort(pts, ind, n, m_d, (m_p = nm));
		}
		assert(right != 0 && right != n);
#undef DATA
#else
		m_p = .5*(max[m_d] + min[m_d]);
		int right = divisionsorts(mesh, ind, n, m_d, m_p);
		assert(right != 0 && right != n);
#endif

		// recurse
		child[0] = new KDTree2(mesh, ind, right, 0);
		child[1] = new KDTree2(mesh, &ind[right], n - right, 0);
	}
	else
	{
		// store data here
		Nhere = n;
		element = new int[n];
		for (int i = 0; i < n; i++) element[i] = ind[i];
		child[0] = child[1] = NULL;
	}

#if MEDIAN_SPLIT
	if (first) med.zap(); // release memory after the tree's done
#endif

  //if (nrms == NULL) return;

  // now figure out bounds for the normals
 /* if (Nhere)
  {
	// a terminal node
	//normal = GetNormalAsPnt3(nrms, element[0]);

	normal[0] = mesh->V(element[0])->N().X();
	normal[1] = mesh->V(element[0])->N().Y();
	normal[2] = mesh->V(element[0])->N().Z();

	theta  = 0.0;
	for (int i=1; i<Nhere; i++)
	{
	  //merge_normal_cones(normal, theta,GetNormalAsPnt3(nrms, element[i]), 0,normal, theta);
		merge_normal_cone(normal, theta,mesh->V(element[i])->N(), 0,normal, theta);
	}
  }
  else
  {
	// a non-terminal node
	merge_normal_cone(child[0]->normal, child[0]->theta,child[1]->normal, child[1]->theta,normal, theta);
  }

  tmp = theta + M_PI * .25;

  if (tmp > M_PI) cos_th_p_pi_over_4 = -1.0;
  else            cos_th_p_pi_over_4 = cos(tmp);*/
}

KDTree2::~KDTree2(void)
{
	delete[] element;
	delete child[0];
	delete child[1];
}

bool KDTree2::_search(sn3DMeshData * mesh, const Point3f &p, int num, std::vector<int> & indices, std::vector<float> & d2) const
{
	assert(this);

	if (Nhere)
	{
		float l;
		bool  need_sqrt = false;
		int *el = element;
		int *end = el + Nhere;
		for (; el < end; el++)
		{
			Point3f diff = mesh->V(*el)->P() - p;

			l = diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2];

			if ((int)indices.size() < num)
			{
				indices.push_back(*el);
				d2.push_back(l);

				int en = indices.size();

				for (int i = en - 1; i > 0; i--)
				{
					if (d2[i] < d2[i - 1])
					{
						int tempI;
						float tempF;
						tempI = indices[i];
						tempF = d2[i];
						d2[i] = d2[i - 1];
						indices[i] = indices[i - 1];
						d2[i - 1] = tempF;
						indices[i - 1] = tempI;
					}
					else break;
				}
			}
			else
			{
				if (l < d2[num - 1])
				{
					indices[num - 1] = *el;
					d2[num - 1] = l;
					int en = indices.size();
					for (int i = en - 1; i > 0; i--)
					{
						if (d2[i] < d2[i - 1])
						{
							int tempI;
							float tempF;
							tempI = indices[i];
							tempF = d2[i];
							d2[i] = d2[i - 1];
							indices[i] = indices[i - 1];
							d2[i - 1] = tempF;
							indices[i - 1] = tempI;
						}
						else break;
					}
				}
			}

		}

		return (indices.size() == num);
	}

	if (p[m_d] <= m_p)
	{
		if (child[0]->_search(mesh, p, num, indices, d2))
			return true;

		if (child[1]->_search(mesh, p, num, indices, d2))
			return true;

	}
	else
	{
		if (child[1]->_search(mesh, p, num, indices, d2))
			return true;

		if (child[0]->_search(mesh, p, num, indices, d2))
			return true;

	}

	return false;
}
