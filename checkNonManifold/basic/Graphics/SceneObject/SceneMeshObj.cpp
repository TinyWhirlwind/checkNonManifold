#include "SceneMeshObj.h"

#include <Windows.h>
#include <gl/glew.h>
#include <QtCore/qdebug.h>

#include "GeoData/MeshData.h"
#include "geom/ObjTriMesh.h"
#include "geom/ACADMeshGrid.h"
#include "sn3DMeshRenderer.h"
#include "SceneRender/LightColorGL.h"
#include "GeoData/MeshGrid.h"

#pragma warning (disable : 4996)

using namespace sn3DCore;
boost::shared_ptr<sn3DMeshModel> sn3DMeshModel::New()
{
	boost::shared_ptr<sn3DMeshModel> p(new sn3DMeshModel);
	return p;
}

sn3DMeshModel::sn3DMeshModel()
{
	m_type = TYPE_MESH;
	SetTypeName("MeshModel");

	imark = 0;
	Tr.SetIdentity();
}

sn3DMeshModel::sn3DMeshModel(boost::shared_ptr<sn3DMeshModel> model)
{
	boost::shared_ptr<sn3DMeshData> data = model->GetMeshData();
	AddMesh();

	if (data != 0)
	{
		int vn = data->vn;
		int fn = data->fn;

		for (int i = 0; i < vn; i++)
		{
			Vertex* v = m_mesh->AddVertex(data->V(i)->m_coord, data->V(i)->C());
		}

		for (int i = 0; i < fn; i++)
		{
			Face* f = data->F(i);
			int v0 = f->V(0)->Index();
			int v1 = f->V(1)->Index();
			int v2 = f->V(2)->Index();

			Face* newFace = m_mesh->AddFace(v0, v1, v2);
		}
	}

	m_mesh->DirtyTopology();
	UpdateNormal();
	UpdateBox();
}

sn3DMeshModel::sn3DMeshModel(boost::shared_ptr<sn3DMeshData> mesh)
{
	boost::shared_ptr<sn3DMeshData> data = mesh;
	AddMesh();

	if (data != 0)
	{
		int vn = data->vn;
		int fn = data->fn;

		for (int i = 0; i < vn; i++)
		{
			Vertex* v = m_mesh->AddVertex(data->V(i)->m_coord, data->V(i)->C());
		}

		for (int i = 0; i < fn; i++)
		{
			Face* f = data->F(i);
			int v0 = f->V(0)->Index();
			int v1 = f->V(1)->Index();
			int v2 = f->V(2)->Index();

			Face* newFace = m_mesh->AddFace(v0, v1, v2);
		}
	}

	m_mesh->DirtyTopology();
	UpdateNormal();
	UpdateBox();
}

sn3DMeshModel::~sn3DMeshModel()
{
	if (m_mesh)
	{
		m_mesh->Clear();
	}
}

void  sn3DMeshModel::ReleaseData(bool all)
{
	if (m_mesh)
	{
		m_mesh->Clear();
	}
}

void sn3DMeshModel::SetMesh(boost::shared_ptr<sn3DMeshData> m)
{
	if (m_mesh != m)
	{
		m_mesh = m;

		m->DirtyTopology();
		UpdateNormal();
		UpdateBox();

		if (m_render)
			m_render->SetGeometry(GetMeshData());
	}
}

boost::shared_ptr<sn3DMeshData> sn3DMeshModel::AddMesh()
{
	m_mesh.reset(new sn3DMeshData);

	// 外部需要计算法向和包围盒

	if (m_render)
		m_render->SetGeometry(GetMeshData());

	return m_mesh;
}

boost::shared_ptr<sn3DMeshData> sn3DMeshModel::AddMesh(
	int vNum,
	int faceNum,
	const::std::vector<Point3f>& points,
	const::std::vector<unsigned int>& faces,
	bool bTexture /*= false*/,
	const::std::vector<Point2f>& mapPoints)
{
	m_mesh.reset(new sn3DMeshData);

	////////  增加顶点
	if (!points.empty())
	{
		for (int i = 0; i < vNum; i++)
		{
			m_mesh->AddVertex(points[i]);
		}
	}
	else
	{
		for (int i = 0; i < vNum; i++)
		{
			m_mesh->AddVertex(Point3f(0.0, 0.0, 0.0));
		}
	}

	/////// 增加面
	if (!faces.empty())
	{
		for (int i = 0; i < faceNum; i++)
		{
			m_mesh->AddFace(faces[i * 3], faces[i * 3 + 1], faces[i * 3 + 2]);
		}
	}
	else
	{
		for (int i = 0; i < faceNum; i++)
		{
			m_mesh->AddFace(0, 0, 0);
		}
	}

	/////// 增加纹理
	if (bTexture)
	{
		m_mesh->UpdateVertexTexCoord();
		if (!mapPoints.empty())
		{
			for (int i = 0; i < vNum; i++)
			{
				m_mesh->V(i)->SetTexCoord(mapPoints[i]);
			}
		}
	}

	UpdateNormal();
	UpdateBox();

	if (m_render)
		m_render->SetGeometry(GetMeshData());

	return m_mesh;
}

void sn3DMeshModel::UpdateDisplay()
{
	if (!m_mesh)
		return;
	if (!m_render)
	{
		m_render.reset(new sn3DMeshRenderer);
	}
	m_render->SetGeometry(m_mesh);
	m_bNeedUpdateDisplay = false;
	//m_render->Update();
}

void sn3DMeshModel::BuildPreviewList()
{
}

void sn3DMeshModel::UpdateBox()
{
	if (!m_mesh)  return;
	if (m_mesh->fn == 0) return;

	//compute center
	m_center = Point3f(0.0, 0.0, 0.0);
	int count = 0; //未删除顶点计数
	for (int i = 0; i < (int)m_mesh->vn; i++)
	{
		if (m_mesh->V(i)->IsD())continue;
		m_center.X() += m_mesh->V(i)->m_coord.X();
		m_center.Y() += m_mesh->V(i)->m_coord.Y();
		m_center.Z() += m_mesh->V(i)->m_coord.Z();
		count++;
	}

	m_center.X() /= (float)count;
	m_center.Y() /= (float)count;
	m_center.Z() /= (float)count;

	//compute box
	Point3f max = Point3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	Point3f min = Point3f(FLT_MAX, FLT_MAX, FLT_MAX);

	for (int i = 0; i < (int)m_mesh->vn; i++)
	{
		if (m_mesh->V(i)->IsD())continue;
		if (min.X() > m_mesh->V(i)->m_coord.X()) min.X() = m_mesh->V(i)->m_coord.X();
		if (min.Y() > m_mesh->V(i)->m_coord.Y()) min.Y() = m_mesh->V(i)->m_coord.Y();
		if (min.Z() > m_mesh->V(i)->m_coord.Z()) min.Z() = m_mesh->V(i)->m_coord.Z();
		if (max.X() < m_mesh->V(i)->m_coord.X()) max.X() = m_mesh->V(i)->m_coord.X();
		if (max.Y() < m_mesh->V(i)->m_coord.Y()) max.Y() = m_mesh->V(i)->m_coord.Y();
		if (max.Z() < m_mesh->V(i)->m_coord.Z()) max.Z() = m_mesh->V(i)->m_coord.Z();
	}

	m_bbox.max = max;
	m_bbox.min = min;

	int fn = m_mesh->fn;
	double avgEdgeLen = 0.0;

	for (int i = 0; i < fn; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			avgEdgeLen += (m_mesh->F(i)->P((j + 1) % 3) - m_mesh->F(i)->P(j)).Norm();
		}
	}

	m_avgEdgeLen = (float)(avgEdgeLen / ((double)fn * 3));
}

void sn3DMeshModel::UpdateNormal()
{
	if (m_mesh)
	{
		m_mesh->UpdateNormal();
	}
}

void sn3DMeshModel::UpdateDirection()
{
	if (!m_mesh)
		return;
}

void sn3DMeshModel::UpdateGeometry()
{
	if (m_mesh == nullptr)
		return;
}

void sn3DMeshModel::UpdateGeometryWithoutTranslation()
{
	if (m_mesh == nullptr)
		return;
}

void sn3DMeshModel::RenderForSelection()
{
	boost::shared_ptr<sn3DMeshData> data = GetMeshData();

	int faceN = data->fn;
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glBegin(GL_TRIANGLES);

	for (int i = 0; i < faceN; i++)
	{
		Face* f = data->F(i);
		if (f->IsD())continue;
		if (f->V(0)->IsD())continue;
		if (f->V(1)->IsD())continue;
		if (f->V(2)->IsD())continue;

		Point3f point;

		point = f->V(0)->m_coord;
		glVertex3f(point.X(), point.Y(), point.Z());

		point = f->V(1)->m_coord;
		glVertex3f(point.X(), point.Y(), point.Z());

		point = f->V(2)->m_coord;
		glVertex3f(point.X(), point.Y(), point.Z());
	}
	glEnd();
	glPopAttrib();
}

void sn3DMeshModel::Render()
{
	boost::shared_ptr<sn3DMeshRenderer> render = GetRenderer();
	if (!render)
		return;

	if (m_bNeedUpdateDisplay)
	{
		UpdateDisplay();
	}

	//if (m_bNeedUpdateColor)
	//{
	//}

	glPushAttrib(GL_TRANSFORM_BIT | GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
	LightColorGL::GreyColor(); // 设定颜色
	glEnable(GL_LIGHTING);

	if (!IsH())
	{
		//float matrix[16];
		//m_motion.GetTransformGL(matrix);
		glPushMatrix();
		glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
		m_render->Draw();
		glPopAttrib();
		glColor4f(0.0f, 0.0f, 0.7f, 0.2f);
		RenderSelected(); // 绘制被选择区域
		glPopMatrix();
	}

	glDisable(GL_TEXTURE_2D);
	glPopAttrib();
}

void sn3DMeshModel::RenderDynamic()
{
}

void sn3DMeshModel::UpdateAll()
{
	if (m_mesh)
		m_mesh->UpdateNormal();
	//UpdateDisplay();
	UpdateBox();
	setNeedUpdateModel();
}

void sn3DMeshModel::RenderBox()
{
	if (IsShowBox())
		m_render->DrawSolidBox(m_bbox.max, m_bbox.min);
}

void sn3DMeshModel::RenderWire()
{
	Matrix44f mvWorldMat = getModelWorldMatrix44f();

	if (!GetMeshData())
		return;
	boost::shared_ptr<sn3DMeshData> pData = GetMeshData();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonOffset(-1.0f, -1.0f);
	glLineWidth(2.0f);
	float _color[4] = { 0.0, 0.0, 0.0, 1.0 };
	glColor4fv(&_color[0]);
	int faceN = (int)pData->m_faces.size();
	for (int i = 0; i < faceN; i++)
	{
		Face* f = &pData->m_faces[i];
		if (pData->m_faces[i].IsD())continue;
		if (pData->m_faces[i].V(0)->IsD())continue;
		if (pData->m_faces[i].V(1)->IsD())continue;
		if (pData->m_faces[i].V(2)->IsD())continue;

		glBegin(GL_TRIANGLES);
		for (int j = 0; j < 3; ++j)
		{
			Point3f p = mvWorldMat * f->V(j)->P();
			glVertex3fv(&p[0]);
		}
		glEnd();
	}
}

void sn3DMeshModel::RenderSelected()
{
	GetRenderer()->RenderSelected();
}

int sn3DMeshModel::GetPointSize()
{
	if (!m_mesh)
		return 0;
	else
		return m_mesh->vn;
}

int sn3DMeshModel::GetFaceSize()
{
	if (!m_mesh)
		return 0;
	else
		return m_mesh->fn;
}

void sn3DMeshModel::Clear()
{
	if (m_mesh)
		m_mesh->Clear();
	if (m_render)
		m_render->Clear();
}

void sn3DMeshModel::SetDrawMode(GLW::DrawMode dm)
{
	boost::shared_ptr<sn3DMeshRenderer> render = GetRenderer();

	if (GetMeshData() == nullptr)
		return;
	if (GetMeshData()->n_faces() != 0)
	{
		render->SetDrawMode(dm);
	}
	else
	{
		render->SetDrawMode(GLW::DMPoints);
	}
}

void sn3DMeshModel::SetColorMode(int flag)
{
}

const char* sn3DMeshModel::GetFullPath()
{
	static std::string name;
	name = m_strPath;
	name += "temp\\";
	name += GetName();
	name += ".";
	name += GetExtName();
	return name.c_str();
}


void sn3DCore::sn3DMeshModel::MirrorFace()
{
	m_bIsMirrored = !m_bIsMirrored;
	if (m_mesh)
	{
		m_mesh->MirrorFace();
		setNeedUpdateModel();
		setNeedUpdateModel();
		
	}
}

void sn3DCore::sn3DMeshModel::GetTransformForOpengl(float* mat)
{
	Matrix44f mvWorldMat = getModelWorldMatrix44f();

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mat[i * 4 + j] = mvWorldMat[j][i];	//Matrix44f -> OpenGL矩阵 
		}
	}
}

void  sn3DCore::sn3DMeshModel::InitGrid()
{
	
	if (m_mesh == nullptr)
	{
		return;
	}
	m_meshForGrid.reset(new sn3DCore::sn3DMeshData);
	int vn = m_mesh->vn;
	int fn = m_mesh->fn;

	Matrix44f localscale = getLocalScaleMatrix44f();
	
	for (int i = 0; i < vn; i++)
	{
		Vertex* v = m_meshForGrid->AddVertex(localscale*m_mesh->V(i)->m_coord, m_mesh->V(i)->C());
	}

	for (int i = 0; i < fn; i++)
	{
		Face* f = m_mesh->F(i);
		int v0 = f->V(0)->Index();
		int v1 = f->V(1)->Index();
		int v2 = f->V(2)->Index();

		Face* newFace = m_meshForGrid->AddFace(v0, v1, v2);
	}
	m_meshForGrid->DirtyTopology();
	m_meshForGrid->UpdateNormal();

	Box3f Box = Geom::meshAABB(m_meshForGrid.get());
	float diag = (Box.max - Box.min).Norm();
	float resolution = diag / 30.f;
	m_Grid.reset(new MeshGrid(m_meshForGrid.get(), resolution));
	m_bNeedUpdateGrid = false;
}
boost::shared_ptr<MeshGrid> sn3DCore::sn3DMeshModel::GetGrid()
{
	if (m_bNeedUpdateGrid || m_Grid == nullptr)
	{
		InitGrid();
	}
	return m_Grid;
}

bool sn3DCore::sn3DMeshModel::RayCrossPoint(const Point3f& origin, const Point3f& dir, Point3f& hitPoint, int& hitTri)
{
	boost::shared_ptr<MeshGrid> meshGrid = GetGrid();
	if (!meshGrid)
		return false;
	Matrix44f tmat = getParentWorldMatrix44f() *getLocalMatrix44f()*Inverse(getLocalScaleMatrix44f());
	Matrix44f matToLocal = sn3DCore::Inverse(tmat);
	Matrix44f matRotateInverse = sn3DCore::Inverse(getWorldRotateMatrix44f());
	Point3f rayOrigin = matToLocal * origin;
	Point3f rayDir = matRotateInverse* dir;
	int caseNum;	
	Point3f hitPt;	
	meshGrid->IntersectMeshWithRay(rayOrigin, rayDir, hitTri, hitPt, caseNum);
	
	if (hitTri == -1)
		return false;
	hitPoint = tmat * hitPt;
	return true;
}

bool sn3DCore::sn3DMeshModel::ClosetPoint(const Point3f& point, Point3f& closetPt, Point3f& triNormal)
{
	boost::shared_ptr<MeshGrid> meshGrid = GetGrid();
	if (!meshGrid)
		return false;

	Matrix44f tmat = getParentWorldMatrix44f() * getLocalMatrix44f() * Inverse(getLocalScaleMatrix44f());
	Matrix44f matToLocal = sn3DCore::Inverse(tmat);
	Point3f q = matToLocal * point;

	float s, t, dist = FLT_MAX;
	int footCase;		
	Face* tri = meshGrid->PointDistanceToMesh(q, s, t, dist, footCase);
	if (!tri)
		return false;

	Point3f p1 = tri->V(0)->P();
	Point3f p2 = tri->V(1)->P();
	Point3f p3 = tri->V(2)->P();

	/// <summary>
	Point3f tmp1 = tmat * p1;
	Point3f tmp2 = tmat * p2;
	Point3f tmp3 = tmat * p3;
	/// <summary>


	Point3f tClosePt = p1 + (p2 - p1) * s + (p3 - p1) * t;
	closetPt = tmat * tClosePt;
	Point3f tNormal = (p2 - p1) ^ (p3 - p1);
	triNormal = getWorldRotateMatrix44f()*tNormal;
	triNormal.Normalize();
	return true;
}

bool sn3DCore::sn3DMeshModel::IsPointIn(const Point3f& point)
{
	boost::shared_ptr<MeshGrid> meshGrid = GetGrid();
	if (!meshGrid)
		return false;
	Matrix44f tmat = getParentWorldMatrix44f() * getLocalMatrix44f() * Inverse(getLocalScaleMatrix44f());
	Matrix44f matToLocal = sn3DCore::Inverse(tmat);
	Point3f q = matToLocal * point;

	Box3f box = meshGrid->getGridBox();
	if (!box.IsIn(q))return false;

	float s, t, dist = FLT_MAX;
	int footCase;		
	Face* tri = meshGrid->PointDistanceToMesh(q, s, t, dist, footCase);
	if (!tri)
		return false;

	Point3f p1 = tri->V(0)->P();
	Point3f p2 = tri->V(1)->P();
	Point3f p3 = tri->V(2)->P();

	Point3f dirPt = p1 + (p2 - p1) * s + (p3 - p1) * t;
	Point3f redir = q - dirPt;
	int iCase = 0; int triIdx = -1; Point3f hitPt;
	meshGrid->IntersectMeshWithRay(q, redir, triIdx, hitPt, iCase);

	if (triIdx != -1)
	{
		Face* pTri = meshGrid->m_pMesh->F(triIdx);
		Point3f tpt0 = pTri->P(0);
		Point3f tpt1 = pTri->P(1);
		Point3f tpt2 = pTri->P(2);
		Point3f nrv = ((tpt1 - tpt0) ^ (tpt2 - tpt0)).Normalize();
		if (nrv * (hitPt - q) > 0)
		{
			return true;
		}
	}
	return false;
}

bool sn3DCore::sn3DMeshModel::IntersectModelBox(boost::shared_ptr<sn3DCore::sn3DMeshModel> pModel)
{
	boost::shared_ptr<MeshGrid> pfirstGrid = GetGrid();
	boost::shared_ptr<MeshGrid> psecondGrid = pModel->GetGrid();
	if (pfirstGrid == nullptr || psecondGrid == nullptr)
	{
		return false;
	}
	auto getFaceDirection = [](int nIndex, std::vector <Point3f>& pt)->Point3f {
		if (pt.size() < 8)
		{
			return Point3f(0, 0, 0);
		}

		switch (nIndex)
		{
		case 0:
		{
			Point3f v0 = pt[2] - pt[0];
			Point3f v1 = pt[1] - pt[0];
			v0.Normalize();
			v1.Normalize();
			Point3f v2 = v0 ^ v1;
			v2.Normalize();
			return v2;
			break;
		}
		case 1:
		{
			Point3f v0 = pt[4] - pt[0];
			Point3f v1 = pt[2] - pt[0];
			v0.Normalize();
			v1.Normalize();
			Point3f v2 = v0 ^ v1;
			v2.Normalize();
			return v2;
			break;
		}
		case 2:
		{
			Point3f v0 = pt[1] - pt[0];
			Point3f v1 = pt[3] - pt[0];
			v0.Normalize();
			v1.Normalize();
			Point3f v2 = v0 ^ v1;
			v2.Normalize();
			return v2;
			break;
		}
		default:
			return Point3f(0, 0, 0);
			break;
		}
	};

	auto getInterval = [](std::vector<Point3f>& ptPoints, Point3f& ptDir, float& fMin, float& fMax)->void {
		fMin = FLT_MAX;
		fMax = -FLT_MAX;
		for (auto& ptPoint : ptPoints)
		{
			Point3f pt1 = ptPoint;
			Point3f pt2 = ptDir;
			pt1.Normalize();
			pt2.Normalize();
			float fDot = pt1 * pt2;
			float ret = fDot * ptPoint.Norm();

			fMin = qMin(fMin, ret);
			fMax = qMax(fMax, ret);
		}
	};
	auto getCornerPts = [](std::vector<Point3f>& corners, Box3f box, Matrix44f MatToWorld)
	{
		corners.clear();
		corners.push_back(MatToWorld*Point3f(box.min.x, box.min.y, box.min.z));
		corners.push_back(MatToWorld*Point3f(box.max.x, box.min.y, box.min.z));
		corners.push_back(MatToWorld*Point3f(box.min.x, box.max.y, box.min.z));
		corners.push_back(MatToWorld*Point3f(box.min.x, box.min.y, box.max.z));

		corners.push_back(MatToWorld*Point3f(box.min.x, box.max.y, box.max.z));
		corners.push_back(MatToWorld*Point3f(box.max.x, box.min.y, box.max.z));
		corners.push_back(MatToWorld*Point3f(box.max.x, box.max.y, box.min.z));
		corners.push_back(MatToWorld*Point3f(box.max.x, box.max.y, box.max.z));
	};

	std::vector<Point3f>firstCorners;
	std::vector<Point3f>secondCorners;
	Matrix44f FirstToWorld = getParentWorldMatrix44f() * getLocalTranformMatrix44f() * getLocalRotateMatrix44f();
	Matrix44f SecondToWorld = pModel->getParentWorldMatrix44f() * pModel->getLocalTranformMatrix44f() * pModel->getLocalRotateMatrix44f();
	getCornerPts(firstCorners, pfirstGrid->getGridBox(), FirstToWorld);
	getCornerPts(secondCorners, psecondGrid->getGridBox(), SecondToWorld);
	std::vector<Point3f> ptDir;
	std::vector<Point3f> ptDir2;
	for (int i = 0; i < 3; ++i)
	{
		ptDir.push_back(getFaceDirection(i, firstCorners));
		ptDir2.push_back(getFaceDirection(i, secondCorners));
	}

	float fMin1, fMax1, fMin2, fMax2;
	for (int i = 0; i < 3; ++i)
	{
		getInterval(firstCorners, ptDir[i], fMin1, fMax1);
		getInterval(secondCorners, ptDir[i], fMin2, fMax2);
		if (fMax2 < fMin1 || fMax1 < fMin2)
		{
			return false;
		}

		getInterval(firstCorners, ptDir2[i], fMin1, fMax1);
		getInterval(secondCorners, ptDir2[i], fMin2, fMax2);
		if (fMax2 < fMin1 || fMax1 < fMin2)
		{
			return false;
		}
	}

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			Point3f ptAxis = ptDir[i] ^ ptDir2[j];
			ptAxis.Normalize();

			getInterval(firstCorners, ptAxis, fMin1, fMax1);
			getInterval(secondCorners, ptAxis, fMin2, fMax2);
			if (fMax2 < fMin1 || fMax1 < fMin2)
			{
				return false;
			}
		}
	}

	return true;

}