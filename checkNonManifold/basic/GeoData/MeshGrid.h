#ifndef  _SN3D_MESH_GRID_
#define  _SN3D_MESH_GRID_
#include "MeshData.h"

namespace sn3DCore
{
	#define FOOT_PT_INNER		0
	#define FOOT_PT_EDGE12		1
	#define FOOT_PT_EDGE23		2
	#define FOOT_PT_EDGE31		3
	#define FOOT_PT_VERTEX1		4
	#define FOOT_PT_VERTEX2		5
	#define FOOT_PT_VERTEX3		6
	class GridCube
	{
	public:
		//! Constructor
		GridCube() { m_iX = m_iY = m_iZ = m_iTriIdx = m_iNumTris = m_iPtIdx = m_iNumPts = 0; m_iNextCube = -1; }
		int m_iX, m_iY, m_iZ;	    /// 3D Grid index
		int m_iNextCube;			/// Index of next cube having same hash index
		int m_iTriIdx;				/// Index of first triangle
		int m_iNumTris;				/// Num of triangle
		int m_iPtIdx;				/// Index of first point
		int m_iNumPts;				/// Num of Point

		Point3f m_Center;
	};
	class MeshGrid
	{
	public:
		MeshGrid(sn3DMeshData* pMeshdata, float resolution);
		virtual ~MeshGrid();
	public:
		sn3DMeshData* m_pMesh;
	private:
		float m_fResolution;
		Box3f m_Box;
		int m_iDimX, m_iDimY, m_iDimZ;

		int m_iHashSize; 									
		int m_iHashDimSize;

		std::vector<GridCube> m_vCubes;
		std::vector<int> m_vTableIdx;
		std::vector<int> m_vCubeTris;
		std::vector<int> m_vCubePts;

		float* BoxCenterToPt; //temporary data, length between pt and center of box.
		bool* triMark;        //temporary data, mark of triangle;
	private:
		void InsertTriangles();
		void InsertPoints();
		bool PlaneBoxOverlap(Point3f nrv, float d, Point3f boxhalfsize);
		void GetTriCubeRange(int triIdx, int CubeIdxMin[3], int CubeIdxMax[3]);
		GridCube* GetCreateCube(int gx, int gy, int gz);
		int GetHashIdx(int gx, int gy, int gz);
		inline int GetCubePos(GridCube* pCube) { return (int)(pCube - &(m_vCubes.at(0))); }
		bool CheckOverlapCubeTriangle(Point3f cubePt, Point3f boxSize, Point3f triPt1, Point3f triPt2, Point3f triPt3);
		int IntersectRayWCoordPlane(Point3f pt, Point3f dir, float c, char type, Point3f& ipt);

		bool IntersectRayWCube(Point3f pt, Point3f dir, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, Point3f& hitPt, int& FaceNum);
		bool CalcTriParams(Point3f p, Point3f p1, Point3f p2, Point3f p3, Point3f nrv, int& iCase);
		bool CalcTriParams(Point3f p, Point3f p1, Point3f p2, Point3f p3, Point3f nrv, double& r, double& s, double& t);
		int SolveLinSys2x2(double a11, double a12, double a21, double a22, double b1, double b2, double& x, double& y);
		bool IntersectRayTriangle(Point3f p, Point3f dir, Point3f p1, Point3f p2, Point3f p3, Point3f nrv, Point3f &hit, float& u, int& iCase);
		float IMR_IntersectCubeTrisWRay(Point3f pt, Point3f dir, GridCube* pCube, int& triIdx, Point3f &hitPt, float& t, int& iCase);
	public:
		inline Box3f getGridBox()
		{
			return m_Box;
		}
		inline void GetPtCube(const Point3f &pt, int& gx, int& gy, int& gz)
		{
			gx = (int)floor((pt.x - m_Box.min.x) / m_fResolution);
			gy = (int)floor((pt.y - m_Box.min.y) / m_fResolution);
			gz = (int)floor((pt.z - m_Box.min.z) / m_fResolution);
		}
		bool GetCubeIdx(const Point3f& pt, int& gx, int& gy, int& gz)
		{
			gx = (int)floor((pt.x - m_Box.min.x) / m_fResolution);
			if (gx < 0 || gx >= m_iDimX)
				return false;

			gy = (int)floor((pt.y - m_Box.min.y) / m_fResolution);
			if (gy < 0 || gy >= m_iDimY)
				return false;

			gz = (int)floor((pt.z - m_Box.min.z) / m_fResolution);
			if (gz < 0 || gz >= m_iDimZ)
				return false;

			return true;
		}
		GridCube* GetCube(int gx, int gy, int gz);
		void PR_ProcessCube(Point3f pt, float qrad, int gx, int gy, int gz, Face* &pFace, float& qdist, float& s, float& t, int& footCase);
		float CalcSqrDistPtTri(Point3f q, Point3f t1, Point3f t2, Point3f t3, float& s, float& t, int& footCase);
		float GetResolution() { return m_fResolution; }
		Face* PointCollisionToMesh(Point3f q, float& s, float& t, float& dist, int& footCase);
		Face* PointDistanceToMesh(Point3f q, float& s, float& t, float& dist, int& footCase);
		Face* ProjectPointToMeshByFace(Point3f q, float rad, float& s, float& t, float& dist, int& footCase);

		int PointCollisionToMeshByPoint(Point3f q, float& dist);
		int PointDistanceToMeshByPoint(Point3f q, float& dist);


		void IMR_IntersectMeshWInfiniteLine(Point3f q, Point3f dir, std::vector<Face*>& vTris, std::vector<int>& vIsecType, std::vector<Point3f>& vIsectCoords);
		float IntersectMeshWithRay(const Point3f& pt, const Point3f& dir, int& triIdx, Point3f &hitPt, int& iCase);
		int IntersectLineWCoordPlane(Point3f pt, Point3f dir, float c, char type, Point3f &ipt);
		bool IntersectLineWCube(Point3f pt, Point3f dir, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, Point3f &hit1, Point3f &hit2, int& face1, int& face2);
		void IMR_IntersectMeshWLine(Point3f p1, Point3f p2, std::vector<Face*>& vTris, std::vector<int>& vIsecType, std::vector<Point3f>& vIsectCoords);
	};
}


#endif
