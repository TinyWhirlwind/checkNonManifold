#ifndef _SN3D_GEOMETRY_DATA_
#define _SN3D_GEOMETRY_DATA_

/****************************************
  GeometryData  类
几何数据抽象类，各种几何单元及其属性的存储方法:
点 面 边 信息
法向信息
颜色信息
纹理坐标信息
拓扑结构信息
*****************************************/

#include <vector>
#include <map>

#include "Common/Point2.h"
#include "Common/Point3.h"
#include "Common/Point4.h"
#include "Common/Color4.h"
#include "Common/matrix44.h"
#include "Common/matrix33.h"
#include "FaceNormal.h"

namespace sn3DCore
{
	class Vertex;
	class Face;
	class Edge;
	class VertexTopology;
	class FaceTopology;
	struct FaceNormal;
	typedef Vertex* VertexPointer;
	typedef std::vector<Vertex>::iterator VertexIterator;
	typedef std::vector<Face>::iterator FaceIterator;

	class sn3DGeometryData
	{
	public:
		sn3DGeometryData()	{}
		virtual ~sn3DGeometryData()	{}

		///////////////  几何数据  ///////////////////
		std::vector<Vertex>		m_vertices;     //  点数据(用于表示三角网格 点云表示)
		std::vector<Face>		m_faces;        //  面数据
		std::vector<Edge>		m_edges;        //  物理边
		std::vector<int>		m_pEdges;       //  边索引
		std::vector<Point3f>	m_vertexNormal; //  顶点法向
		std::vector<Point3f>	m_faceNormal;   //  面法向
		std::vector<int>		m_vertexMark;	//  顶点整数标志
		std::vector<int>		m_faceMark;     //  面整数标志
		std::vector<Color4f>	m_vertexColor;  //  顶点颜色
		std::vector<Color4f>	m_faceColor;    //  面颜色
		std::vector<Point2f>		m_vertexTexCoord;	//  顶点纹理坐标
		std::vector<Point2f>		m_faceTexCoord;     //  面纹理坐标
		std::vector<VertexTopology> m_vertexTopology;	//  顶点拓扑
		std::vector<FaceTopology>	m_faceTopology;     //  面拓扑
		std::vector<int>		m_FaceIdentity;			// 面标识
		std::vector<int>		m_FaceIdentity1;
		std::vector<int>		m_FaceIdentity2;
		std::vector<int>		m_VertexIdentity;
		std::vector<float>		m_vertexAttrib;			//定点属性
		std::vector<Point3f>	m_pointMem;				//点云坐标
		std::vector<int>		m_pointFlag;			//点云标记
		std::map<int, std::vector<FaceNormal>> m_vertexSplitNormal;	//顶点分裂法向
	};
}
#endif