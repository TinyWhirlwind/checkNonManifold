#include"MeshTopology.h"

//#include <QDebug>
#include"MeshData.h"
#include"Geometry/SpaceBox.h"

namespace sn3DCore
{
	Pos::Pos() {}
	Pos::Pos(Face * const fp, int const zp, Vertex * const vp) { f = fp; z = zp; v = vp; }
	Pos::Pos(Face * const fp, int const zp) { f = fp; z = zp; v = f->V(zp); }
	Pos::Pos(Face * const fp, Vertex * const vp)
	{
		f = fp;
		v = vp;
		if (f->V(0) == v)
			z = 2;
		else if (f->V(1) == v)
			z = 0;
		else if (f->V(2) == v)
			z = 1;
	}
	Vertex *& Pos::V() { return v; }
	Face   *& Pos::F() { return f; }
	int   & Pos::I() { return z; }
	bool Pos::operator == (Pos const & p) const
	{
		return (f == p.f && z == p.z && v == p.v);
	}

	bool Pos::operator != (Pos const & p) const
	{
		return (f != p.f || z != p.z || v != p.v);
	}

	bool Pos::operator <= (Pos const & p) const
	{
		return	(f != p.f) ? (f < p.f) : (z != p.z) ? (z < p.z) : (v <= p.v);
	}

	Pos & Pos::operator = (const Pos & h)
	{
		f = h.f; z = h.z; v = h.v; return *this;
	}

	void Pos::SetNull()
	{
		f = 0;
		v = 0;
		z = -1;
	}

	bool Pos::IsNull() const
	{
		return f == 0 || v == 0 || z < 0;
	}

	void Pos::Set(Face  * const fp, int const zp, Vertex  * const vp)
	{
		f = fp; z = zp; v = vp;
	}

	void Pos::Set(Face  * const pFace, Vertex  * const pVertex)
	{
		f = pFace;
		v = pVertex;
		if (f->V(0) == v)	   z = 2;
		else if (f->V(1) == v) z = 0;
		else if (f->V(2) == v) z = 1;
	}

	void Pos::NextF()
	{
		Face * t = f;
		f = t->FFp(z);
		z = t->FFi(z);
	}
	void Pos::NextE()
	{
		FlipE();
		FlipF();
	}
	void Pos::NextB()
	{
		do
			NextE();//沿共点边移动寻找下一个边界边
		while (!IsBorder());
		FlipV();
	}

	void Pos::FlipE()
	{
		if (f->V((z + 1) % 3) == v) z = (z + 1) % 3;
		else z = (z - 1 + 3) % 3;
	}

	void Pos::FlipF()
	{
		Face *nf = f->FFp(z);
		int nz = f->FFi(z);
		f = nf;
		z = nz;
	}

	void Pos::FlipV()
	{
		if (f->V((z + 1) % 3) == v)
			v = f->V((z + 0) % 3);
		else
			v = f->V((z + 1) % 3);
	}

	Vertex * Pos::VFlip()
	{
		if (f->V((z + 1) % 3) == v)	return f->V((z + 0) % 3);
		else			return f->V((z + 1) % 3);
	}

	const Vertex * Pos::VFlip() const
	{
		if (f->V((z + 1) % 3) == v)	return f->V((z + 0) % 3);
		else			return f->V((z + 1) % 3);
	}

	Face * Pos::FFlip()
	{
		Face *nf = f->FFp(z);
		return nf;
	}

	const Face * Pos::FFlip() const
	{
		Face *nf = f->FFp(z);
		return nf;
	}

	bool Pos::IsBorder()
	{
		return f->FFp(z) == f;
	}

	bool Pos::IsManifold()
	{
		return (f->FFp(z) == f || f == f->FFp(z)->FFp(f->FFi(z)));
	}

	int Pos::NumberOfIncidentVertices()
	{
		int  count = 0;
		bool on_border = false;
		CheckIncidentFaces(count, on_border);
		if (on_border) return (count / 2) + 1;//边界情况处理
		else	return count;
	}

	int Pos::NumberOfIncidentFaces()
	{
		int  count = 0;
		bool on_border = false;
		CheckIncidentFaces(count, on_border);
		if (on_border) return count / 2;//如遇到边界，面数减半
		else return count;
	}

	bool Pos::Coerent() const
	{
		return v == f->V(z);	// 检查v是否是边 z 的起点
	}

	void Pos::CheckIncidentFaces(int & count, bool & on_border)
	{
		Pos ht = *this;
		do
		{
			++count;
			ht.NextE();
			if (ht.IsBorder()) on_border = true;//如遇到边界置位
		} while (ht != *this);
	}

	VVNeighbor::VVNeighbor(Vertex * _v)
	{
		pos.v = _v;
		pos.f = _v->VFp();
		pos.z = _v->VFi();
		if (Topology::IsBoundary(_v))
		{
			while (pos.FFlip() != pos.f)
			{
				pos.NextE();
			}
		}
	}
	size_t VVNeighbor::GetCount()
	{
		return 0;// n;
	}

	int VVNeighbor::GetIndex(int  id)
	{
		Pos _pos;
		_pos.f = pos.f;
		_pos.z = pos.z;
		_pos.v = pos.v;

		int index = 0;
		do
		{
			if (id == _pos.f->V((_pos.z + 1) % 3)->Index())
			{
				return index;
			}
			index++;
			_pos.NextE();
		} while (_pos.f != pos.f);

		return -1;
	}

	int VVNeighbor::GetNeighborID(int index)
	{
		Pos _pos;
		_pos.f = pos.f;
		_pos.z = pos.z;
		_pos.v = pos.v;
		for (int i = 0; i < index; i++)
		{
			_pos.NextE();
		}

		if (_pos.f->V((_pos.z + 1) % 3) == _pos.v)return _pos.f->V(_pos.z)->Index();
		else return _pos.f->V((_pos.z + 1) % 3)->Index();

	}

	int VVNeighbor::GetPrevID(int index)
	{
		Pos _pos;
		_pos.f = pos.f;
		_pos.z = pos.z;
		_pos.v = pos.v;
		for (int i = 0; i < index; i++)
		{
			_pos.NextE();
		}

		_pos.FlipF();
		_pos.FlipE();

		if (_pos.f->V((_pos.z + 1) % 3) == _pos.v)return _pos.f->V(_pos.z)->Index();
		else return _pos.f->V((_pos.z + 1) % 3)->Index();
	}

	int VVNeighbor::GetNextID(int index)
	{
		Pos _pos;
		_pos.f = pos.f;
		_pos.z = pos.z;
		_pos.v = pos.v;
		for (int i = 0; i < index + 1; i++)
		{
			_pos.NextE();
		}

		if (_pos.f->V((_pos.z + 1) % 3) == _pos.v)return _pos.f->V(_pos.z)->Index();
		else return _pos.f->V((_pos.z + 1) % 3)->Index();
	}

	Vertex * VVNeighbor::GetNeighborVertex(int index)
	{
		Pos _pos;
		_pos.f = pos.f;
		_pos.z = pos.z;
		_pos.v = pos.v;
		for (int i = 0; i < index; i++)
		{
			_pos.NextE();
		}

		return _pos.f->V((_pos.z + 1) % 3);
	}

	void Topology::GetSurroundingFacesVF(Face * face, std::vector<Face *> & surround)
	{
		Vertex * vert;
		Face * f, *f1;
		int z, z1;
		for (int i = 0; i < 3; i++)
		{
			//清除访问标志
			vert = face->V(i);
			f = vert->VFp();
			z = vert->VFi();
			while (f != NULL)
			{
				f->ClearV();
				f1 = f->VFp(z);
				z1 = f->VFi(z);
				f = f1;
				z = z1;
			}
		}
		face->SetV();

		for (int i = 0; i < 3; i++) {
			vert = face->V(i);
			f = vert->VFp();
			z = vert->VFi();
			while (f != NULL)
			{
				if ((!f->IsV()))
				{
					f->SetV();
					surround.push_back(f);
				}
				f1 = f->VFp(z);
				z1 = f->VFi(z);
				f = f1;
				z = z1;
			}
		}
	}

	void Topology::FaceFace(sn3DMeshData &m)
	{
		if (m.fn == 0) return;
		int end;

		int num = (int)m.fn;

		PEdge * edgeArray = new PEdge[num * 3];//分配边数组空间,每个面三个边。独立看待

		bool * edgeFlag = new bool[num * 3];//标识边是否已经构建拓扑关系

		memset(edgeFlag, 0, num * 3 * sizeof(bool));

		// fill Edge vector
		for (int fi = 0; fi < num; fi++)
		{
			if (!m.F(fi)->IsD())
			{
				for (int j = 0; j < 3; ++j)
				{
					edgeArray[fi * 3 + j].Set(m.F(fi), j);
				}
			}
			else
			{
				edgeFlag[fi * 3] = edgeFlag[fi * 3 + 1] = edgeFlag[fi * 3 + 2] = true;
			}

		}
		end = num * 3;

		for (int i = 0; i < end; i++)//对每个边通过共点面链表信息构建共边关系，即共边面一定是共顶点的
		{
			Vertex * v, *v1, *v2;

			Face * face, *face1;
			int z, z1;

			Face * first, *cur, *next;
			int firstz, curz, nextz;

			if (!edgeFlag[i])   //如果还没有构建拓扑关系，进行处理
			{
				first = cur = next = NULL;//first记录第一个包含该边的面
				firstz = curz = nextz = 0;

				v = edgeArray[i].v[0];//每个边起点

				face = v->VFp();//共点面链表头指针
				z = v->VFi();

				while (face != NULL)
				{
					for (int j = 0; j < 3; j++)
					{
						v1 = edgeArray[face->Index() * 3 + j].v[0];//面的每个边的两个顶点
						v2 = edgeArray[face->Index() * 3 + j].v[1];
						if (((v1 == edgeArray[i].v[0]) && (v2 == edgeArray[i].v[1]))
							|| ((v1 == edgeArray[i].v[1]) && (v2 == edgeArray[i].v[0])))
						{
							edgeFlag[face->Index() * 3 + j] = true;//标识已经建立拓扑关系

							if (first == NULL)
							{
								first = cur = next = face;
								firstz = curz = nextz = j;
							}
							else
							{
								next = face;
								nextz = j;
								cur->SetFFp(curz, next);
								cur->FFi(curz) = nextz;
								cur = next;
								curz = nextz;
							}
						}
					}
					face1 = face->VFp(z);
					z1 = face->VFi(z);
					face = face1;
					z = z1;
				}
				cur->SetFFp(curz, first);
				cur->FFi(curz) = firstz;

			}
		}
		delete[] edgeArray;
		delete[] edgeFlag;

	}

	void Topology::VertexFace(sn3DMeshData & m)
	{
		int end;
		m.UpdateVertexTopology();
		m.UpdateFaceTopology();

		end = (int)m.m_vertices.size();
		for (int i = 0; i < end; i++)
		{
			m.V(i)->SetVFp(-1);
			m.V(i)->VFi() = 0;
		}

		end = (int)m.m_faces.size();
		for (int i = 0; i < end; i++)
		{
			if (!m.F(i)->IsD())
			{
				for (int j = 0; j < 3; ++j)
				{
					m.F(i)->SetVFp(j, m.F(i)->V(j)->VFp());
					m.F(i)->VFi(j) = m.F(i)->V(j)->VFi();
					m.F(i)->V(j)->SetVFp(m.F(i)->Index());
					m.F(i)->V(j)->VFi() = j;
				}
			}
		}
	}

	void Topology::IdentifyBoundary2(sn3DMeshData& m)
	{
		int vn = m.vn;
		for (int i = 0; i < vn; i++) {
			m.V(i)->ClearB();
		}

		for (int i = 0; i < vn; i++) {
			VFIterator vfi(m.V(i));
			std::map<Vertex*, int> edgeCount;

			for (; !vfi.End(); ++vfi) {
				Vertex* v1 = vfi.f->V((vfi.z + 1) % 3);
				Vertex* v2 = vfi.f->V((vfi.z + 2) % 3);

				edgeCount[v1]++;
				edgeCount[v2]++;
			}

			for (const auto& edge : edgeCount) {
				if (edge.second == 1) {  // 只被遍历到一次的边表示它是一个边界边
					m.V(i)->SetB();
					break;
				}
			}
		}
	}

	void Topology::IdentifyBoundary(sn3DMeshData & m)
	{
		int vn = m.vn;
		for (int i = 0; i < vn; i++)m.V(i)->ClearB();
		for (int i = 0; i < vn; i++)
		{
			VFIterator vfi(m.V(i));
			for (; !vfi.End(); ++vfi)
			{
				vfi.f->V((vfi.z + 1) % 3)->ClearV();
				vfi.f->V((vfi.z + 2) % 3)->ClearV();
			}
			vfi.f = m.V(i)->VFp();
			vfi.z = m.V(i)->VFi();
			for (; !vfi.End(); ++vfi)
			{
				if (!vfi.f->V((vfi.z + 1) % 3)->IsV())vfi.f->V((vfi.z + 1) % 3)->SetV();
				else vfi.f->V((vfi.z + 1) % 3)->ClearV();
				if (!vfi.f->V((vfi.z + 2) % 3)->IsV())vfi.f->V((vfi.z + 2) % 3)->SetV();
				else vfi.f->V((vfi.z + 2) % 3)->ClearV();
			}
			vfi.f = m.V(i)->VFp();
			vfi.z = m.V(i)->VFi();
			for (; !vfi.End(); ++vfi)
			{
				if (vfi.f->V((vfi.z + 1) % 3)->IsV())
				{
					m.V(i)->SetB();
					break;
				}
				if (vfi.f->V((vfi.z + 2) % 3)->IsV())
				{
					m.V(i)->SetB();
					break;
				}
			}
		}
	}

	void Topology::GetOneringEdge(Vertex * v, std::vector<Edge*> & nei)
	{
		nei.clear();
		VFIterator vfi(v);
		sn3DMeshData * mesh = (sn3DMeshData *)v->m_data;
		for (; !vfi.End(); ++vfi)
		{
			vfi.f->V((vfi.z + 1) % 3)->ClearV();
			vfi.f->V((vfi.z + 2) % 3)->ClearV();
		}

		vfi.f = v->VFp();
		vfi.z = v->VFi();
		for (; !vfi.End(); ++vfi)
		{
			Vertex * vv = vfi.f->V((vfi.z + 1) % 3);
			if (!vv->IsV())
			{
				vv->SetV();
				nei.push_back(mesh->E(vfi.f->E(vfi.z)));
			}
			vv = vfi.f->V((vfi.z + 2) % 3);
			if (!vv->IsV())
			{
				vv->SetV();
				nei.push_back(mesh->E(vfi.f->E((vfi.z + 2) % 3)));
			}
		}
	}

	bool Topology::IsBoundary(Vertex * v)
	{
		VFIterator vfi(v);
		for (; !vfi.End(); ++vfi)
		{
			vfi.f->V((vfi.z + 1) % 3)->ClearV();
			vfi.f->V((vfi.z + 2) % 3)->ClearV();
		}

		vfi.f = v->VFp();
		vfi.z = v->VFi();
		for (; !vfi.End(); ++vfi)
		{
			if (!vfi.f->V((vfi.z + 1) % 3)->IsV())vfi.f->V((vfi.z + 1) % 3)->SetV();
			else vfi.f->V((vfi.z + 1) % 3)->ClearV();
			if (!vfi.f->V((vfi.z + 2) % 3)->IsV())vfi.f->V((vfi.z + 2) % 3)->SetV();
			else vfi.f->V((vfi.z + 2) % 3)->ClearV();
		}

		vfi.f = v->VFp();
		vfi.z = v->VFi();
		for (; !vfi.End(); ++vfi)
		{
			if (vfi.f->V((vfi.z + 1) % 3)->IsV())
			{
				return true;
			}
			if (vfi.f->V((vfi.z + 2) % 3)->IsV())
			{
				return true;
			}
		}

		return false;
	}

	void Topology::RemoveRedundant(sn3DMeshData *m)
	{
		std::vector<Point3f> vertexBase;//读入的顶点数据
		std::vector<Color4f> vertexColor;
		std::vector<bool> vertexDelete;//标识顶点是否已经被删除
		std::vector<int> vertexMap;// 顶点映射

		////////   将STL模型点、面信息记录下来  ////////////
		int facenum = (int)m->m_faces.size();
		int vertexnum = (int)m->m_vertices.size();;//顶点总数

		vertexBase.resize(vertexnum);//分配顶点空间
		vertexColor.resize(vertexnum);
		vertexMap.resize(vertexnum);//分配面空间(顶点索引)
		vertexDelete.resize(vertexnum);//是否被删除的标志

		/////  读入顶点几何数据  ///////////////////
		for (int k = 0; k < vertexnum; k++)
		{
			vertexBase[k] = m->m_vertices[k].m_coord;
			vertexColor[k] = m->m_vertices[k].m_color;
		}


		/////////初始化空间剖分结构
		CSpaceBox hierarchy(70);	//空间查找结构
		hierarchy.Initialize(vertexBase);

		/////////////////////   去除冗余顶点   ///////////////////////////////
		int vertexCount = vertexnum;//去冗余后的顶点个数

		for (int i = 0; i < vertexnum; i++)
		{
			if (!vertexDelete[i])//判断该点是否被删除
			{
				//计算单元坐标
				int x = (int)((vertexBase[i].X() - hierarchy.m_MinCorner.X())* hierarchy.m_Delta.X());
				if (x == hierarchy.m_DivisionNum) x = hierarchy.m_DivisionNum - 1;
				int y = (int)((vertexBase[i].Y() - hierarchy.m_MinCorner.Y())* hierarchy.m_Delta.Y());
				if (y == hierarchy.m_DivisionNum) y = hierarchy.m_DivisionNum - 1;
				int z = (int)((vertexBase[i].Z() - hierarchy.m_MinCorner.Z()) * hierarchy.m_Delta.Z());
				if (z == hierarchy.m_DivisionNum) z = hierarchy.m_DivisionNum - 1;

				int entrySize = hierarchy.GetEntrySize(x, y, z);

				for (int j = 0; j < entrySize; j++)
				{
					int index = hierarchy.GetEntryItem(x, y, z, j);

					if (index == i)continue;//避免同一顶点比较

					if (vertexDelete[index])continue;//避免跟已经删除的顶点比较

					if (vertexBase[i] == vertexBase[index])
					{
						vertexDelete[index] = true;//删除该冗余顶点
						vertexMap[index] = i;//更新面的顶点索引信息	
						vertexCount--;
					}
				}
			}

		}

		hierarchy.Clear();//释放空间

		///////////////   分配点面空间,更新模型数据   ////////////////////////////
		//去冗余后记录顶点索引的变化信息
		int count = 0;
		for (int i = 0; i < vertexnum; i++)
		{
			if (!vertexDelete[i])
			{
				vertexMap[i] = count;
				count++;
			}
		}
		for (int i = 0; i < vertexnum; i++)//更新已经被删除的顶点索引
		{
			if (vertexDelete[i])
			{
				vertexMap[i] = vertexMap[vertexMap[i]];
			}
		}


		//更新面的顶点索引信息
		for (int i = 0, j = 0; i < facenum; i++)
		{
			int v0 = m->F(i)->V(0)->Index();
			int v1 = m->F(i)->V(1)->Index();
			int v2 = m->F(i)->V(2)->Index();
		
			m->m_faces[i].SetV(0, vertexMap[v0]);
			m->m_faces[i].SetV(1, vertexMap[v1]);
			m->m_faces[i].SetV(2, vertexMap[v2]);
		}

		m->m_vertices.clear();
		m->vn = 0;

		for (int i = 0; i < vertexnum; i++)
		{
			if (!vertexDelete[i])
			{
				m->AddVertex(vertexBase[i], vertexColor[i]);
			}
		}
		
	}

	int Topology::EdgeCollapse(sn3DMeshData  & m, int vi0, int vi1)
	{
		// 边折叠计算
		std::vector<VFIterator> av0; // 相邻面
		std::vector<VFIterator> av1;
		std::vector<VFIterator> av01; // 公共相邻面
		Vertex * v0 = m.V(vi0);
		Vertex * v1 = m.V(vi1);
		VFIterator x;

		for (x.f = v0->VFp(), x.z = v0->VFi(); x.f != 0; ++x)
		{
			int zv1 = -1;

			for (int j = 0; j < 3; ++j)
				if (x.f->V(j) == &*v1)
				{
					zv1 = j;
					break;
				}

			if (zv1 == -1) 	av0.push_back(x); // only adjacent to v0
			else    		av01.push_back(x);
		}

		for (x.f = v1->VFp(), x.z = v1->VFi(); x.f != 0; ++x)
		{
			int zv0 = -1;
			for (int j = 0; j < 3; ++j)
				if (x.f->V(j) == &*v0)
				{
					zv0 = j;
					break;
				}

			if (zv0 == -1)	av1.push_back(x); // only adjacent to v1
		}

		std::vector<VFIterator>::iterator i;

		int n_face_del = 0;

		for (i = av01.begin(); i != av01.end(); ++i)
		{
			Face  & f = *((*i).f);
			VFDetach(f, ((*i).z + 1) % 3);
			VFDetach(f, ((*i).z + 2) % 3);
			f.SetD();
		}

		//set CVertex Face topology
		for (i = av0.begin(); i != av0.end(); ++i)
		{
			(*i).f->SetV((*i).z, vi1);
			(*i).f->SetVFp((*i).z, (*i).f->V((*i).z)->VFp());
			(*i).f->VFi((*i).z) = (*i).f->V((*i).z)->VFi();
			(*i).f->V((*i).z)->SetVFp((*i).f->Index());
			(*i).f->V((*i).z)->VFi() = (*i).z;
		}

		v0->SetD(); // 删除
		return n_face_del;
	}

	void VFDetach(Face & f, int z)
	{
		if (f.V(z)->VFp() == &f)  //if it is the first face detach from the begin
		{
			int fz = f.V(z)->VFi();
			if (f.VFp(fz) == 0)
				f.V(z)->SetVFp(-1);
			else
				f.V(z)->SetVFp(f.VFp(fz)->Index());
			f.V(z)->VFi() = f.VFi(fz);
		}
		else  // scan the list of faces in order to find the current face f to be detached
		{
			VFIterator x(f.V(z)->VFp(), f.V(z)->VFi());
			VFIterator y;
			for (;;)
			{
				y = x;
				++x;
				assert(x.f != 0);
				if (x.f == &f) // found!
				{
					y.f->SetVFp(y.z, f.VFp(z));
					y.f->VFi(y.z) = f.VFi(z);
					break;
				}
			}
		}
	}

	void VFDetach(Face & f, Vertex & v)
	{
		if (v.VFp() == &f)  //if it is the first face detach from the begin
		{
			int fz = v.VFi();
			if (f.VFp(fz) == 0)
				v.SetVFp(-1);
			else
				v.SetVFp(f.VFp(fz)->Index());
			v.VFi() = f.VFi(fz);
		}
		else  // scan the list of faces in order to find the current face f to be detached
		{
			VFIterator x(v.VFp(), v.VFi());
			VFIterator y;
			for (;;)
			{
				y = x;
				++x;
				assert(x.f != 0);
				if (x.f == &f) // found!
				{
					y.f->SetVFp(y.z, f.VFp(x.z));
					y.f->VFi(y.z) = f.VFi(x.z);
					break;
				}
			}
		}
	}

	void VFAttach(Face &f, int z)
	{
		f.SetVFp(z, f.V(z)->VFp());
		f.VFi(z) = f.V(z)->VFi();
		f.V(z)->SetVFp(f.Index());
		f.V(z)->VFi() = z;
	}

	int ComplexSize(Face * f, int e)
	{
		Pos pos(f, e);
		int cnt = 0;
		do
		{
			pos.NextF();//移动到下一个共边面
			++cnt;
		} while (pos.f != f);//直到回到面f为止
		return cnt;
	}

	bool FFCorrectness(Face * f, const int e)
	{
		if (f->FFp(e) == 0) return false;   // 尚未初始化拓扑信息

		if (f->FFp(e) == f) // 如果是边界
		{
			if (f->FFi(e) == e) return true;
			else return false;
		}

		if (f->FFp(e)->FFp(f->FFi(e)) == f) //如果是流形
		{
			if (f->FFp(e)->FFi(f->FFi(e)) == e) return true;
			else return false;
		}

		Pos curFace(f, e); //如果是非流形

		do
		{
			if (curFace.IsManifold()) return false; //如果共边链表中出现流行边面说明拓扑信息不正确
			if (curFace.IsBorder()) return false;
			curFace.NextF();
		} while (curFace.f != f);
		return true;
	}

	void SwapEdge(Face * f, const int z)
	{
		Vertex * swapTemp = f->V(z);//交换边z的两顶点
		int z1 = (z + 1) % 3;//更新邻面的邻接拓扑信息
		int z2 = (z + 2) % 3;
		Face *g1p = f->FFp(z1);//原除z边邻面外的两个邻面
		Face *g2p = f->FFp(z2);
		int g1i = f->FFi(z1);
		int g2i = f->FFi(z2);

		if (g1p != f) //如果非边界
		{
			g1p->FFi(g1i) = z2;
			f->FFi(z2) = g1i;
		}
		else
		{
			f->FFi(z2) = z2;
		}

		if (g2p != f)//如果非边界
		{
			g2p->FFi(g2i) = z1;
			f->FFi(z1) = g2i;
		}
		else
		{
			f->FFi(z1) = z1;
		}

		f->SetFFp(z1, g2p);
		f->SetFFp(z2, g1p);
	}

	Face *NeighborFaceByVF(Face * f, int z, int & z1)
	{
		Vertex * v0 = f->V(z);
		Vertex * v1 = f->V((z + 1) % 3);
		VFIterator vfi(v1);

		Face * neighbor = 0;

		for (; !vfi.End(); ++vfi)
		{
			Face * ff = vfi.f;
			if (ff == f) continue;
			if (ff->V((vfi.z + 1) % 3) == v0)//寻找共边邻面
			{
				neighbor = ff;
				z1 = vfi.z;
				break;
			}
		}

		return neighbor;
	}

	bool IsInFace(Face * f, Vertex * v)
	{
		if (f->V(0) == v) return true;
		if (f->V(1) == v) return true;
		if (f->V(2) == v) return true;
		return false;
	}

	bool IsInFace(Face * f, int v)
	{
		if (f->V(0)->Index() == v) return true;
		if (f->V(1)->Index() == v) return true;
		if (f->V(2)->Index() == v) return true;
		return false;

	}
}