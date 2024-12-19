#include "checkMesh.h"
#include "GeoData/MeshData.h"
#include "GeoData/MeshTopology.h"
#include <iostream>
#include <queue>
#include <set>

typedef std::pair<int, int> MeshEdge;
class MeshCheck::PImpl
{
public:
	PImpl(boost::shared_ptr<sn3DCore::sn3DMeshData> mesh)
	{
		m_mesh_ = mesh;
		isCheck_ = false;
		for (int i = 0; i < m_mesh_->n_vertices(); i++)
		{
			m_mesh_->V(i)->ClearS();
		}
		nonManifoldPts_.clear();
		nonManifoldEdges_.clear();
	}
	PImpl()
	{
		m_mesh_ = nullptr;
		isCheck_ = false;
		nonManifoldPts_.clear();
		nonManifoldEdges_.clear();
	}
	~PImpl() {}

public:
	bool checkNonManifoldEdge();
	bool checkNonManifoldPoint();
	void removeDegenerateFaces();
	void deleteNonManifold();
	void repairNonManifoldPts();
	void repairNonManifoldEdges();
	std::vector<std::vector<int>> getAllConnectedVertices();
	MeshEdge EdgeId(MeshEdge half_edge);
	int GetVertexRingVertices(sn3DCore::sn3DMeshData* mesh, sn3DCore::Vertex* v, std::vector<Vertex*>& ringVertics);
public:
	boost::shared_ptr<sn3DCore::sn3DMeshData> m_mesh_;
	std::vector<int> nonManifoldPts_;
	std::map<int, std::vector<MeshEdge>> nonManifoldEdges_;
	bool isCheck_;
};

MeshCheck::MeshCheck(boost::shared_ptr<sn3DCore::sn3DMeshData> mesh)
{
	impl_.reset(new PImpl(mesh));
}

MeshCheck::MeshCheck()
{
	impl_.reset(new PImpl());
}

MeshCheck::~MeshCheck() = default;

void MeshCheck::setMesh(boost::shared_ptr<sn3DCore::sn3DMeshData> mesh)
{
	impl_.reset(new PImpl(mesh));
}

boost::shared_ptr<sn3DCore::sn3DMeshData> MeshCheck::getMesh()
{
	return impl_->m_mesh_;
}

bool MeshCheck::checkNonManifold()
{
	bool result = false;
	impl_->isCheck_ = true;
	if (impl_->checkNonManifoldEdge())
	{
		result = true;
		std::cout << impl_->nonManifoldEdges_.size() << " non manifold edges" << std::endl;
	}
	if (impl_->checkNonManifoldPoint())
	{
		result = true;
		std::cout<< impl_->nonManifoldPts_ .size()<<" non manifold points" << std::endl;
	}
	return result;
}

void MeshCheck::removeNonManifold()
{
	if (!impl_->isCheck_)
	{
		impl_->checkNonManifoldPoint();
		impl_->checkNonManifoldEdge();
	}
	impl_->deleteNonManifold();
}

void MeshCheck::repairNonManifold()
{
	if (impl_->checkNonManifoldEdge())
	{
		impl_->repairNonManifoldEdges();
	}
	if (impl_->checkNonManifoldPoint())
	{
		impl_->repairNonManifoldPts();
	}
}

void MeshCheck::removeDegenerateFaces()
{
	impl_->removeDegenerateFaces();
}

boost::shared_ptr<sn3DCore::sn3DMeshData> MeshCheck::getMaxConnectedComponent()
{
	boost::shared_ptr<sn3DCore::sn3DMeshData> maxMesh(new sn3DCore::sn3DMeshData);
	std::vector<std::vector<int>> verVertexList = impl_->getAllConnectedVertices();
	if (verVertexList.size() == 0)
		return nullptr;
	int maxIndex = -1;
	int MAX_SIZE = INT_MIN;
	for (int i = 0; i < verVertexList.size(); i++)
	{
		int curSize = verVertexList[i].size();
		if (curSize > MAX_SIZE)
		{
			maxIndex = i;
			MAX_SIZE = curSize;
		}
	}
	for (int i = 0; i < impl_->m_mesh_->n_vertices(); i++)
	{
		impl_->m_mesh_->V(i)->ClearV();
	}
	std::vector<Face*> selectFaces;
	std::map<int, int> indexMap;
	std::set<int> curVerSet(verVertexList[maxIndex].begin(), verVertexList[maxIndex].end());
	int newIndex = -1;
	for (int i = 0; i < impl_->m_mesh_->n_faces(); i++)
	{
		Face* f = impl_->m_mesh_->F(i);
		bool allVerticesInSet = true;

		for (int j = 0; j < 3; j++)
		{
			if (curVerSet.find(f->m_v[j]) == curVerSet.end())
			{
				allVerticesInSet = false;
				break;
			}
		}
		if (allVerticesInSet)
		{
			for (int j = 0; j < 3; j++)
			{
				Vertex* curV = f->V(j);
				if (curV->IsV())
					continue;
				curV->SetV();
				maxMesh->AddVertex(curV->P());
				newIndex++;
				indexMap[f->m_v[j]] = newIndex;
			}
			selectFaces.push_back(f);
		}
	}

	for (auto itor : selectFaces)
	{
		int old_v0 = itor->m_v[0];
		int old_v1 = itor->m_v[1];
		int old_v2 = itor->m_v[2];
		maxMesh->AddFace(indexMap[old_v0], indexMap[old_v1], indexMap[old_v2]);
	}
	return maxMesh;
}

std::vector<boost::shared_ptr<sn3DCore::sn3DMeshData>> MeshCheck::getAllConnectedComponent()
{
	std::vector<boost::shared_ptr<sn3DCore::sn3DMeshData>> resultMeshlist;
	std::vector<std::vector<int>> verVertexList = impl_->getAllConnectedVertices();
	if (verVertexList.size() == 0)
		return {};

	for (int i = 0; i < verVertexList.size(); i++)
	{
		for (int j = 0; j < impl_->m_mesh_->n_vertices(); j++)
		{
			impl_->m_mesh_->V(j)->ClearV();
		}
		boost::shared_ptr<sn3DCore::sn3DMeshData> resultMesh(new sn3DCore::sn3DMeshData());
		std::vector<Face*> selectFaces;
		std::map<int, int> indexMap;
		std::set<int> curVerSet(verVertexList[i].begin(), verVertexList[i].end());
		int newIndex = -1;
		for (int j = 0; j < impl_->m_mesh_->n_faces(); j++)
		{
			Face* f = impl_->m_mesh_->F(j);
			bool allVerticesInSet = true;

			for (int k = 0; k < 3; k++)
			{
				if (curVerSet.find(f->m_v[k]) == curVerSet.end())
				{
					allVerticesInSet = false;
					break;
				}
			}
			if (allVerticesInSet)
			{
				for (int k = 0; k < 3; k++)
				{
					Vertex* curV = f->V(k);
					if (curV->IsV())
						continue;
					curV->SetV();
					resultMesh->AddVertex(curV->P());
					newIndex++;
					indexMap[f->m_v[k]] = newIndex;
				}
				selectFaces.push_back(f);
			}
		}

		for (auto itor : selectFaces)
		{
			int old_v0 = itor->m_v[0];
			int old_v1 = itor->m_v[1];
			int old_v2 = itor->m_v[2];
			resultMesh->AddFace(indexMap[old_v0], indexMap[old_v1], indexMap[old_v2]);
		}
		//sn3DIOManager::instance().Write(QString("D:/data/MeshError/connectMesh%1.obj").arg(i), resultMesh.get());
		resultMeshlist.push_back(resultMesh);
	}
	return resultMeshlist;
}

MeshEdge MeshCheck::PImpl::EdgeId(MeshEdge half_edge)
{
	int v0 = half_edge.first;
	int v1 = half_edge.second;
	if (v0 > v1)
	{
		return std::make_pair(v1, v0);
	}
	else
	{
		return std::make_pair(v0, v1);
	}
}

bool MeshCheck::PImpl::checkNonManifoldPoint()
{
	if (m_mesh_->n_vertices() <= 0)
		return false;
	nonManifoldPts_.clear();

	typedef std::vector<std::vector<Face*>> VFList;
	std::map<int, VFList> allNonManifoldGroup;

	bool isNonManifold = false;
	for (auto& itor : m_mesh_->m_vertices)
	{
		VFIterator vfi(&itor);
		std::map<MeshEdge, Face*> allHalfEdge;
		std::set<Face*> allRingFace;
		for (; !vfi.End(); ++vfi)
		{
			Vertex* v0 = vfi.f->V(vfi.z);
			Vertex* v1 = vfi.f->V((vfi.z + 1) % 3);
			Vertex* v2 = vfi.f->V((vfi.z + 2) % 3);
			allRingFace.insert(vfi.f);
			int from_vv0 = v0->m_index;
			int to_vv0 = v1->m_index;
			int from_vv1 = v2->m_index;
			int to_vv1 = v0->m_index;
			MeshEdge half_edge0 = std::make_pair(from_vv0, to_vv0);
			MeshEdge half_edge1 = std::make_pair(from_vv1, to_vv1);
			allHalfEdge[half_edge0] = vfi.f;
			allHalfEdge[half_edge1] = vfi.f;
		}

		int ringNum = 0;
		std::set<Face*> visitedFace;
		VFList allNonMainSet;//记录环路面片集合
		for (auto it : allHalfEdge)
		{
			Face* f = it.second;
			if (visitedFace.find(f) != visitedFace.end())
				continue;
			std::vector<Face*> nonMainSet;//记录每个环路的点集
			std::queue<Face*> faceList;
			faceList.push(f);
			visitedFace.insert(f);
			nonMainSet.push_back(f);
			while (!faceList.empty())
			{
				f = faceList.front();
				faceList.pop();
				for (int i = 0; i < 3; i++)
				{
					Face* neighbourFace = f->FFp(i);
					if (neighbourFace != NULL
						&& visitedFace.find(neighbourFace) == visitedFace.end()
						&& allRingFace.find(neighbourFace) != allRingFace.end())
					{
						faceList.push(neighbourFace);
						visitedFace.insert(neighbourFace);
						nonMainSet.push_back(neighbourFace);
					}
				}
			}
			ringNum++;
			if (ringNum > 1)
			{
				nonManifoldPts_.push_back(itor.m_index);
				isNonManifold = true;
				break;
			}
		}
	}
	return isNonManifold;
}

bool MeshCheck::PImpl::checkNonManifoldEdge()
{
	if (m_mesh_->n_vertices() <= 0)
		return false;

	nonManifoldEdges_.clear();
	//删除退化面片
	removeDegenerateFaces();
	//非流行面
	bool isNonManifold = false;
	std::map<MeshEdge, int> ringF;
	std::map<MeshEdge, int> edgeFace;

	for (auto iter : m_mesh_->m_faces)
	{
		MeshEdge edge0 = std::make_pair(iter.m_v[0], iter.m_v[1]);
		MeshEdge edge1 = std::make_pair(iter.m_v[1], iter.m_v[2]);
		MeshEdge edge2 = std::make_pair(iter.m_v[2], iter.m_v[0]);
		edgeFace[edge0] = iter.m_index;
		edgeFace[edge1] = iter.m_index;
		edgeFace[edge2] = iter.m_index;
		ringF[EdgeId(edge0)]++;
		ringF[EdgeId(edge1)]++;
		ringF[EdgeId(edge2)]++;
	}

	for (auto iter : ringF)
	{
		if (iter.second > 2)
		{
			MeshEdge edgeCW = iter.first;
			MeshEdge edgeCCW = std::make_pair(iter.first.first, iter.first.second);
			int faceId0 = edgeFace[edgeCW];
			int faceId1 = edgeFace[edgeCCW];
			nonManifoldEdges_[faceId0].push_back(edgeCW);
			nonManifoldEdges_[faceId1].push_back(edgeCCW);
			isNonManifold = true;
		}
	}
	return isNonManifold;
}

void MeshCheck::PImpl::removeDegenerateFaces()
{
	bool process = false;

	int fn = m_mesh_->fn;

	for (int i = 0; i < fn; i++)
	{
		Face* f = m_mesh_->F(i);
		if (f->V(0) == f->V(1)) // 重复
		{
			f->SetD();
			process = true;
		}
		if (f->V(0) == f->V(2))
		{
			f->SetD();
			process = true;
		}
		if (f->V(2) == f->V(1))
		{
			f->SetD();
			process = true;
		}
	}

	if (process)
		m_mesh_->Compact();
	m_mesh_->DirtyTopology();
}

void MeshCheck::PImpl::deleteNonManifold()
{
	if (m_mesh_->n_vertices() <= 0)
		return;

	for (auto itor : nonManifoldPts_)
	{
		m_mesh_->V(itor)->SetD();
	}
	for (auto itor : nonManifoldEdges_)
	{
		for (auto it : itor.second)
		{
			int v0 = it.first;
			int v1 = it.second;
			m_mesh_->V(v0)->SetD();
			m_mesh_->V(v1)->SetD();
		}
	}
	
	for(int i =0;i<m_mesh_->n_faces();i++)
	{
		Face* f = m_mesh_->F(i);
		if (f->V(0)->IsD() || f->V(1)->IsD() || f->V(2)->IsD())
		{
			f->SetD();
		}
	}
	m_mesh_->DirtyTopology();
}

void MeshCheck::PImpl::repairNonManifoldPts()
{
	if (m_mesh_->n_vertices() <= 0)
		return;
	for (int i = 0; i < m_mesh_->n_faces(); i++)
	{
		m_mesh_->F(i)->ClearV();
	}
	for (int i = 0; i < m_mesh_->n_faces(); i++)
	{
		m_mesh_->F(i)->ClearS();
	}
	bool isNonManifold = false;
	typedef std::vector<std::vector<Face*>> VFList;
	std::map<int, VFList> allNonManifoldGroup;
	for (auto& itor : nonManifoldPts_)
	{
		Vertex* curV = m_mesh_->V(itor);
		VFIterator vfi(curV);
		std::map<MeshEdge, Face*> allHalfEdge;
		std::set<Face*> allRingFace;
		for (; !vfi.End(); ++vfi)
		{
			Vertex* v0 = vfi.f->V(vfi.z);
			Vertex* v1 = vfi.f->V((vfi.z + 1) % 3);
			Vertex* v2 = vfi.f->V((vfi.z + 2) % 3);
			allRingFace.insert(vfi.f);
			int from_vv0 = v0->m_index;
			int to_vv0 = v1->m_index;
			int from_vv1 = v2->m_index;
			int to_vv1 = v0->m_index;
			MeshEdge half_edge0 = std::make_pair(from_vv0, to_vv0);
			MeshEdge half_edge1 = std::make_pair(from_vv1, to_vv1);
			allHalfEdge[half_edge0] = vfi.f;
			allHalfEdge[half_edge1] = vfi.f;
		}
		int ringNum = 0;
		std::set<Face*> visitedFace;
		VFList allNonMainSet;//记录环路面片集合
		for (auto it : allHalfEdge)
		{
			Face* f = it.second;
			if (visitedFace.find(f) != visitedFace.end())
				continue;
			std::vector<Face*> nonMainSet;//记录每个环路的点集
			std::queue<Face*> faceList;
			faceList.push(f);
			visitedFace.insert(f);
			nonMainSet.push_back(f);
			while (!faceList.empty())
			{
				f = faceList.front();
				faceList.pop();
				for (int i = 0; i < 3; i++)
				{
					Face* neighbourFace = f->FFp(i);
					if (neighbourFace != NULL
						&& visitedFace.find(neighbourFace) == visitedFace.end()
						&& allRingFace.find(neighbourFace) != allRingFace.end())
					{
						faceList.push(neighbourFace);
						visitedFace.insert(neighbourFace);
						nonMainSet.push_back(neighbourFace);
					}
				}
			}
			allNonMainSet.push_back(nonMainSet);//记录当前点的环路面片集合
		}
		allNonManifoldGroup[itor] = allNonMainSet;
		for (int i = 0; i < allNonMainSet.size(); i++)
		{
			for (int j = 0; j < allNonMainSet[i].size(); j++)
			{
				allNonMainSet[i][j]->SetV();
			}
		}
	}

	//点裁分
	boost::shared_ptr<sn3DMeshData> repairMesh(new sn3DMeshData);
	if (allNonManifoldGroup.size() == 0)
		return;
	for (auto itor : m_mesh_->m_vertices)
	{
		repairMesh->AddVertex(itor.P());
	}
	for (auto itor : m_mesh_->m_faces)
	{
		if (itor.IsV())
			continue;
		repairMesh->AddFace(itor.m_v[0], itor.m_v[1], itor.m_v[2]);
	}

	for (auto itor : allNonManifoldGroup)
	{
		VFList curGroup = itor.second;
		int group_size = curGroup.size();

		//将面片集合分为遍历过和没遍历过
		std::map<int, std::vector<Face*>> no_visitedGroup;
		for (int m = 0; m < curGroup.size(); m++)
		{
			bool no_visited = true;
			for (int n = 0; n < curGroup[m].size(); n++)
			{
				if (curGroup[m][n]->IsS())
				{
					no_visited = false;
					break;
				}
			}
			if (no_visited)
			{
				no_visitedGroup[m] = curGroup[m];
			}
		}
		if (no_visitedGroup.size() == curGroup.size())//都没有遍历过
		{
			auto start_group = no_visitedGroup.begin();

			for (int n = 0; n < start_group->second.size(); n++)
			{
				int v0 = start_group->second[n]->m_v[0];
				int v1 = start_group->second[n]->m_v[1];
				int v2 = start_group->second[n]->m_v[2];
				repairMesh->AddFace(v0, v1, v2);
				start_group->second[n]->SetS();
			}
			start_group++;
			for (; start_group != no_visitedGroup.end(); start_group++)
			{
				Point3f addOriginPt = repairMesh->V(itor.first)->P();
				repairMesh->AddVertex(addOriginPt);//拆分点
				int addIndex = repairMesh->n_vertices() - 1;
				for (int n = 0; n < start_group->second.size(); n++)
				{
					int originIndex = -1;
					for (int i = 0; i < 3; i++)
					{
						int v0 = start_group->second[n]->m_v[i];
						if (v0 == itor.first)
						{
							originIndex = i;
							break;
						}
					}
					int v1 = start_group->second[n]->m_v[(originIndex + 1) % 3];
					int v2 = start_group->second[n]->m_v[(originIndex + 2) % 3];
					repairMesh->AddFace(addIndex, v1, v2);
					start_group->second[n]->SetS();
				}
			}
		}
		else
		{
			for (auto vg : no_visitedGroup)
			{
				Point3f addOriginPt = repairMesh->V(itor.first)->P();
				repairMesh->AddVertex(addOriginPt);//拆分点
				int addIndex = repairMesh->n_vertices() - 1;
				for (int n = 0; n < vg.second.size(); n++)
				{
					int originIndex = -1;
					for (int i = 0; i < 3; i++)
					{
						int v0 = vg.second[n]->m_v[i];
						if (v0 == itor.first)
						{
							originIndex = i;
							break;
						}
					}
					int v1 = vg.second[n]->m_v[(originIndex + 1) % 3];
					int v2 = vg.second[n]->m_v[(originIndex + 2) % 3];
					repairMesh->AddFace(addIndex, v1, v2);
					vg.second[n]->SetS();
				}
			}
		}
	}
	repairMesh->DirtyTopology();
	for (auto& itor : repairMesh->m_faces)
	{
		itor.ClearV();
	}
	for (auto& itor : repairMesh->m_faces)
	{
		itor.ClearS();
	}
	m_mesh_->Clear();
	m_mesh_ = repairMesh;
}

void MeshCheck::PImpl::repairNonManifoldEdges()
{
	if (m_mesh_->n_vertices() <= 0)
		return;

	std::map<MeshEdge, int> allEdges;
	std::map<MeshEdge, int> edgeIndexs;
	std::map<MeshEdge, std::vector<int>> edgeFaces;
	for (auto itor : m_mesh_->m_faces)
	{
		for (int i = 0; i < 3; i++)
		{
			int v0 = itor.m_v[i];
			int v1 = itor.m_v[(i + 1) % 3];
			MeshEdge e0 = std::make_pair(v0, v1);
			edgeFaces[e0].push_back(itor.m_index);
			edgeIndexs[e0] = i;
			allEdges[EdgeId(e0)]++;
		}
	}
	std::map<MeshEdge, std::vector<int>> nonManifoldEdge;
	for (auto wholeEdge : allEdges)
	{
		if (wholeEdge.second > 2)
		{
			MeshEdge edgeCW = wholeEdge.first;
			MeshEdge edgeCCW = std::make_pair(edgeCW.second, edgeCW.first);
			for (auto cwf : edgeFaces[edgeCW])
			{
				nonManifoldEdge[edgeCW].push_back(cwf);
			}
			for (auto ccwf : edgeFaces[edgeCCW])
			{
				nonManifoldEdge[edgeCW].push_back(ccwf);
			}
		}
	}

	//删除非流行面片
	m_mesh_->DirtyBoundary();
	for (auto itor : nonManifoldEdge)
	{
		int v0 = itor.first.first;
		int v1 = itor.first.second;
		m_mesh_->V(v0)->SetD();
		m_mesh_->V(v1)->SetD();

		/*for (auto it : itor.second)
		{
			Face* f = m_mesh_->F(it);
			if (itor.second.size() == 3)
			{
				if (f->V(0)->IsB() || f->V(1)->IsB() || f->V(2)->IsB())
				{
					f->SetD();
					break;
				}
			}
			else
			{
				int v0 = itor.first.first;
				int v1 = itor.first.second;
				m_mesh_->V(v0)->SetD();
				m_mesh_->V(v1)->SetD();
			}
		}*/
	}

	for (int i = 0; i < m_mesh_->n_faces(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Face* f = m_mesh_->F(i);
			if (f->V(j)->IsD())
			{
				f->SetD();
				break;
			}
		}
	}

	m_mesh_->Compact();
	m_mesh_->DirtyTopology();
	m_mesh_->DirtyBoundary();
}

std::vector<std::vector<int>> MeshCheck::PImpl::getAllConnectedVertices()
{
	if (m_mesh_->n_vertices() <= 3)
		return {};

	for (int i = 0; i < m_mesh_->n_vertices(); i++)
	{
		m_mesh_->V(i)->ClearV();
	}

	std::vector<std::vector<int>> verVertexList;
	for (int i = 0; i < m_mesh_->n_vertices(); i++)
	{
		if (m_mesh_->V(i)->IsV())
			continue;

		int seed = i;
		std::vector<int> curVertexList;
		Vertex* v0 = m_mesh_->V(seed);
		v0->SetV();
		std::queue<int> ringV;
		curVertexList.push_back(seed);
		ringV.push(seed);
		while (!ringV.empty())
		{
			int curIndex = ringV.front();
			Vertex* v = m_mesh_->V(curIndex);
			ringV.pop();

			std::vector<Vertex*> ringVertex;
			GetVertexRingVertices(m_mesh_.get(), v, ringVertex);
			for (auto itor : ringVertex)
			{
				if (itor->IsV())
					continue;
				itor->SetV();
				ringV.push(itor->m_index);
				curVertexList.push_back(itor->m_index);
			}
		}
		verVertexList.push_back(curVertexList);
	}
	return verVertexList;
}

int MeshCheck::PImpl::GetVertexRingVertices(sn3DCore::sn3DMeshData* mesh, sn3DCore::Vertex* v, std::vector<Vertex*>& ringVertics)
{
	ringVertics.clear();
	std::set<int> visited;

	Vertex* v0, * v1;
	VFIterator vfi(v);
	for (; !vfi.End(); ++vfi)
	{
		Face* f = vfi.F();
		v0 = f->V((vfi.z + 1) % 3);
		v1 = f->V((vfi.z + 2) % 3);
		if (visited.find(v0->Index()) == visited.end())
		{
			visited.insert(v0->Index());
			ringVertics.push_back(v0);
		}
		if (visited.find(v1->Index()) == visited.end())
		{
			visited.insert(v1->Index());
			ringVertics.push_back(v1);
		}
	}
	return ringVertics.size();
}