#ifndef _TRIMESDHOPERATION_
#define _TRIMESHOPERATION_


#include "../SceneGraph/sn3DMeshData.h"
#include<vector>
using namespace std;
using namespace sn3DCore;
class TriMeshOperation
{
public:

  static float MedianEdgeLength(sn3DMeshData * mesh,int percentile);

  static void RemoveLongEdge(sn3DMeshData * m,int  factor,int  percentile);

  static void ComputeVertexBoundary(sn3DMeshData * mesh);

  static void RandomSamplePoint(sn3DMeshData * mesh,float rate,std::vector<Point3f> &points, vector<Point3f> &normals);

  static void RemoveUnreferenceVertex(sn3DMeshData * mesh);

  static void RemoveDegenerateFace(sn3DMeshData * mesh);

};


#endif