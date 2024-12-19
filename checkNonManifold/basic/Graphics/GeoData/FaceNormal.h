#ifndef FACE_NORMAL_H
#define FACE_NORMAL_H

#include <vector>
#include <map>

#include "Common/point3.h"

/* 分裂法向定义 */

/// 分裂法向
namespace sn3DCore
{
	struct FaceNormal
	{
		FaceNormal()
		{
			face_id = -1;
		}
		FaceNormal(const Point3f& n, int faceId = -1)
		{
			normal = n;
			face_id = faceId;
		}
		Point3f normal;		// 面片法向
		int face_id;		// 面片Id
	};
	typedef std::vector<FaceNormal> VecFaceNormal;

	/// 顶点分裂法向数据结构：map<VertexId, vector<Normal> >
	typedef  std::map<int, VecFaceNormal > VertexNormalMap;
}
#endif