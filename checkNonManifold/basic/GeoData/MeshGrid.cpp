#include "MeshGrid.h"

#define Max(a,b) (((a) > (b)) ? (a) : (b))
#define Min(a,b) (((a) < (b)) ? (a) : (b))

MeshGrid::MeshGrid(sn3DCore::sn3DMeshData* MeshData, float resolution)
{
	float eps = 0.001* resolution;
	if (resolution <= 0)
		resolution = 1;

	m_fResolution = resolution;
	m_pMesh = MeshData;
	m_Box = Geom::meshAABB(MeshData);

	m_Box.min.x -= eps;
	m_Box.max.x += eps;
	m_Box.min.y -= eps;
	m_Box.max.y += eps;
	m_Box.min.z -= eps;
	m_Box.max.z += eps;

	m_iDimX = Max(1, (int)ceil((m_Box.max.x - m_Box.min.x) / m_fResolution));
	m_iDimY = Max(1, (int)ceil((m_Box.max.y - m_Box.min.y) / m_fResolution));
	m_iDimZ = Max(1, (int)ceil((m_Box.max.z - m_Box.min.z) / m_fResolution));

	m_iHashSize = 262144;
	m_iHashDimSize = 64;
	InsertTriangles();
	InsertPoints();
	triMark = new bool[m_pMesh->n_faces()];
	BoxCenterToPt = new float[m_vCubes.size()];
}


MeshGrid::~MeshGrid()
{
	delete[] triMark;
	delete[] BoxCenterToPt;
}

void MeshGrid::InsertTriangles()
{
	int i, j,cubeIdxMin[3], cubeIdxMax[3];
	int gx, gy, gz, posTri, count;
	Point3f cubeBase, cubeSize, p1, p2, p3;
	GridCube* pCube;
	std::vector<int> vNumCubes;

	int numTris = m_pMesh->n_faces();

	std::vector<int> vTri2Grid;
	vTri2Grid.reserve(1000);
	m_vTableIdx.resize(m_iHashSize, -1);
	m_vCubes.reserve(1000);
	vNumCubes.resize(numTris, 0);

	cubeSize[0] = cubeSize[1] = cubeSize[2] = m_fResolution;

	for (i = 0; i < numTris; i++)
	{
		Face* f = m_pMesh->F(i);
		p1 = f->P(0);
		p2 = f->P(1);
		p3 = f->P(2);
		GetTriCubeRange(i, cubeIdxMin, cubeIdxMax);

		for (gx = cubeIdxMin[0]; gx <= cubeIdxMax[0]; gx++)
		{
			cubeBase[0] = m_Box.min.x + gx * m_fResolution;
			for (gy = cubeIdxMin[1]; gy <= cubeIdxMax[1]; gy++)
			{
				cubeBase[1] = m_Box.min.y + gy * m_fResolution;
				for (gz = cubeIdxMin[2]; gz <= cubeIdxMax[2]; gz++)
				{
					cubeBase[2] = m_Box.min.z + gz * m_fResolution;

					if (CheckOverlapCubeTriangle(cubeBase, cubeSize, p1, p2, p3) == false)
						continue;
					pCube = GetCreateCube(gx, gy, gz);
					pCube->m_iNumTris++;
					vTri2Grid.push_back(GetCubePos(pCube));
					vNumCubes[i]++;
				}

			}
		}
	}

	m_vCubeTris.resize(vTri2Grid.size());
	if (m_vCubes.size() > 0)
		m_vCubes[0].m_iTriIdx = 0;

	for (i = 1; i < m_vCubes.size(); i++)
		m_vCubes[i].m_iTriIdx = m_vCubes[i - 1].m_iTriIdx + m_vCubes[i - 1].m_iNumTris;

	for (i = 0; i < m_vCubes.size(); i++)
		m_vCubes[i].m_iNumTris = 0;

	int j1 =0, j2 = 0;

	for (i = 0; i < numTris; i++)
	{
		j2 += vNumCubes[i];
		for (j = j1; j < j2; j++)
		{
			pCube = &m_vCubes[vTri2Grid[j]];
			posTri = pCube->m_iTriIdx + pCube->m_iNumTris;
			m_vCubeTris[posTri] = i;
			pCube->m_iNumTris++;
		}
		j1 = j2;
	}
}

bool MeshGrid::CheckOverlapCubeTriangle(Point3f cubePt, Point3f boxSize, Point3f triPt1, Point3f triPt2, Point3f triPt3)
{
	Point3f boxcenter, boxhalfsize;
	Point3f v0, v1, v2;
	Point3f e0, e1, e2;
	float fex, fey, fez;
	float p0, p1, p2;
	float mini, maxi;
	float rad;
	Point3f nrv;
	float d;

	boxhalfsize = boxSize*0.5;
	boxcenter = cubePt + boxhalfsize;
	v0 = triPt1 - boxcenter;
	v1 = triPt2 - boxcenter;
	v2 = triPt3 - boxcenter;

	e0 = v1 - v0;
	e1 = v2 - v1;
	e2 = v0 - v2;

	fex = fabs(e0[0]);
	fey = fabs(e0[1]);
	fez = fabs(e0[2]);

	p0 = e0[2] * v0[1] - e0[1] * v0[2];
	p2 = e0[2] * v2[1] - e0[1] * v2[2];
	mini = Min(p0, p2);
	maxi = Max(p0, p2);
	rad = fez * boxhalfsize[1] + fey * boxhalfsize[2];
	if (mini > rad || maxi < -rad)
		return false;

	p0 = -e0[2] * v0[0] + e0[0] * v0[2];
	p2 = -e0[2] * v2[0] + e0[0] * v2[2];
	mini = Min(p0, p2);
	maxi = Max(p0, p2);
	rad = fez * boxhalfsize[0] + fex * boxhalfsize[2];   
		if (mini > rad || maxi < -rad)
			return false;

	p1 = e0[1] * v1[0] - e0[0] * v1[1];
	p2 = e0[1] * v2[0] - e0[0] * v2[1];
	mini = Min(p1, p2);
	maxi = Max(p1, p2);
	rad = fey * boxhalfsize[0] + fex * boxhalfsize[1];
	if (mini > rad || maxi < -rad)
		return false;

	fex = fabs(e1[0]);
	fey = fabs(e1[1]);
	fez = fabs(e1[2]);

	p0 = e1[2] * v0[1] - e1[1] * v0[2];
	p2 = e1[2] * v2[1] - e1[1] * v2[2];
	mini = Min(p0, p2);
	maxi = Max(p0, p2);
	rad = fez * boxhalfsize[1] + fey * boxhalfsize[2];
	if (mini > rad || maxi < -rad)
		return false;

	p0 = -e1[2] * v0[0] + e1[0] * v0[2];
	p2 = -e1[2] * v2[0] + e1[0] * v2[2];
	mini = Min(p0, p2);
	maxi = Max(p0, p2);
	rad = fez * boxhalfsize[0] + fex * boxhalfsize[2]; 
		if (mini > rad || maxi < -rad)
			return false;

	p0 = e1[1] * v0[0] - e1[0] * v0[1];
	p1 = e1[1] * v1[0] - e1[0] * v1[1];
	mini = Min(p0, p1);
	maxi = Max(p0, p1);
	rad = fey * boxhalfsize[0] + fex * boxhalfsize[1];
	if (mini > rad || maxi < -rad)
		return false;

	fex = fabs(e2[0]);
	fey = fabs(e2[1]);
	fez = fabs(e2[2]);

	p0 = e2[2] * v0[1] - e2[1] * v0[2];
	p1 = e2[2] * v1[1] - e2[1] * v1[2];
	mini = Min(p0, p1);
	maxi = Max(p0, p1);
	rad = fez * boxhalfsize[1] + fey * boxhalfsize[2];
	if (mini > rad || maxi < -rad)
		return false;

	p0 = -e2[2] * v0[0] + e2[0] * v0[2];
	p1 = -e2[2] * v1[0] + e2[0] * v1[2];
	mini = Min(p0, p1);
	maxi = Max(p0, p1);
	rad = fez * boxhalfsize[0] + fex * boxhalfsize[2];
	if (mini > rad || maxi < -rad)
		return false;

	p1 = e2[1] * v1[0] - e2[0] * v1[1];
	p2 = e2[1] * v2[0] - e2[0] * v2[1];
	mini = Min(p1, p2);
	maxi = Max(p1, p2);
	rad = fey * boxhalfsize[0] + fex * boxhalfsize[1];
	if (mini > rad || maxi < -rad)
		return false;


	mini = Min(v0[0], Min(v1[0], v2[0]));
	maxi = Max(v0[0], Max(v1[0], v2[0]));
	if (mini > boxhalfsize[0] || maxi < -boxhalfsize[0])
		return false;

	mini = Min(v0[1], Min(v1[1], v2[1]));
	maxi = Max(v0[1], Max(v1[1], v2[1]));
	if (mini > boxhalfsize[1] || maxi < -boxhalfsize[1])
		return false;

	mini = Min(v0[2], Min(v1[2], v2[2]));
	maxi = Max(v0[2], Max(v1[2], v2[2]));
	if (mini > boxhalfsize[2] || maxi < -boxhalfsize[2])
		return false;

	nrv = e0.Cross(e1);
	d = -nrv * v0;
	if (!PlaneBoxOverlap(nrv, d, boxhalfsize))
		return false;
	return true;
}

bool MeshGrid::PlaneBoxOverlap(Point3f nrv, float d, Point3f boxhalfsize)
{
	Point3f vmin, vmax;
	for (int i = 0; i < 3; i++)
	{
		if (nrv[i] > 0.0f)
		{
			vmin[i] = -boxhalfsize[i];
			vmax[i] = boxhalfsize[i];
		}
		else
		{
			vmin[i] = boxhalfsize[i];
			vmax[i] = -boxhalfsize[i];
		}
	}

	if (nrv * vmin + d > 0.0f)
		return false;

	if (nrv * vmax + d  >= 0.0f)
		return true;

	return false;
}

void MeshGrid::GetTriCubeRange(int triIdx, int CubeIdxMin[3], int CubeIdxMax[3])
{
	Point3f p1, p2, p3, pmin, pmax;

	Face* f = m_pMesh->F(triIdx);
	p1 = f->P(0);
	p2 = f->P(1);
	p3 = f->P(2);

	pmin[0] = Min(p1[0], Min(p2[0], p3[0]));
	pmin[1] = Min(p1[1], Min(p2[1], p3[1]));
	pmin[2] = Min(p1[2], Min(p2[2], p3[2]));
	pmax[0] = Max(p1[0], Max(p2[0], p3[0]));
	pmax[1] = Max(p1[1], Max(p2[1], p3[1]));
	pmax[2] = Max(p1[2], Max(p2[2], p3[2]));

	GetPtCube(pmin, CubeIdxMin[0], CubeIdxMin[1], CubeIdxMin[2]);
	GetPtCube(pmax, CubeIdxMax[0], CubeIdxMax[1], CubeIdxMax[2]);
}

GridCube* MeshGrid::GetCreateCube(int gx, int gy, int gz)
{
	int hashIdx, cubePos;
	bool found;
	GridCube newCube, * pCube;
	hashIdx = GetHashIdx(gx, gy, gz);

	cubePos = m_vTableIdx[hashIdx];
	pCube = NULL;
	found = false;

	for (;;)
	{
		if (cubePos == -1)
			break;
		pCube = &m_vCubes[cubePos];
		if (pCube->m_iX == gx && pCube->m_iY == gy && pCube->m_iZ == gz)
		{
			found = true;
			break;
		}
		cubePos = pCube->m_iNextCube;
	}

	if (found == true)
		return &m_vCubes[cubePos];

	cubePos = m_vCubes.size();
	if (pCube != NULL)
	{
		pCube->m_iNextCube = cubePos;
	}
	else
	{
		m_vTableIdx[hashIdx] = cubePos;
	}
	newCube.m_iX = gx;
	newCube.m_iY = gy;
	newCube.m_iZ = gz;
	newCube.m_Center.x = m_Box.min.x + (gx + 0.5f) * m_fResolution;
	newCube.m_Center.y = m_Box.min.y + (gy + 0.5f) * m_fResolution;
	newCube.m_Center.z = m_Box.min.z + (gz + 0.5f) * m_fResolution;
	m_vCubes.push_back(newCube);
	return &m_vCubes[cubePos];
}

int MeshGrid::GetHashIdx(int gx, int gy, int gz)
{
	return (((abs(gx) & (m_iHashDimSize - 1)) * m_iHashDimSize) + (abs(gy) & (m_iHashDimSize - 1))) * m_iHashDimSize + (abs(gz) & (m_iHashDimSize - 1));
}

void MeshGrid::PR_ProcessCube(Point3f pt, float qrad, int gx, int gy, int gz,Face* &pFace, float& qdist, float& s, float& t, int& footCase)
{
	int i, i1, i2, triIdx, footIdx;
	float xmin, xmax, ymin, ymax, zmin, zmax;
	float d2, dx, dy, dz, h1, h2;

	GridCube* pCube;
	if ((pCube = GetCube(gx, gy, gz)) == NULL)
		return;
	
	xmin = m_Box.min.x + gx * m_fResolution;
	xmax = xmin + m_fResolution;
	ymin = m_Box.min.y + gy * m_fResolution;
	ymax = ymin + m_fResolution;
	zmin = m_Box.min.z + gz * m_fResolution;
	zmax = zmin + m_fResolution;

	if (pt[0] < xmin)
		dx = xmin - pt[0];
	else if (pt[0] > xmax)
		dx = pt[0] - xmax;
	else
		dx = 0.0f;

	if (pt[1] < ymin)
		dy = ymin - pt[1];
	else if (pt[1] > ymax)
		dy = pt[1] - ymax;
	else
		dy = 0.0f;

	if (pt[2] < zmin)
		dz = zmin - pt[2];
	else if (pt[2] > zmax)
		dz = pt[2] - zmax;
	else
		dz = 0.0f;

	d2 = dx * dx + dy * dy + dz * dz;

	if (d2 > qrad)
		return;

	if (pFace != NULL)
		if (d2 > qdist)
			return;
	i1 = pCube->m_iTriIdx;
	i2 = i1 + pCube->m_iNumTris;

	Point3f p1, p2, p3;

	for (i = i1; i < i2; i++)
	{
		triIdx = m_vCubeTris[i];
		Face* f = m_pMesh->F(triIdx);
		if (triMark[triIdx])continue;
		triMark[triIdx] = true;
		p1 = f->P(0);
		p2 = f->P(1);
		p3 = f->P(2);

		d2 = CalcSqrDistPtTri(pt, p1, p2, p3, h1, h2, footIdx);

		if (pFace == NULL)
		{
			pFace = m_pMesh->F(triIdx);
			qdist = d2;
			s = h1;
			t = h2;
			footCase = footIdx;
			continue;
		}

		if (d2 < qdist)
		{
			pFace = m_pMesh->F(triIdx);
			qdist = d2;
			s = h1;
			t = h2;
			footCase = footIdx;
		}
	}
}

GridCube* MeshGrid::GetCube(int gx, int gy, int gz)
{
	int hashIdx, cubePos;
	GridCube* pCube = NULL, * pTmpCube;

	hashIdx = GetHashIdx(gx, gy, gz);
	cubePos = m_vTableIdx[hashIdx];
	while (cubePos != -1)
	{
		pTmpCube = &m_vCubes[cubePos];
		if (pTmpCube->m_iX == gx
			&& pTmpCube->m_iY == gy
			&& pTmpCube->m_iZ == gz)
		{
			pCube = pTmpCube;
			break;
		}
		cubePos = pTmpCube->m_iNextCube;
	}
	return pCube;
}


float MeshGrid::CalcSqrDistPtTri(Point3f q, Point3f t1, Point3f t2, Point3f t3, float& s, float& t, int& footCase)
{
	int region;
	Point3f E0, E1, PB;
	float a, b, c, d, e, f, det;
	float tmp0, tmp1, numer, denom;
	float invDet, sqrDist = 0.0f;

	E0 = t2 - t1;
	E1 = t3 - t1;
	PB = t1 - q;
	a = E0 * E0;
	b = E0 * E1;
	c = E1 * E1;
	d = E0 * PB;
	e = E1 * PB;
	f = PB * PB;

	det = fabs(a * c - b * b);
	s = b * e - c * d; 
	t = b * d - a * e; 

	if (s + t <= det)
	{
		if (s < 0.0f)
		{
			if (t < 0.0f)
				region = 4;
			else
				region = 3;
		}
		else if (t < 0.0f)
			region = 5;
		else
			region = 0;
	}
	else
	{
		if (s < 0.0f)
			region = 2;
		else if (t < 0.0f)
			region = 6;
		else
			region = 1;
	}
	switch (region)
	{
	case 0:
		invDet = 1.0f / det;
		s *= invDet;
		t *= invDet;
		sqrDist = s * (a * s + b * t + 2.0f * d) + t * (b * s + c * t + 2.0f * e) + f;
		footCase = FOOT_PT_INNER;
		break;

	case 1:
		numer = c + e - b - d;
		if (numer <= 0.0f)
		{
			s = 0.0f;
			t = 1.0f;
			sqrDist = c + 2.0f * e + f;
			footCase = FOOT_PT_VERTEX3;
		}
		else
		{
			denom = a - 2.0f * b + c;
			if (numer >= denom)
			{
				s = 1.0f;
				t = 0.0f;
				sqrDist = a + 2.0f * d + f;
				footCase = FOOT_PT_VERTEX2;
			}
			else
			{
				s = numer / denom;
				t = 1.0f - s;
				sqrDist = s * (a * s + b * t + 2.0f * d) + t * (b * s + c * t + 2.0f * e) + f;
				footCase = FOOT_PT_EDGE23;
			}
		}

		break;

	case 2:
		tmp0 = b + d;
		tmp1 = c + e;
		if (tmp1 > tmp0)
		{
			numer = tmp1 - tmp0;
			denom = a - 2.0f * b + c;
			if (numer >= denom)
			{
				s = 1.0f;
				t = 0.0f;
				sqrDist = a + 2.0f * d + f;
				footCase = FOOT_PT_VERTEX2;
			}
			else
			{
				s = numer / denom;
				t = 1.0f - s;
				sqrDist = s * (a * s + b * t + 2.0f * d) + t * (b * s + c * t + 2.0f * e) + f;
				footCase = FOOT_PT_EDGE23;
			}
		}
		else
		{
			s = 0.0f;
			if (tmp1 <= 0.0f)
			{
				t = 1.0f;
				sqrDist = c + 2.0f * e + f;
				footCase = FOOT_PT_VERTEX3;
			}
			else if (e >= 0.0f)
			{
				t = 0.0f;
				sqrDist = f;
				footCase = FOOT_PT_VERTEX1;
			}
			else
			{
				t = -e / c;
				sqrDist = e * t + f;
				footCase = FOOT_PT_EDGE31;
			}
		}

		break;

	case 3:
		s = 0.0f;
		if (e >= 0.0f)
		{
			t = 0.0f;
			sqrDist = f;
			footCase = FOOT_PT_VERTEX1;
		}
		else if (-e >= c)
		{
			t = 1.0f;
			sqrDist = c + 2.0f * e + f;
			footCase = FOOT_PT_VERTEX3;
		}
		else
		{
			t = -e / c;
			sqrDist = e * t + f;
			footCase = FOOT_PT_EDGE31;
		}

		break;

	case 4:
		if (d < 0.0f)
		{
			t = 0.0f;
			if (-d >= a)
			{
				s = 1.0f;
				sqrDist = a + 2.0f * d + f;
				footCase = FOOT_PT_VERTEX2;
			}
			else
			{
				s = -d / a;
				sqrDist = d * s + f;
				footCase = FOOT_PT_EDGE12;
			}
		}
		else
		{
			s = 0.0f;
			if (e >= 0.0f)
			{
				t = 0.0f;
				sqrDist = f;
				footCase = FOOT_PT_VERTEX1;
			}
			else if (-e >= c)
			{
				t = 1.0f;
				sqrDist = c + 2.0f * e + f;
				footCase = FOOT_PT_VERTEX3;
			}
			else
			{
				t = -e / c;
				sqrDist = e * t + f;
				footCase = FOOT_PT_EDGE31;
			}
		}

		break;

	case 5:
		t = 0.0f;
		if (d >= 0.0f)
		{
			s = 0.0f;
			sqrDist = f;
			footCase = FOOT_PT_VERTEX1;
		}
		else if (-d >= a)
		{
			s = 1.0f;
			sqrDist = a + 2.0f * d + f;
			footCase = FOOT_PT_VERTEX2;
		}
		else
		{
			s = -d / a;
			sqrDist = d * s + f;
			footCase = FOOT_PT_EDGE12;
		}

		break;

	case 6:
		tmp0 = b + e;
		tmp1 = a + d;
		if (tmp1 > tmp0)
		{
			numer = tmp1 - tmp0;
			denom = a - 2.0f * b + c;
			if (numer >= denom)
			{
				t = 1.0f;
				s = 0.0f;
				sqrDist = c + 2.0f * e + f;
				footCase = FOOT_PT_VERTEX3;
			}
			else
			{
				t = numer / denom;
				s = 1.0f - t;
				sqrDist = s * (a * s + b * t + 2.0f * d) + t * (b * s + c * t + 2.0f * e) + f;
				footCase = FOOT_PT_EDGE23;
			}
		}
		else
		{
			t = 0.0f;
			if (tmp1 <= 0.0f)
			{
				s = 1.0f;
				sqrDist = a + 2.0f * d + f;
				footCase = FOOT_PT_VERTEX2;
			}
			else if (d >= 0.0f)
			{
				s = 0.0f;
				sqrDist = f;
				footCase = FOOT_PT_VERTEX1;
			}
			else
			{
				s = -d / a;
				sqrDist = d * s + f;
				footCase = FOOT_PT_EDGE12;
			}
		}

		break;
	}

	if (_isnan(s) != 0 || _isnan(t) != 0)
	{
		float d1 = SquaredDistance(q, t1);
		float d2 = SquaredDistance(q, t2);
		float d3 = SquaredDistance(q, t3);

		if (d1 <= d2 && d1 <= d3)
		{
			s = 0;
			t = 0;
			sqrDist = d1;
			footCase = FOOT_PT_VERTEX1;
		}
		else if (d2 <= d1 && d2 <= d3)
		{
			s = 1;
			t = 0;
			sqrDist = d2;
			footCase = FOOT_PT_VERTEX2;
		}
		else
		{
			s = 0;
			t = 1;
			sqrDist = d3;
			footCase = FOOT_PT_VERTEX3;
		}
	}

	return fabs(sqrDist);
}

Face* MeshGrid::PointCollisionToMesh(Point3f q, float& s, float& t, float& dist, int& footCase)
{
	float min_Length = FLT_MAX;
	int CubeSize = m_vCubes.size();
	int NumTris = m_pMesh->n_faces();
	for (int i = 0; i < NumTris; i++) triMark[i] = false;

	Point3f p1, p2, p3;
	float minLength = FLT_MAX,tmps,tmpt;
	Face* rMeshTri = NULL;
	int gx, gy, gz;
	if (GetCubeIdx(q, gx, gy, gz))
	{
		int gx1 = Max(gx-1, 0);
		int gx2 = Min(gx+1, m_iDimX - 1);
		int gy1 = Max(gy-1, 0);
		int gy2 = Min(gy+1, m_iDimY - 1);
		int gz1 = Max(gz-1, 0);
		int gz2 = Min(gz+1, m_iDimZ - 1);

		for (int ix = gx1; ix <= gx2; ix++)
			for (int iy = gy1; iy <= gy2; iy++)
				for (int iz = gz1; iz <= gz2; iz++)
				{
					GridCube* tCube = GetCube(ix, iy, iz);
					if (tCube != NULL)
					{
						int begin = tCube->m_iTriIdx;
						int end = begin + tCube->m_iNumTris;
						for (int j = begin; j < end; j++)
						{
							if (!triMark[m_vCubeTris[j]])
							{
								triMark[m_vCubeTris[j]] = true;
								Face* f = m_pMesh->F(m_vCubeTris[j]);
								p1 = f->P(0);
								p2 = f->P(1);
								p3 = f->P(2);
								float d = CalcSqrDistPtTri(q, p1, p2, p3, tmps, tmpt, footCase);
								if (minLength > d)
								{
									minLength = d;
									rMeshTri = f;
									s = tmps;
									t = tmpt;
								}
							}
	
						}
					}
				}
	
	}
	dist = sqrt(minLength);
	return rMeshTri;
}

Face* MeshGrid::PointDistanceToMesh(Point3f q, float& s, float& t, float& dist, int& footCase)
{
	float min_Length = FLT_MAX;
	int CubeSize = m_vCubes.size();
	int NumTris = m_pMesh->n_faces();
	for (int i = 0; i < NumTris; i++) triMark[i] = false;

	Point3f p1, p2, p3;
	float minLength = FLT_MAX;
	float tmps, tmpt;
	Face* rMeshTri = NULL;

	for (int i = 0; i < CubeSize; i++)
	{
		Point3f tpt = m_vCubes[i].m_Center;
		float tlen = (tpt - q).Norm();

		if (tlen < min_Length)
		{
			min_Length = tlen;
		}
		BoxCenterToPt[i] = tlen;
	}
	if (min_Length > dist + 1.74 * m_fResolution )
	{
		return rMeshTri;
	}
	min_Length = min_Length < dist ? min_Length : dist;

	float targetLen = min_Length + 1.74 * m_fResolution;
	for (int i = 0; i < CubeSize; i++)
	{
		if (BoxCenterToPt[i] < targetLen)
		{
			int begin = m_vCubes[i].m_iTriIdx;
			int end = begin + m_vCubes[i].m_iNumTris;
			for (int j = begin; j < end; j++)
			{
				if (!triMark[m_vCubeTris[j]])
				{
					triMark[m_vCubeTris[j]] = true;
					Face* f = m_pMesh->F(m_vCubeTris[j]);
					p1 = f->P(0);
					p2 = f->P(1);
					p3 = f->P(2);
					float d = CalcSqrDistPtTri(q, p1, p2, p3, tmps, tmpt, footCase);
					if (minLength > d)
					{
						minLength = d;
						rMeshTri = m_pMesh->F(m_vCubeTris[j]);
						s = tmps;
						t = tmpt;
					}
				}

			}
		}
	}

	dist = sqrt(minLength);

	return rMeshTri;
}

Face* MeshGrid::ProjectPointToMeshByFace(Point3f q, float rad, float& s, float& t, float& dist, int& footCase)
{
	int i;
	int qgx, qgy, qgz;
	int numsteps;
	int gx, gy, gz, gx1, gx2, gy1, gy2, gz1, gz2;
	float d, qdistmin = 0.0f;
	Face* pMinTri = NULL;

	GetPtCube(q, qgx, qgy, qgz);
	numsteps = (int)ceil(rad / m_fResolution);
	float qrad = rad * rad;
	int iNumTris = m_pMesh->fn;
	
	for (int i = 0; i < iNumTris; i++)
	{
		triMark[i] = false;
	}
	for (i = 0; i <= numsteps; i++)
	{
		if (pMinTri != NULL)
		{
			d = (i - 1) * m_fResolution;
			if (d * d > qdistmin)
				break;
		}

		gx1 = Max(qgx - i, 0);
		gx2 = Min(qgx + i, m_iDimX - 1);
		gy1 = Max(qgy - i, 0);
		gy2 = Min(qgy + i, m_iDimY - 1);
		gz1 = Max(qgz - i, 0);
		gz2 = Min(qgz + i, m_iDimZ - 1);

		for (gy = gy1; gy <= gy2; gy++)
			for (gz = gz1; gz <= gz2; gz++)
			{
				PR_ProcessCube(q, qrad, gx1, gy, gz, pMinTri, qdistmin, s, t, footCase);
				if (gx2 > gx1)
					PR_ProcessCube(q, qrad, gx2, gy, gz, pMinTri, qdistmin, s, t, footCase);
			}

		for (gx = gx1 + 1; gx < gx2; gx++)
			for (gz = gz1; gz <= gz2; gz++)
			{
				PR_ProcessCube(q, qrad, gx, gy1, gz, pMinTri, qdistmin, s, t, footCase);
				if (gy2 > gy1)
					PR_ProcessCube(q, qrad, gx, gy2, gz, pMinTri, qdistmin, s, t, footCase);
			}

		for (gx = gx1 + 1; gx < gx2; gx++)
			for (gy = gy1 + 1; gy < gy2; gy++)
			{
				PR_ProcessCube(q, qrad, gx, gy, gz1, pMinTri, qdistmin, s, t, footCase);
				if (gz2 > gz1)
					PR_ProcessCube(q, qrad, gx, gy, gz2, pMinTri, qdistmin, s, t, footCase);
			}
	}
	if (pMinTri == NULL) 
		return NULL;

	if (qdistmin > qrad)
		return NULL;

	dist = sqrt(qdistmin);
	return pMinTri;
}

int MeshGrid::IntersectRayWCoordPlane(Point3f pt, Point3f dir, float c, char type, Point3f& ipt)
{
	float t;
	if (type != 'X' && type != 'Y' && type != 'Z')
		return 0;

	switch (type)
	{
	case 'X':
		if (fabs(dir[0]) < 0.000001)
			return -1; 
		t = (c - pt[0]) / dir[0];
		if (t < 0) 
			return 0;
		ipt[0] = c;
		ipt[1] = pt[1] + t * dir[1];
		ipt[2] = pt[2] + t * dir[2];
		return 1;

	case 'Y':
		if (fabs(dir[1]) < 0.000001)
			return -1; 
		t = (c - pt[1]) / dir[1];
		if (t < 0) 
			return 0;
		ipt[0] = pt[0] + t * dir[0];
		ipt[1] = c;
		ipt[2] = pt[2] + t * dir[2];
		return 1;

		break;
	case 'Z':
		if (fabs(dir[2]) < 0.000001)
			return -1; 
		t = (c - pt[2]) / dir[2];
		if (t < 0) 
			return 0;
		ipt[0] = pt[0] + t * dir[0];
		ipt[1] = pt[1] + t * dir[1];
		ipt[2] = c;
		return 1;

		break;
	default:
		return 0;
	}
}

bool MeshGrid::IntersectRayWCube(Point3f pt, Point3f dir, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, Point3f &hitPt, int &FaceNum)
{
	Point3f ipt;
	float qdist, minqdist;
	int face;

	minqdist = -1;
	if (IntersectRayWCoordPlane(pt, dir, xMin, 'X', ipt) == 1)
	{
		if (yMin <= ipt[1] && ipt[1] <= yMax && zMin <= ipt[2] && ipt[2] <= zMax)
		{
			if (minqdist < 0)
			{
				minqdist = SquaredDistance(pt, ipt);
				hitPt = ipt;
				face = 4;
			}
			else
			{
				qdist = SquaredDistance(pt, ipt);
				if (qdist < minqdist)
				{
					minqdist = qdist;
					hitPt = ipt;
					face = 4;
				}
			}
		}
	}

	if (IntersectRayWCoordPlane(pt, dir, xMax, 'X', ipt) == 1)
	{
		if (yMin <= ipt[1] && ipt[1] <= yMax && zMin <= ipt[2] && ipt[2] <= zMax)
		{
			if (minqdist < 0)
			{
				minqdist = SquaredDistance(pt, ipt);
				hitPt = ipt;
				face = 2;
			}
			else
			{
				qdist = SquaredDistance(pt, ipt);
				if (qdist < minqdist)
				{
					minqdist = qdist;
					hitPt = ipt;
					face = 2;
				}
			}
		}
	}

	if (IntersectRayWCoordPlane(pt, dir, yMin, 'Y', ipt) == 1)
	{
		if (xMin <= ipt[0] && ipt[0] <= xMax && zMin <= ipt[2] && ipt[2] <= zMax)
		{
			if (minqdist < 0)
			{
				minqdist = SquaredDistance(pt, ipt);
				hitPt = ipt;
				face = 1;
			}
			else
			{
				qdist = SquaredDistance(pt, ipt);
				if (qdist < minqdist)
				{
					minqdist = qdist;
					hitPt = ipt;
					face = 1;
				}
			}
		}
	}

	if (IntersectRayWCoordPlane(pt, dir, yMax, 'Y', ipt) == 1)
	{
		if (xMin <= ipt[0] && ipt[0] <= xMax && zMin <= ipt[2] && ipt[2] <= zMax)
		{
			if (minqdist < 0)
			{
				minqdist = SquaredDistance(pt, ipt);
				hitPt = ipt;
				face = 3;
			}
			else
			{
				qdist = SquaredDistance(pt, ipt);
				if (qdist < minqdist)
				{
					minqdist = qdist;
					hitPt = ipt;
					face = 3;
				}
			}
		}
	}

	if (IntersectRayWCoordPlane(pt, dir, zMin, 'Z', ipt) == 1)
	{
		if (xMin <= ipt[0] && ipt[0] <= xMax && yMin <= ipt[1] && ipt[1] <= yMax)
		{
			if (minqdist < 0)
			{
				minqdist = SquaredDistance(pt, ipt);
				hitPt = ipt;
				face = 5;
			}
			else
			{
				qdist = SquaredDistance(pt, ipt);
				if (qdist < minqdist)
				{
					minqdist = qdist;
					hitPt = ipt;
					face = 5;
				}
			}
		}
	}

	if (IntersectRayWCoordPlane(pt, dir, zMax, 'Z', ipt) == 1)
	{
		if (xMin <= ipt[0] && ipt[0] <= xMax && yMin <= ipt[1] && ipt[1] <= yMax)
		{
			if (minqdist < 0)
			{
				minqdist = SquaredDistance(pt, ipt);
				hitPt = ipt;
				face = 6;
			}
			else
			{
				qdist = SquaredDistance(pt, ipt);
				if (qdist < minqdist)
				{
					minqdist = qdist;
					hitPt = ipt;
					face = 6;
				}
			}
		}
	}

	if (minqdist < 0)
		return false;

	FaceNum = face;
	return true;
}

bool MeshGrid::CalcTriParams(Point3f p, Point3f p1, Point3f p2, Point3f p3, Point3f nrv, int& iCase)
{
	float nx = fabs(nrv[0]), ny = fabs(nrv[1]), nz = fabs(nrv[2]);
	double r, s, t;

	if (CalcTriParams(p, p1, p2, p3, nrv, r, s, t) == false)
		return false;

	// check r, s, t
	if (r < 0 || r > 1)
		return false;
	if (s < 0 || s > 1)
		return false;
	if (t < 0 || t > 1)
		return false;

	if (s == 0 && t == 0)
		iCase = FOOT_PT_VERTEX1;
	else if (r == 0 && t == 0)
		iCase = FOOT_PT_VERTEX2;
	else if (r == 0 && s == 0)
		iCase = FOOT_PT_VERTEX3;
	else if (t == 0)
		iCase = FOOT_PT_EDGE12;
	else if (r == 0)
		iCase = FOOT_PT_EDGE23;
	else if (s == 0)
		iCase = FOOT_PT_EDGE31;
	else
		iCase = FOOT_PT_INNER;

	return true;
}

bool MeshGrid::CalcTriParams(Point3f p, Point3f p1, Point3f p2, Point3f p3, Point3f nrv, double& r, double& s, double& t)
{
	int x, y;
	float nx = fabs(nrv[0]), ny = fabs(nrv[1]), nz = fabs(nrv[2]);
	if (nx >= ny && nx >= nz)
	{
		x = 1;
		y = 2;
	}
	else if (ny >= nx && ny >= nz)
	{
		x = 0;
		y = 2;
	}
	else
	{
		x = 0;
		y = 1;
	}

	if (SolveLinSys2x2(p2[x] - p1[x], p3[x] - p1[x], p2[y] - p1[y], p3[y] - p1[y], p[x] - p1[x], p[y] - p1[y], r, s) < 0)
		return false;
	t = 1 - (r + s);
	return true;
}

int MeshGrid::SolveLinSys2x2(double a11, double a12, double a21, double a22, double b1, double b2, double& x, double& y)
{
	double D = a11 * a22 - a12 * a21;

	if (D == 0)
		return -1;

	x = (b1 * a22 - a12 * b2) / D;
	y = (a11 * b2 - a21 * b1) / D;
	return 0;
}

bool MeshGrid::IntersectRayTriangle(Point3f p, Point3f dir, Point3f p1, Point3f p2, Point3f p3, Point3f nrv, Point3f &hit, float& u, int& iCase)
{
	float dn = dir*nrv;

	if (dn == 0)
		return false;

	u = ((p1[0] - p[0]) * nrv[0] + (p1[1] - p[1]) * nrv[1] + (p1[2] - p[2]) * nrv[2]) / dn;
	if (u < 0)
		return false;

	hit[0] = p[0] + u * dir[0];
	hit[1] = p[1] + u * dir[1];
	hit[2] = p[2] + u * dir[2];

	return CalcTriParams(hit, p1, p2, p3, nrv, iCase);
}

float MeshGrid::IMR_IntersectCubeTrisWRay(Point3f pt, Point3f dir, GridCube* pCube, int& triIdx, Point3f &hitPt, float& t, int& iCase)
{
	int i, i1, i2, ic;
	float u, dmin = -1.0f, d;
	Point3f tpt1, tpt2, tpt3, ipt, nrv;
	Face* pTri;

	i1 = pCube->m_iTriIdx;
	i2 = i1 + pCube->m_iNumTris;

	for (i = i1; i < i2; i++)
	{
		pTri = m_pMesh->F(m_vCubeTris[i]);
		nrv = pTri->N();
		tpt1 = pTri->V(0)->P();
		tpt2 = pTri->V(1)->P();
		tpt3 = pTri->V(2)->P();
		if (IntersectRayTriangle(pt, dir, tpt1, tpt2, tpt3, nrv, ipt, u, ic) == false)
			continue;

		d = SquaredDistance(pt, ipt);

		if (dmin < 0 || d < dmin)
		{
			dmin = d;
			triIdx = m_vCubeTris[i];
			hitPt = ipt;
			iCase = ic;
			t = u;
		}
	}

	if (dmin < 0)
		return dmin;

	return sqrt(fabs(dmin));
}

float MeshGrid::IntersectMeshWithRay(const Point3f& pt, const Point3f& dir, int& triIdx, Point3f &hitPt, int& iCase)
{
	int  gx, gy, gz, stepX = 1, stepY = 1, stepZ = 1,faceNum;
	float len, denom, dist, t;
	float tMaxX, tMaxY, tMaxZ, tDeltaX, tDeltaY, tDeltaZ;
	float eps = 0.001f * m_fResolution;
	Point3f ptRed, hitBBox; Point3f* pHitBBox = NULL;
	GridCube* pCube;

	triIdx = -1;
	len = sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);

	ptRed[0] = pt[0] - (eps / len) * dir[0];
	ptRed[1] = pt[1] - (eps / len) * dir[1];
	ptRed[2] = pt[2] - (eps / len) * dir[2];

	if (dir[0] < 0)
		stepX = -1;
	if (dir[1] < 0)
		stepY = -1;
	if (dir[2] < 0)
		stepZ = -1;


	if (!GetCubeIdx(ptRed, gx, gy, gz))
	{ 
		if (IntersectRayWCube(ptRed, dir, m_Box.min.x, m_Box.max.x, m_Box.min.y, m_Box.max.y, m_Box.min.z, m_Box.max.z, hitBBox, faceNum) == false)
			return -1.0f;

		pHitBBox = &hitBBox;
		ptRed = hitBBox;

		gx = (int)floor((ptRed.x - m_Box.min.x) / m_fResolution);
		gx = Min(Max(0, gx), m_iDimX - 1);
		gy = (int)floor((ptRed.y - m_Box.min.y) / m_fResolution);
		gy = Min(Max(0, gy), m_iDimY - 1);
		gz = (int)floor((ptRed.z - m_Box.min.z) / m_fResolution);
		gz = Min(Max(0, gz), m_iDimZ - 1);
	}

	if (dir[0] >= 0)
	{
		denom = Max(eps, dir[0]);
		tMaxX = (m_Box.min.x + (gx + 1) * m_fResolution - ptRed[0]) / denom;
	}
	else
	{
		denom = Min(-eps, dir[0]);
		tMaxX = (m_Box.min.x + gx * m_fResolution - ptRed[0]) / denom;
	}
	tDeltaX = stepX * m_fResolution / denom;

	if (dir[1] >= 0)
	{
		denom = Max(eps, dir[1]);
		tMaxY = (m_Box.min.y + (gy + 1) * m_fResolution - ptRed[1]) / denom;
	}
	else
	{
		denom = Min(-eps, dir[1]);
		tMaxY = (m_Box.min.y + gy * m_fResolution - ptRed[1]) / denom;
	}
	tDeltaY = stepY * m_fResolution / denom;

	if (dir[2] >= 0)
	{
		denom = Max(eps, dir[2]);
		tMaxZ = (m_Box.min.z + (gz + 1) * m_fResolution - ptRed[2]) / denom;
	}
	else
	{
		denom = Min(-eps, dir[2]);
		tMaxZ = (m_Box.min.z + gz * m_fResolution - ptRed[2]) / denom;
	}
	tDeltaZ = stepZ * m_fResolution / denom;

	for (;;)
	{
		if ((pCube = GetCube(gx, gy, gz)) != NULL)
			if ((dist = IMR_IntersectCubeTrisWRay(ptRed, dir, pCube, triIdx, hitPt, t, iCase)) >= 0.0f)
				if (t >= 0 && t <= tMaxX && t <= tMaxY && t <= tMaxZ)
				{
					return sqrt(SquaredDistance(pt, hitPt));
				}

		if (tMaxX <= tMaxY && tMaxX <= tMaxZ)
		{
			gx += stepX;
			if (gx < 0 || gx == m_iDimX)
				break;

			tMaxX += tDeltaX;
		}
		else if (tMaxY <= tMaxX && tMaxY <= tMaxZ)
		{
			gy += stepY;
			if (gy < 0 || gy == m_iDimY)
				break;

			tMaxY += tDeltaY;
		}
		else
		{
			gz += stepZ;
			if (gz < 0 || gz == m_iDimZ)
				break;
			tMaxZ += tDeltaZ;
		}
	}

	if (pHitBBox == NULL)
	{
		if (IntersectRayWCube(ptRed, dir, m_Box.min.x, m_Box.max.x, m_Box.min.y, m_Box.max.y, m_Box.min.z, m_Box.max.z, hitBBox, faceNum) == false)
			return -1;
		pHitBBox = &hitBBox;
	}
	triIdx = -1;
	hitPt = *pHitBBox;

	return sqrt(SquaredDistance(pt, hitPt));
}

void MeshGrid::InsertPoints()
{
	int numPts = m_pMesh->n_vertices();
	GridCube* pCube;
	int gx, gy, gz;
	std::vector<int> vPtToCubes;
	vPtToCubes.resize(numPts, 0);
	for (int i = 0; i < numPts; i++)
	{
		Point3f pt = m_pMesh->V(i)->P();
		
		GetPtCube(pt, gx, gy, gz);
		pCube = GetCreateCube(gx, gy, gz);
		vPtToCubes[i] = GetCubePos(pCube);
		pCube->m_iNumPts++;
	}
	m_vCubePts.resize(numPts);
	m_vCubes[0].m_iPtIdx = 0;

	for (int i = 1; i < m_vCubes.size(); i++)
		m_vCubes[i].m_iPtIdx = m_vCubes[i - 1].m_iPtIdx + m_vCubes[i - 1].m_iNumPts;
	for (int i = 0; i < m_vCubes.size(); i++)
		m_vCubes[i].m_iNumPts = 0;

	for (int i = 0; i < numPts; i++)
	{
		pCube = &m_vCubes[vPtToCubes[i]];
		int posTri = pCube->m_iPtIdx + pCube->m_iNumPts;
		m_vCubePts[posTri] = i;
		pCube->m_iNumPts++;
	}
	
}

int MeshGrid::PointCollisionToMeshByPoint(Point3f q, float& dist)
{
	float min_Length = FLT_MAX;
	int CubeSize = m_vCubes.size();
	int NumTris = m_pMesh->n_faces();

	int minPtIndex = -1;
	int gx, gy, gz;
	if (GetCubeIdx(q, gx, gy, gz))
	{
		int gx1 = Max(gx - 1, 0);
		int gx2 = Min(gx + 1, m_iDimX - 1);
		int gy1 = Max(gy - 1, 0);
		int gy2 = Min(gy + 1, m_iDimY - 1);
		int gz1 = Max(gz - 1, 0);
		int gz2 = Min(gz + 1, m_iDimZ - 1);

		for (int ix = gx1; ix <= gx2; ix++)
			for (int iy = gy1; iy <= gy2; iy++)
				for (int iz = gz1; iz <= gz2; iz++)
				{
					GridCube* tCube = GetCube(ix, iy, iz);
					if (tCube != NULL)
					{
						int begin = tCube->m_iPtIdx;
						int end = begin + tCube->m_iNumPts;
						for (int j = begin; j < end; j++)
						{
							int tIndex = m_vCubePts[j];
							Point3f tpt = m_pMesh->V(tIndex)->P();
							float length = SquaredDistance(q, tpt);
							if (length< min_Length)
							{
								min_Length = length;
								minPtIndex = tIndex;
							}
						}
					}
				}

	}
	if (minPtIndex != -1)
	{
		dist = sqrt(min_Length);
	}
	return minPtIndex;
}

int MeshGrid::PointDistanceToMeshByPoint(Point3f q, float& dist)
{
	float minBoxLength = FLT_MAX;
	int CubeSize = m_vCubes.size();
	int minPtIndex = -1;
	
	for (int i = 0; i < CubeSize; i++)
	{
		Point3f tpt = m_vCubes[i].m_Center;
		float tlen = (tpt - q).Norm();

		if (tlen < minBoxLength)
		{
			minBoxLength = tlen;
		}
		BoxCenterToPt[i] = tlen;
	}
	if (minBoxLength > dist + 1.74 * m_fResolution )
	{
		return minPtIndex;
	}
	minBoxLength = minBoxLength < dist ? minBoxLength : dist;

	float targetLen = minBoxLength + 1.74 * m_fResolution;

	float minLength = FLT_MAX;
	for (int i = 0; i < CubeSize; i++)
	{
		if (BoxCenterToPt[i] < targetLen)
		{
			int begin = m_vCubes[i].m_iPtIdx;
			int end = begin + m_vCubes[i].m_iNumPts;
			for (int j = begin; j < end; j++)
			{
				int tIndex = m_vCubePts[j];
				Point3f tpt = m_pMesh->V(tIndex)->P();
				float length = SquaredDistance(q, tpt);
				if (length < minLength)
				{
					minLength = length;
					minPtIndex = tIndex;
				}
			}
		}
	}
	dist = sqrt(minLength);
	return minPtIndex;
}
void MeshGrid::IMR_IntersectMeshWInfiniteLine(Point3f q, Point3f dir, std::vector<Face*>& vTris, std::vector<int>& vIsecType, std::vector<Point3f>& vIsectCoords)
{
	int f1, f2;
	float xmin, xmax, ymin, ymax, zmin, zmax, eps = 0.001893f;
	Point3f hit1, hit2;
	// enlarge bbox by eps
	xmin = m_Box.min.x - eps;
	xmax = m_Box.max.x + eps;
	ymin = m_Box.min.y - eps;
	ymax = m_Box.max.y + eps;
	zmin = m_Box.min.z - eps;
	zmax = m_Box.max.z + eps;

	if (IntersectLineWCube(q, dir, xmin, xmax, ymin, ymax, zmin, zmax, hit1, hit2, f1, f2) != true)
		return;

	IMR_IntersectMeshWLine(hit1, hit2, vTris, vIsecType, vIsectCoords);
}

bool MeshGrid::IntersectLineWCube(Point3f pt, Point3f dir, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, Point3f &hit1, Point3f &hit2, int& face1, int& face2)
{
	int cnt = 0;
	Point3f ipt;

	// intersect ray with plane x = xMin (face 4)
	if (IntersectLineWCoordPlane(pt, dir, xMin, 'X', ipt) == 1)
	{
		if (yMin <= ipt[1] && ipt[1] <= yMax && zMin <= ipt[2] && ipt[2] <= zMax)
		{
			if (cnt == 0)
			{
				hit1 = ipt;
				face1 = 4;
				cnt++;
			}
			else
			{
				hit2 = ipt;
				face2 = 4;
				return true;
			}
		}
	}

	// intersect ray with plane x = xMax (face 2)
	if (IntersectLineWCoordPlane(pt, dir, xMax, 'X', ipt) == 1)
	{
		if (yMin <= ipt[1] && ipt[1] <= yMax && zMin <= ipt[2] && ipt[2] <= zMax)
		{
			if (cnt == 0)
			{
				hit1 = ipt;
				face1 = 2;
				cnt++;
			}
			else
			{
				hit2 = ipt;
				face2 = 2;
				return true;
			}
		}
	}

	// intersect ray with plane y = yMin (face 1)
	if (IntersectLineWCoordPlane(pt, dir, yMin, 'Y', ipt) == 1)
	{
		if (xMin <= ipt[0] && ipt[0] <= xMax && zMin <= ipt[2] && ipt[2] <= zMax)
		{
			if (cnt == 0)
			{
				hit1 = ipt;
				face1 = 1;
				cnt++;
			}
			else
			{
				hit2 = ipt;
				face2 = 1;
				return true;
			}
		}
	}

	// intersect ray with plane y = yMax (face 3)
	if (IntersectLineWCoordPlane(pt, dir, yMax, 'Y', ipt) == 1)
	{
		if (xMin <= ipt[0] && ipt[0] <= xMax && zMin <= ipt[2] && ipt[2] <= zMax)
		{
			if (cnt == 0)
			{
				hit1 = ipt;
				face1 = 3;
				cnt++;
			}
			else
			{
				hit2 = ipt;
				face2 = 3;
				return true;
			}
		}
	}

	// intersect ray with plane z = zMin (face 5)
	if (IntersectLineWCoordPlane(pt, dir, zMin, 'Z', ipt) == 1)
	{
		if (xMin <= ipt[0] && ipt[0] <= xMax && yMin <= ipt[1] && ipt[1] <= yMax)
		{
			if (cnt == 0)
			{
				hit1 = ipt;
				face1 = 5;
				cnt++;
			}
			else
			{
				hit2 = ipt;
				face2 = 5;
				return true;
			}
		}
	}

	// intersect ray with plane z = zMax (face 6)
	if (IntersectLineWCoordPlane(pt, dir, zMax, 'Z', ipt) == 1)
	{
		if (xMin <= ipt[0] && ipt[0] <= xMax && yMin <= ipt[1] && ipt[1] <= yMax)
		{
			if (cnt == 0)
			{
				hit1 = ipt;
				face1 = 6;
				cnt++;
			}
			else
			{
				hit2 = ipt;
				face2 = 6;
				return true;
			}
		}
	}

	// < 2 hits
	return false;
}

int MeshGrid::IntersectLineWCoordPlane(Point3f pt, Point3f dir, float c, char type, Point3f &ipt)
{
	float t;

	// check type
	if (type != 'X' && type != 'Y' && type != 'Z')
		return 0;

	switch (type)
	{
	case 'X':
		if (fabs(dir[0]) < 0.000001)
			return -1; // parallel
		t = (c - pt[0]) / dir[0];
		ipt[0] = c;
		ipt[1] = pt[1] + t * dir[1];
		ipt[2] = pt[2] + t * dir[2];
		return 1;

	case 'Y':
		if (fabs(dir[1]) < 0.000001)
			return -1; // parallel
		t = (c - pt[1]) / dir[1];
		ipt[0] = pt[0] + t * dir[0];
		ipt[1] = c;
		ipt[2] = pt[2] + t * dir[2];
		return 1;

		break;
	case 'Z':
		if (fabs(dir[2]) < 0.000001)
			return -1; // parallel
		t = (c - pt[2]) / dir[2];
		ipt[0] = pt[0] + t * dir[0];
		ipt[1] = pt[1] + t * dir[1];
		ipt[2] = c;
		return 1;

		break;
	default:
		return 0;
	}
}

void MeshGrid::IMR_IntersectMeshWLine(Point3f p1, Point3f p2, std::vector<Face*>& vTris, std::vector<int>& vIsecType, std::vector<Point3f>& vIsectCoords)
{
	int i, i1, i2, isecType;
	int gx, gy, gz, stepX = 1, stepY = 1, stepZ = 1, triIdx;
	float qlen, len, denom;
	float tMaxX, tMaxY, tMaxZ, tDeltaX, tDeltaY, tDeltaZ;
	float eps = 0.001f * m_fResolution, u;
	Point3f dir = p2 - p1;
	Point3f tpt1, tpt2, tpt3, ipt, nrv;
	bool bRes;
	Face* pTri;
	GridCube* pCube;
	Point3f hitBBox;
	int fn = m_pMesh->fn;
	for (int i = 0; i < fn; i++)
	{
		triMark[i] = false;
	}
	if ((qlen = SquaredDistance(p1, p2)) == 0)
		return;
	len = sqrt(qlen);
	dir /= len;

	triIdx = -1;
	if (dir[0] < 0)
		stepX = -1;
	if (dir[1] < 0)
		stepY = -1;
	if (dir[2] < 0)
		stepZ = -1;


	if (GetCubeIdx(p1, gx, gy, gz) == false)
	{
		int faceNum;
		if (IntersectRayWCube(p1, dir, m_Box.min.x, m_Box.max.x, m_Box.min.y, m_Box.max.y, m_Box.min.z, m_Box.max.z, hitBBox, faceNum) == false)
			return;

		if (SquaredDistance(p1, hitBBox) > qlen)
			return;

		gx = (int)floor((hitBBox[0] - m_Box.min.x) / m_fResolution);
		gx = Min(Max(0, gx), m_iDimX - 1);

		gy = (int)floor((hitBBox[1] - m_Box.min.y) / m_fResolution);
		gy = Min(Max(0, gy), m_iDimY - 1);

		gz = (int)floor((hitBBox[2] - m_Box.min.z) / m_fResolution);
		gz = Min(Max(0, gz), m_iDimZ - 1);
	}

	if (dir[0] >= 0)
	{
		denom = Max(eps, dir[0]);
		tMaxX = (m_Box.min.x + (gx + 1) * m_fResolution - p1[0]) / denom;
	}
	else
	{
		denom = Min(-eps, dir[0]);
		tMaxX = (m_Box.min.x + gx * m_fResolution - p1[0]) / denom;
	}
	tDeltaX = stepX * m_fResolution / denom;

	if (dir[1] >= 0)
	{
		denom = Max(eps, dir[1]);
		tMaxY = (m_Box.min.y + (gy + 1) * m_fResolution - p1[1]) / denom;
	}
	else
	{
		denom = Min(-eps, dir[1]);
		tMaxY = (m_Box.min.y + gy * m_fResolution - p1[1]) / denom;
	}
	tDeltaY = stepY * m_fResolution / denom;

	if (dir[2] >= 0)
	{
		denom = Max(eps, dir[2]);
		tMaxZ = (m_Box.min.z + (gz + 1) * m_fResolution - p1[2]) / denom;
	}
	else
	{
		denom = Min(-eps, dir[2]);
		tMaxZ = (m_Box.min.z + gz * m_fResolution - p1[2]) / denom;
	}
	tDeltaZ = stepZ * m_fResolution / denom;

	for (;;)
	{
		if ((pCube = GetCube(gx, gy, gz)) != NULL)
		{
			i1 = pCube->m_iTriIdx;
			i2 = i1 + pCube->m_iNumTris;

			for (i = i1; i < i2; i++)
			{
				pTri = m_pMesh->F(m_vCubeTris[i]);

				if (triMark[m_vCubeTris[i]])
					continue;

				tpt1 = pTri->P(0);
				tpt2 = pTri->P(1);
				tpt3 = pTri->P(2);
				nrv = ((tpt2 - tpt1) ^ (tpt3 - tpt1)).Normalize();
	
				triMark [m_vCubeTris[i]]= true;

				bRes = IntersectRayTriangle(p1, dir, tpt1, tpt2, tpt3, nrv, ipt, u, isecType);

				if (bRes == false)
					continue;

				vTris.push_back(pTri);
				vIsecType.push_back(isecType);
				vIsectCoords.push_back(ipt);
			}
		}

		if (tMaxX <= tMaxY && tMaxX <= tMaxZ)
		{
			if (tMaxX > len)
				break;

			gx += stepX;

			if (gx < 0 || gx == m_iDimX)
				break;

			tMaxX += tDeltaX;
		}
		else if (tMaxY <= tMaxX && tMaxY <= tMaxZ)
		{
			if (tMaxY > len)
				break;

			gy += stepY;

			if (gy < 0 || gy == m_iDimY)
				break;

			tMaxY += tDeltaY;
		}
		else
		{
			if (tMaxZ > len)
				break;

			gz += stepZ;

			if (gz < 0 || gz == m_iDimZ)
				break;

			tMaxZ += tDeltaZ;
		}
	}
}