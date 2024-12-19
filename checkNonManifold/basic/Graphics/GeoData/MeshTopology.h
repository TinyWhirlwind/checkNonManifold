#ifndef _SN3D_TOPOLOGY_
#define _SN3D_TOPOLOGY_

/***************************************
 三角网格拓扑结构

 Pos        类 --  半边结构
 VFIterator 类 --  顶点单圈邻面，无序访问
 VVNeighbor 类 --  顶点单圈邻点，顺序访问
 Topology   类 --  创建及查询拓扑信息

****************************************/

#include <vector>
#include "Face.h"
#include "MeshData.h"

namespace sn3DCore
{
	class Pos
	{
	public:

		Vertex * & V();
		Face   * & F();
		int      & I();

		Pos();
		Pos(Face * const fp, int const zp, Vertex * const vp);
		Pos(Face * const fp, int const zp);
		Pos(Face * const fp, Vertex * const vp);

		bool operator == (Pos const & p) const;
		bool operator != (Pos const & p) const;
		bool operator <= (Pos const & p) const;//首先比较面指针，然后比较边索引，最后比较顶点指针

		Pos & operator = (const Pos & h);
		void SetNull();
		bool IsNull() const;

		void Set(Face  * const fp, int const zp, Vertex  * const vp);
		void Set(Face  * const pFace, Vertex  * const pVertex);

		void NextF();  //移动到共边 z的下个面
		void NextE();  //移动到共点 v的下一边
		void NextB();  //移动到下一个边界,要保证当前已在边界位置

		void FlipE();  //点、面不变，边翻转
		void FlipF();  //点、边不变，面翻转
		void FlipV();  //边、面不变，点翻转

		Vertex *VFlip();
		const Vertex *VFlip()const;
		Face *FFlip();
		const Face *FFlip()  const;

		bool IsBorder();
		bool IsManifold();/*检查是否为流形*/

		int NumberOfIncidentVertices();/*返回当前顶点的度,边*/
		int NumberOfIncidentFaces();/*返回当前顶点的度,面*/
		bool Coerent() const;	/*方向一致性检查*/
		void CheckIncidentFaces(int & count, bool & on_border);

	public:
		Face *f;   //面指针
		int z;     //边索引
		Vertex *v; //顶点指针
	};

	class VFIterator
	{
	public:
		Face *&	F() { return f; }
		int	&			I() { return z; }
		bool End() { return f == 0; }
		void operator++(void)
		{
			Face * t = f;
			f = t->VFp(z);
			z = t->VFi(z);
		}
	public:
		VFIterator() {}
		VFIterator(Face * _f, const int &  _z) { f = _f; z = _z; }
		VFIterator(Vertex * _v) { f = _v->VFp(); z = _v->VFi(); }
	public:

		Face *f;
		int z;
	};

	class VVNeighbor
	{
	public:

		size_t GetCount();
		int GetIndex(int  id); // 查找邻居的索引
		int GetNeighborID(int index); // 从0开始的索引
		int GetPrevID(int index);
		int GetNextID(int index);
		Vertex * GetNeighborVertex(int index);

		VVNeighbor() {}
		VVNeighbor(Vertex * _v);

		Pos pos;
	};
	class Topology
	{
	public:
		static void GetSurroundingFacesVF(Face * face, std::vector<Face *> & surround);//利用点面拓扑获取面片邻面
		static bool IsBoundary(Vertex * v);
		static int  EdgeCollapse(sn3DMeshData  & m, int v0, int v1); //变折叠
			/// Auxiliairy data structure for computing face face adjacency information. 
			// It identifies and edge storing two vertex pointer and a face pointer where it belong.
		class PEdge
		{
		public:

			Vertex *  v[2];		// the two Vertex pointer are ordered!
			Face *    f;		  // the face where this edge belong
			int      z;				      // index in [0..2] of the edge of the face

			PEdge() {}

			void Set(Face *  pf, const int nz)
			{
				assert(pf != 0);
				assert(nz >= 0);
				assert(nz < 3);

				v[0] = pf->V(nz);
				v[1] = pf->V((nz + 1) % 3);
				assert(v[0] != v[1]); // The face pointed by 'f' is Degenerate (two coincident vertexes)

				if (v[0] > v[1]) AXYMath::Swap(v[0], v[1]);
				f = pf;
				z = nz;
			}

			inline bool operator <  (const PEdge & pe) const
			{
				if (v[0] < pe.v[0]) return true;
				else if (v[0] > pe.v[0]) return false;
				else return v[1] < pe.v[1];
			}

			inline bool operator <=  (const PEdge & pe) const
			{
				if (v[0] < pe.v[0]) return true;
				else if (v[0] > pe.v[0]) return false;
				else return v[1] <= pe.v[1];
			}

			inline bool operator >  (const PEdge & pe) const
			{
				if (v[0] > pe.v[0]) return true;
				else if (v[0] < pe.v[0]) return false;
				else return v[1] > pe.v[1];
			}

			inline bool operator >=  (const PEdge & pe) const
			{
				if (v[0] > pe.v[0]) return true;
				else if (v[0] < pe.v[0]) return false;
				else return v[1] >= pe.v[1];
			}

			inline bool operator == (const PEdge & pe) const
			{
				return v[0] == pe.v[0] && v[1] == pe.v[1];
			}

			inline bool operator != (const PEdge & pe) const
			{
				return v[0] != pe.v[0] || v[1] != pe.v[1];
			}
		};

		static void RemoveRedundant(sn3DMeshData * m);
		static void FaceFace(sn3DMeshData &m);      // Update the Face-Face topological relation	
		static void VertexFace(sn3DMeshData & m);  // Update the Vertex-Face topological relation
		static void IdentifyBoundary(sn3DMeshData & m);
		static void IdentifyBoundary2(sn3DMeshData& m);

		static void GetOneringEdge(Vertex * v, std::vector<Edge*> & nei);
	};

	bool  IsInFace(Face * f, Vertex * v);
	bool  IsInFace(Face * f, int v);
	Face * NeighborFaceByVF(Face * f, int z, int & z1);
	void  VFDetach(Face & f, int z);
	void  VFAttach(Face &f, int z);
	void  VFDetach(Face & f, Vertex & v);
	int   ComplexSize(Face * f, int e);//计算边 e 的复杂度(即共面数)
	bool  FFCorrectness(Face * f, const int e);//检查共边拓扑关系
	void  SwapEdge(Face * f, const int z);//通过改变顶点索引翻转面的朝向
}
#endif
