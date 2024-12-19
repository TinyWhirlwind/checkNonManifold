#include "MeshData.h"

#include "MeshTopology.h"

#pragma warning (disable : 4129)

namespace sn3DCore
{
    boost::shared_ptr<sn3DMeshData> sn3DMeshData::New()
    {

        boost::shared_ptr<sn3DMeshData> p(new sn3DMeshData);
        return p;
    }
    sn3DMeshData::sn3DMeshData()
    {
        fn = 0;
        vn = 0;
        en = 0;
    }

    sn3DMeshData::sn3DMeshData(boost::shared_ptr<sn3DMeshData> pData)
    {
        std::vector<int> vmap;
        std::vector<int> _vmap;
        fn = 0;
        vn = 0;
        en = 0;
        _vmap.resize(pData->vn);

        int vnt = pData->vn;
        int fnt = pData->fn;

        for (int i = 0; i < vnt; i++)
        {
            _vmap[i] = this->vn;
            vmap.push_back(i);
            Vertex *v = AddVertex(pData->V(i)->m_coord, pData->V(i)->m_color);
            if (pData->V(i)->IsD()) v->SetD();
        }

        for (int i = 0; i < fnt; i++)
        {
            Face * f = pData->F(i);

            int v0 = f->V(0)->Index();
            int v1 = f->V(1)->Index();
            int v2 = f->V(2)->Index();

            Face * newFace = AddFace(_vmap[v0], _vmap[v1], _vmap[v2]);
            if (f->IsD()) newFace->SetD();
        }

        UpdateVertexIdentity();

        for (int i = 0; i < this->vn; i++)
        {
            V(i)->Identity() = vmap[i];
        }
    }

    sn3DMeshData::~sn3DMeshData()
    {
    }

    int sn3DMeshData::n_vertices()
    {
        return m_vertices.size();
    }

    int sn3DMeshData::n_faces()
    {
        return m_faces.size();
    }

    Vertex *sn3DMeshData::V(int index)
    {
        int size = (int)m_vertices.size();
        if (index >= 0 && index < size)
            return &m_vertices[index];
        else
            return nullptr;
    }

    const Vertex * sn3DMeshData::V(int index) const
    {
        int size = (int)m_vertices.size();
        if (index >= 0 && index < size)
            return &m_vertices[index];
        else
            return nullptr;
    }

    Edge *sn3DMeshData::E(int index)
    {
        int size = (int)m_edges.size();
        if (index >= 0 && index < size)
            return &m_edges[index];
        else return 0;
    }

    Edge *sn3DMeshData::FE(int f, int i)
    {
        int id = m_pEdges[f * 3 + i];
        return &m_edges[id];

    }

    Face *sn3DMeshData::F(int index)
    {
        int size = (int)m_faces.size();
        if (index >= 0 && index < size)
            return &m_faces[index];
        else
            return nullptr;
    }

    const Face *sn3DMeshData::F(int index) const
    {
        int size = (int)m_faces.size();
        if (index >= 0 && index < size)
            return &m_faces[index];
        else
            return nullptr;
    }

    Vertex *sn3DMeshData::AddVertex()
    {
        int index = (int)m_vertices.size();
        Vertex vertex;
        vertex.SetGeometry(this);
        vertex.Index() = index;
        m_vertices.push_back(vertex);
        vn++;
        return &m_vertices.back();
    }

    Vertex *sn3DMeshData::AddVertex(const Point3f & point, const Color4f& color, bool update)
    {
        int index = (int)m_vertices.size();
        Vertex vertex(point, index, this);
        m_vertices.push_back(vertex);

        if (update)
        {
            if (this->m_vertexTopology.size() == vn)
                m_vertexTopology.push_back(VertexTopology());
        }

        vn++;
        m_vertices.back().m_color = color;
        return &m_vertices.back();
    }

    Vertex * sn3DMeshData::AddVertex(const Point3d & point, const Color4f& color, bool update)
    {
        int index = (int)m_vertices.size();
        Vertex vertex(Point3f(point[0], point[1], point[2]), index, this);
        m_vertices.push_back(vertex);

        if (update)
        {
            if (this->m_vertexTopology.size() == vn)
                m_vertexTopology.push_back(VertexTopology());
        }
        vn++;
        m_vertices.back().m_color = color;
        return &m_vertices.back();
    }

    void sn3DMeshData::AddVertex(int n)
    {
        m_vertices.reserve(m_vertices.size() + n);
        for (int i = 0; i < n; ++i)
            AddVertex();
    }

    Face *sn3DMeshData::AddFace()
    {
        Face face;
        int index = (int)m_faces.size();
        face.SetGeometry(this);
        face.Index() = index;
        m_faces.push_back(face);
        fn++;
        return &m_faces.back();
    }
    Face *sn3DMeshData::AddFace(std::vector<int> &vIndices)
    {
        Face face;
        int index = (int)m_faces.size();
        face.SetV(0, vIndices[0]);
        face.SetV(1, vIndices[1]);
        face.SetV(2, vIndices[2]);
        face.SetGeometry(this);
        face.Index() = index;
        m_faces.push_back(face);
        fn++;
        return &m_faces.back();
    }
    Face *sn3DMeshData::AddFace(int v0, int v1, int v2, bool bReverse /*= false*/)
    {
        Face face;
        int index = (int)m_faces.size();
        if (!bReverse)
        {
            face.SetV(0, v0);
            face.SetV(1, v1);
            face.SetV(2, v2);
        }
        else
        {
            face.SetV(0, v2);
            face.SetV(1, v1);
            face.SetV(2, v0);
        }
        face.SetGeometry(this);
        face.Index() = index;
        m_faces.push_back(face);
        if (this->m_faceTopology.size() == fn)
            m_faceTopology.push_back(FaceTopology());
        fn++;
        return &m_faces.back();
    }

    void sn3DMeshData::AddFace(int n)
    {
        m_faces.reserve(m_faces.size() + n);
        for (int i = 0; i < n; ++i)
            AddFace();
    }

    void sn3DMeshData::AddFaceTexCoord(Point2f p1, Point2f p2, Point2f p3)
    {
        m_faceTexCoord.push_back(p1);
        m_faceTexCoord.push_back(p2);
        m_faceTexCoord.push_back(p3);
    }

    void sn3DMeshData::AddFaceTopology()
    {
        FaceTopology topology;
        m_faceTopology.push_back(topology);
    }

    void sn3DMeshData::AddFaceNormal()
    {
        m_faceNormal.push_back(Point3f());
    }

    void sn3DMeshData::DeleteVertex(int index)
    {
        m_vertices[index].SetD();
    }

    void sn3DMeshData::DeleteFace(int index)
    {
        m_faces[index].SetD();
        fn--;
    }

    void sn3DMeshData::Compact()
    {
        int  indexF = 0; //未删除元素索引
        int  indexV = 0;
        int  end;

        end = (int)m_faces.size();
        for (int i = 0; i < end; i++)
        {
            if (!m_faces[i].IsD())
                m_faces[i].Index() = indexF++;
            else m_faces[i].Index() = -2;
        }

        end = (int)m_vertices.size();
        for (int i = 0; i < end; i++)
        {
            if (!m_vertices[i].IsD())
                m_vertices[i].Index() = indexV++;
            else m_vertices[i].Index() = -2;
        }

        int * newV0 = new int[indexF];
        int * newV1 = new int[indexF];
        int * newV2 = new int[indexF];
        float * newPX = new float[indexV];
        float * newPY = new float[indexV];
        float * newPZ = new float[indexV];
        Point3f * newVN = new Point3f[indexV];
        int * flagF = new int[indexF];
        int * flagV = new int[indexV];
        int tempIndexV = 0;
        int tempIndexF = 0;

        Point2f * faceTex = nullptr;
        Point3f * vertexNormal = nullptr;

        if (m_faceTexCoord.size() != 0)
        {
            faceTex = new Point2f[indexF * 3];
        }
        if (fn == 0 && m_vertexNormal.size() != 0)
        {
            vertexNormal = new Point3f[indexV];
        }

        end = (int)m_faces.size();
        for (int i = 0; i < end; i++)
        {
            if (m_faces[i].IsD())continue;
            newV0[tempIndexF] = m_faces[i].V(0)->Index();
            newV1[tempIndexF] = m_faces[i].V(1)->Index();
            newV2[tempIndexF] = m_faces[i].V(2)->Index();
            flagF[tempIndexF] = m_faces[i].Flag();
            if (m_faceTexCoord.size() != 0)
            {
                faceTex[tempIndexF * 3] = m_faceTexCoord[i * 3];

                faceTex[tempIndexF * 3 + 1] = m_faceTexCoord[i * 3 + 1];

                faceTex[tempIndexF * 3 + 2] = m_faceTexCoord[i * 3 + 2];

            }
            tempIndexF++;
        }

        end = (int)m_vertices.size();
        for (int i = 0; i < end; i++)
        {
            if (m_vertices[i].IsD())continue;
            newPX[tempIndexV] = m_vertices[i].m_coord.X();
            newPY[tempIndexV] = m_vertices[i].m_coord.Y();
            newPZ[tempIndexV] = m_vertices[i].m_coord.Z();
            flagV[tempIndexV] = m_vertices[i].Flag();
            if (vertexNormal)vertexNormal[tempIndexV] = m_vertexNormal[i];
            tempIndexV++;
        }

        m_faces.resize(indexF);
        m_vertices.resize(indexV);
        if (faceTex != nullptr)m_faceTexCoord.resize(indexF * 3, Point2f(0.0, 0.0));
        if (vertexNormal != nullptr)m_vertexNormal.resize(indexV);

        for (int i = 0; i < indexF; i++)
        {
            m_faces[i].SetV(0, newV0[i]);
            m_faces[i].SetV(1, newV1[i]);
            m_faces[i].SetV(2, newV2[i]);
            m_faces[i].Flag() = flagF[i];
            m_faces[i].Index() = i;
            m_faces[i].SetGeometry(this);
            if (faceTex != nullptr)
            {
                m_faces[i].SetTexCoord(faceTex[i * 3], 0);
                m_faces[i].SetTexCoord(faceTex[i * 3 + 1], 1);
                m_faces[i].SetTexCoord(faceTex[i * 3 + 2], 2);
            }
        }

        for (int i = 0; i < indexV; i++)
        {
            m_vertices[i].m_coord.X() = newPX[i];
            m_vertices[i].m_coord.Y() = newPY[i];
            m_vertices[i].m_coord.Z() = newPZ[i];
            m_vertices[i].Flag() = flagV[i];
            m_vertices[i].Index() = i;
            m_vertices[i].SetGeometry(this);
            if (vertexNormal)m_vertexNormal[i] = vertexNormal[i];
        }

        fn = (int)m_faces.size();
        vn = (int)m_vertices.size();

        delete[] newV0;
        delete[] newV1;
        delete[] newV2;
        delete[] newPX;
        delete[] newPY;
        delete[] newPZ;
        delete[] newVN;
        delete[] flagF;
        if (vertexNormal != nullptr)
            delete[] vertexNormal;
        if (faceTex != nullptr)
        {
            delete[] faceTex;

            m_vertexTexCoord.resize(vn);
            for (int i = 0; i < (int)fn; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    F(i)->V(j)->SetTexCoord(F(i)->TexCoord(j));
                }
            }
        }
    }

    void sn3DMeshData::operator = (boost::shared_ptr<sn3DMeshData> copy)
    {
        int end;

        //分配存储空间
        m_vertices.clear();
        m_faces.clear();
        end = copy->m_vertices.size();
        m_vertices.resize(end);
        end = copy->m_faces.size();
        m_faces.resize(end);

        fn = copy->fn;
        vn = copy->vn;

        //复制顶点信息
        end = m_vertices.size();
        for (int i = 0; i < end; i++)
        {
            m_vertices[i].m_coord = copy->m_vertices[i].m_coord;
            m_vertices[i].Flag() = copy->m_vertices[i].Flag();
            m_vertices[i].Index() = i;
            m_vertices[i].SetGeometry(this);
        }

        //复制面信息
        end = m_faces.size();
        for (int i = 0; i < end; i++)
        {
            m_faces[i].Flag() = copy->m_faces[i].Flag();
            m_faces[i].Index() = i;
            m_faces[i].SetGeometry(this);
            for (int j = 0; j < 3; j++)
                m_faces[i].SetV(j, copy->m_faces[i].V(j)->Index());
        }
    }

    boost::shared_ptr<sn3DMeshData> sn3DMeshData::Copy()
    {
        boost::shared_ptr<sn3DMeshData> newMesh(new sn3DMeshData);

        int end;
        //分配存储空间
        end = this->m_vertices.size();
        newMesh->m_vertices.resize(end);
        end = this->m_faces.size();
        newMesh->m_faces.resize(end);

        newMesh->fn = this->fn;
        newMesh->vn = this->vn;

        //复制顶点信息
        end = m_vertices.size();
        for (int i = 0; i < end; i++)
        {
            newMesh->m_vertices[i].m_coord = this->m_vertices[i].m_coord;
            newMesh->m_vertices[i].Flag() = this->m_vertices[i].Flag();
            newMesh->m_vertices[i].Index() = i;
            newMesh->m_vertices[i].SetGeometry(this);
        }

        //复制面信息
        end = m_faces.size();
        for (int i = 0; i < end; i++)
        {
            newMesh->m_faces[i].Flag() = this->m_faces[i].Flag();
            newMesh->m_faces[i].Index() = i;
            newMesh->m_faces[i].SetGeometry(this);
            for (int j = 0; j < 3; j++)
                newMesh->m_faces[i].SetV(j, this->m_faces[i].V(j)->Index());
        }
        return newMesh;
    }

    void sn3DMeshData::Clear()
    {
        if ((int)m_vertices.size() > 0)
            std::vector<sn3DCore::Vertex>().swap(m_vertices);
        if ((int)m_faces.size() > 0)
            std::vector<sn3DCore::Face>().swap(m_faces);
        if ((int)m_edges.size() > 0)
            std::vector<sn3DCore::Edge>().swap(m_edges);
        if ((int)m_vertexNormal.size() > 0)
            std::vector<Point3f>().swap(m_vertexNormal);
        if ((int)m_faceNormal.size() > 0)
            std::vector<Point3f>().swap(m_faceNormal);
        if ((int)m_vertexMark.size() > 0)
            std::vector<int>().swap(m_vertexMark);
        if ((int)m_faceMark.size() > 0)
            std::vector<int>().swap(m_faceMark);
        if ((int)m_vertexColor.size() > 0)
            std::vector<sn3DCore::Color4f>().swap(m_vertexColor);
        if ((int)m_faceColor.size() > 0)
            std::vector<sn3DCore::Color4f>().swap(m_faceColor);
        if ((int)m_vertexTexCoord.size() > 0)
            std::vector<Point2f>().swap(m_vertexTexCoord);
        if ((int)m_faceTexCoord.size() > 0)
            std::vector<Point2f>().swap(m_faceTexCoord);
        if ((int)m_vertexTopology.size() > 0)
            std::vector<sn3DCore::VertexTopology>().swap(m_vertexTopology);
        if ((int)m_faceTopology.size() > 0)
            std::vector<sn3DCore::FaceTopology>().swap(m_faceTopology);
        if ((int)m_FaceIdentity.size() > 0)
            std::vector<int>().swap(m_FaceIdentity);
        if ((int)m_FaceIdentity1.size() > 0)
            std::vector<int>().swap(m_FaceIdentity1);
        if ((int)m_FaceIdentity2.size() > 0)
            std::vector<int>().swap(m_FaceIdentity2);
        fn = 0;
        vn = 0;
    }

    void sn3DMeshData::UpdateNormal()
    {
        int faceNum = (int)m_faces.size();
        int vertexNum = (int)m_vertices.size();
        if (faceNum == 0)return;

        m_faceNormal.resize(faceNum);
        m_vertexNormal.resize(vertexNum);

        for (int i = 0; i < faceNum; i++)
        {
            Point3f p0, p1, p2;
            p0 = F(i)->P(0);
            p1 = F(i)->P(1);
            p2 = F(i)->P(2);
            F(i)->N() = ((p1 - p0) ^ (p2 - p0)).Normalize();
        }
        for (int i = 0; i < vertexNum; i++)
        {
            V(i)->N() = Point3f(0, 0, 0);
        }
        for (int i = 0; i < faceNum; i++)
        {
            Point3f n = F(i)->N();
            for (int j = 0; j < 3; ++j)
                F(i)->V(j)->N() += n;
        }
        for (int i = 0; i < vertexNum; i++)
        {
            V(i)->N().Normalize();
        }
    }

    void sn3DMeshData::ClearNormal()
    {
        std::vector<Point3f>().swap(m_faceNormal);
        std::vector<Point3f>().swap(m_vertexNormal);
    }

    void sn3DMeshData::CopyGeometry(boost::shared_ptr<sn3DMeshData> gm)
    {
        Clear();

        for (int i = 0; i < (int)gm->vn; i++)
        {
            Point3f p = gm->V(i)->P();
            Vertex * v = AddVertex(p, gm->V(i)->C());
            v->Flag() = gm->V(i)->Flag();
        }

        for (int i = 0; i < (int)gm->fn; i++)
        {
            Face * f = AddFace(gm->F(i)->V(0)->Index(), gm->F(i)->V(1)->Index(), gm->F(i)->V(2)->Index());
            f->Flag() = gm->F(i)->Flag();
        }

        UpdateNormal();
    }

    void  sn3DMeshData::SetAttrib(std::vector<float> & attribs)
    {
        float minv = FLT_MAX;
        float maxv = -FLT_MAX;

        m_vertexColor.clear();

        for (int i = 0; i < vn; i++)
        {
            float v = attribs[i];
            if (v > maxv)maxv = v;
            if (v < minv)minv = v;
        }

        for (int i = 0; i < vn; i++)
        {
            float v = attribs[i];
            Color4f color;
            color.ColorRamp(maxv, minv, v);
            m_vertexColor.push_back(color);
        }
    }

    float sn3DMeshData::GetAvgEdgeLen()
    {
        double len = 0.0;

        for (int i = 0; i < fn; i++)
        {
            Face * f = F(i);
            for (int j = 0; j < 3; j++)
            {
                len += (f->P((j + 1) % 3) - f->P(j)).Norm();
            }
        }
        len /= (double)(fn * 3);
        return (float)len;
    }

    void sn3DMeshData::UpdateEdge()
    {
        if (vn == 0) return;
        if (fn == 0) return;

        Topology::VertexFace(*this);

        m_pEdges.resize(fn * 3);
        m_edges.clear();

        en = 0;

        for (int i = 0; i < fn; i++)
        {
            m_pEdges[i * 3] = -1;
            m_pEdges[i * 3 + 1] = -1;
            m_pEdges[i * 3 + 2] = -1;
        }

        //====== physical edge
        for (int i = 0; i < vn; i++)
        {
            VFIterator vf(V(i));
            for (; !vf.End(); ++vf)
            {
                Face * f = vf.f;
                f->V((vf.z + 1) % 3)->ClearV();
                f->V((vf.z + 2) % 3)->ClearV();
            }

            VFIterator vfi(V(i));
            for (; !vfi.End(); ++vfi)
            {
                Face * f = vfi.f;
                Vertex * v0 = f->V((vfi.z + 1) % 3);
                Vertex * v1 = f->V((vfi.z + 2) % 3);

                if (!v0->IsV())
                {
                    v0->SetV();
                    if (v0->Index() > i)
                    {
                        Edge edge;
                        edge.V(0) = i;
                        edge.V(1) = v0->Index();
                        edge.Index() = (int)m_edges.size();
                        m_edges.push_back(edge);
                        m_pEdges[f->Index() * 3 + vfi.z] = en;
                        en++;
                    }
                }

                if (!v1->IsV())
                {
                    v1->SetV();
                    if (v1->Index() > i)
                    {
                        Edge edge;
                        edge.V(0) = i;
                        edge.V(1) = v1->Index();
                        edge.Index() = (int)m_edges.size();
                        m_edges.push_back(edge);
                        m_pEdges[f->Index() * 3 + (vfi.z + 2) % 3] = en;
                        en++;
                    }
                }
            }
        }

        //====== edge indices
        for (int i = 0; i < fn; i++)
            for (int j = 0; j < 3; j++)
            {
                if (m_pEdges[i * 3 + j] != -1)continue;
                VFIterator vfi(F(i)->V((j + 1) % 3));
                for (; !vfi.End(); ++vfi)
                {
                    if (vfi.f == F(i))continue;
                    if (vfi.f->V((vfi.z + 1) % 3) == F(i)->V(j))
                    {
                        m_pEdges[i * 3 + j] = m_pEdges[vfi.f->Index() * 3 + vfi.z];
                        break;
                    }
                    else if (vfi.f->V((vfi.z + 2) % 3) == F(i)->V(j))
                    {
                        m_pEdges[i * 3 + j] = m_pEdges[vfi.f->Index() * 3 + (vfi.z + 2) % 3];
                        break;
                    }
                }
            }

        //====== incident face
        for (int i = 0; i < en; i++)
        {
            Edge * e = E(i);
            Vertex * v0 = V(e->V(0));
            Vertex * v1 = V(e->V(1));
            e->F(0) = -1;
            e->F(1) = -1;

            VFIterator vfi(v0);
            for (; !vfi.End(); ++vfi)
            {
                Face * f = vfi.f;
                int z = vfi.z;
                if (f->V((z + 1) % 3) == v1)
                    e->F(0) = f->Index();
                else if (f->V((z + 2) % 3) == v1)
                    e->F(1) = f->Index();
            }

            if (e->F(0) == -1 || e->F(1) == -1)
                e->SetB();
            else
                e->ClearB();
        }
    }

    void sn3DMeshData::ClearEdge()
    {
        std::vector<Edge>().swap(m_edges);
        std::vector<int>().swap(m_pEdges);
        en = 0;
    }

    bool  sn3DMeshData::HasFaceTexture()
    {
        return (fn != 0 && (fn * 3) == this->m_faceTexCoord.size());
    }

    void  sn3DMeshData::UpdateVertexTopology()
    {
        m_vertexTopology.resize(m_vertices.size());
    }

    void  sn3DMeshData::UpdateFaceTopology()
    {
        m_faceTopology.resize(m_faces.size());
    }

    void sn3DMeshData::ClearVertexTopology()
    {
        std::vector<VertexTopology>().swap(m_vertexTopology);
    }

    void sn3DMeshData::ClearFaceTopology()
    {
        std::vector<FaceTopology>().swap(m_faceTopology);
    }

    void sn3DMeshData::UpdateFaceIdentity()                    //  更新面标志
    {
        if (m_FaceIdentity.size() == 0)
            m_FaceIdentity.resize(m_faces.size(), -1);
        else
        {
            int num = 0;
            int index = 0;
            int end = (int)m_faces.size();
            std::vector<int> temp;
            for (int i = 0; i < end; i++)
                if (!F(i)->IsD())num++;
            temp.resize(num);
            for (int i = 0; i < end; i++)
            {
                if (!F(i)->IsD())
                {
                    temp[index++] = m_FaceIdentity[i];
                }
            }

            m_FaceIdentity.resize(num);
            m_FaceIdentity = temp;
        }
    }

    void sn3DMeshData::UpdateFaceIdentity1()
    {
        if (m_FaceIdentity1.size() == 0)
            m_FaceIdentity1.resize(m_faces.size(), 0);
        else
        {
            int num = 0;
            int index = 0;
            int end = (int)m_faces.size();
            std::vector<int> temp;
            for (int i = 0; i < end; i++)
                if (!F(i)->IsD())num++;
            temp.resize(num);
            for (int i = 0; i < end; i++)
            {
                if (!F(i)->IsD())
                {
                    temp[index++] = m_FaceIdentity1[i];
                }
            }

            m_FaceIdentity1.resize(num);
            m_FaceIdentity1 = temp;
        }
    }

    void sn3DMeshData::UpdateFaceIdentity2()
    {
        if (m_FaceIdentity2.size() == 0)
            m_FaceIdentity2.resize(m_faces.size(), 0);
        else
        {
            int num = 0;
            int index = 0;
            int end = (int)m_faces.size();
            std::vector<int> temp;
            for (int i = 0; i < end; i++)
                if (!F(i)->IsD())num++;
            temp.resize(num);
            for (int i = 0; i < end; i++)
            {
                if (!F(i)->IsD())
                {
                    temp[index++] = m_FaceIdentity2[i];
                }
            }

            m_FaceIdentity2.resize(num);
            m_FaceIdentity2 = temp;
        }
    }

    void sn3DMeshData::UpdateVertexIdentity()
    {
        m_VertexIdentity.resize(m_vertices.size(), 0);
    }

    void sn3DMeshData::ClearVertexIdentity()
    {
        std::vector<int>().swap(m_VertexIdentity);
    }

    void sn3DMeshData::UpdateFaceTexCoord()
    {
        m_faceTexCoord.resize(m_faces.size() * 3, Point2f(0.0, 0.0));
    }

    void sn3DMeshData::UpdateVertexTexCoord()
    {
        m_vertexTexCoord.resize(m_vertices.size(), Point2f(0.0, 0.0));
    }

    void sn3DMeshData::UpdateFaceMark()
    {
        m_faceMark.resize(m_faces.size());
    }

    void sn3DMeshData::ClearFaceMark()
    {
        std::vector<int>().swap(m_faceMark);
    }

    void sn3DMeshData::UpdateVertexMark()
    {
        m_vertexMark.resize(m_vertices.size());
    }

    void sn3DMeshData::ClearVertexMark()
    {
        std::vector<int>().swap(m_vertexMark);
    }

    void sn3DMeshData::UpdateVertexAttrib()
    {
        m_vertexAttrib.resize(m_vertices.size());
    }

    void sn3DMeshData::ClearVertexAttrib()
    {
        std::vector<float>().swap(m_vertexAttrib);
    }

    void sn3DMeshData::DirtyTopology()
    {
        Topology::VertexFace(*this);
        Topology::FaceFace(*this);
    }

    void sn3DMeshData::DirtyBoundary()
    {
        Topology::IdentifyBoundary(*this);
    }

    void sn3DMeshData::RemoveRedundant()
    {
        Topology::RemoveRedundant(this);
    }

    void sn3DMeshData::MirrorFace()
    {
        std::vector<Face>::iterator itorFace = m_faces.begin();
        while (itorFace != m_faces.end())
        {
            int nIndex = itorFace->m_v[0];
            itorFace->m_v[0] = itorFace->m_v[1];
            itorFace->m_v[1] = nIndex;
            itorFace++;
        }
        UpdateNormal();
    }
}
//=================== Geometry ==============================
namespace Geom
{
    float faceArea(Face * f)
    {
        Point3f & p1 = f->P(0);
        Point3f & p2 = f->P(1);
        Point3f & p3 = f->P(2);

        return 0.5 * ((p2 - p1) ^ (p3 - p1)).Norm();
    }

    Point3f faceNormal(Face * f)
    {
        Point3f & p1 = f->P(0);
        Point3f & p2 = f->P(1);
        Point3f & p3 = f->P(2);

        return ((p2 - p1) ^ (p3 - p1)).Normalize();
    }

    Point3f PlaneProject(Point3f upDir, Point3f dir)
    {
        Point3f side;
        Point3f result;

        side = (dir ^ upDir).Normalize();

        result = (side ^ upDir).Normalize();

        return result;
    }

    void updateBoundingBox(sn3DMeshData * mesh, Point3f upDir, Point3f mdir, Point3f sdir, OrientedBoundingBox & box)
    {
        size_t vn = mesh->vn;

        Point3f minp = Point3f(FLT_MAX, FLT_MAX, FLT_MAX);
        Point3f maxp = Point3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        for (size_t it = 0; it < vn; it++)
        {
            Vertex * v = mesh->V(it);
            Point3f p = v->P();

            if (p[0] > maxp[0])maxp[0] = p[0];
            if (p[1] > maxp[1])maxp[1] = p[1];
            if (p[2] > maxp[2])maxp[2] = p[2];

            if (p[0] < minp[0])minp[0] = p[0];
            if (p[1] < minp[1])minp[1] = p[1];
            if (p[2] < minp[2])minp[2] = p[2];

        }

        Point3f w_center = (maxp + minp) / 2.0; // AABB 中心
        minp = Point3f(FLT_MAX, FLT_MAX, FLT_MAX);
        maxp = Point3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for (size_t it = 0; it < vn; it++)
        {
            Vertex * v = mesh->V(it);
            const Point3f p = v->P();

            Point3f coord;

            coord[0] = (p - w_center) * mdir;
            coord[1] = (p - w_center) * sdir;
            coord[2] = (p - w_center) * upDir;


            if (coord[0] > maxp[0])maxp[0] = coord[0];
            if (coord[1] > maxp[1])maxp[1] = coord[1];
            if (coord[2] > maxp[2])maxp[2] = coord[2];

            if (coord[0] < minp[0])minp[0] = coord[0];
            if (coord[1] < minp[1])minp[1] = coord[1];
            if (coord[2] < minp[2])minp[2] = coord[2];
        }

        //====== 定向包围盒
        box.upDir = upDir;
        box.mdir = mdir;
        box.sdir = sdir;

        Point3f c_center = (maxp + minp) / 2.0;
        box.center = w_center + mdir*c_center[0] + sdir*c_center[1] + upDir*c_center[2];
        box.mlen = (maxp - minp)[0] / 2.0;
        box.slen = (maxp - minp)[1] / 2.0;
        box.ulen = (maxp - minp)[2] / 2.0;
    }

    Box3f meshAABB(sn3DMeshData * mesh)
    {
        Point3f maxv, minv;
        maxv[0] = maxv[1] = maxv[2] = -FLT_MAX;
        minv[0] = minv[1] = minv[2] = FLT_MAX;
        size_t vn = mesh->vn;
        for (size_t i = 0; i < vn; i++)
        {
            Point3f p = mesh->V(i)->P();

            if (minv[0] > p[0]) minv[0] = p[0];
            if (minv[1] > p[1]) minv[1] = p[1];
            if (minv[2] > p[2]) minv[2] = p[2];
            if (maxv[0] < p[0]) maxv[0] = p[0];
            if (maxv[1] < p[1]) maxv[1] = p[1];
            if (maxv[2] < p[2]) maxv[2] = p[2];
        }

        Box3f box;
        box.max = maxv;
        box.min = minv;
        return box;
    }

    Point3f faceBarycentric(Face * f, Point3f & _p)
    {
        Point3f p0, p1, p2;
        p0 = f->P(0);
        p1 = f->P(1);
        p2 = f->P(2);

        int flag; // 投影平面
        float nx, ny, nz;
        float d;
        Point3f normal = f->N();
        nx = abs(normal.X());
        ny = abs(normal.Y());
        nz = abs(normal.Z());
        if (nx > ny && nx > nz) { flag = 1; d = 1 / nx; }
        else if (ny > nz) { flag = 2; d = 1 / ny; }
        else { flag = 3; d = 1 / nz; }
        float dett;
        Point3f point;
        point = _p;
        Point3f baryCoord;

        switch (flag)
        {
        case 1:
            dett = ((p0.Y() - p2.Y())*(p1.Z() - p2.Z())) - ((p0.Z() - p2.Z())*(p1.Y() - p2.Y()));
            baryCoord.X() = (p1.Z() - p2.Z())*(point.Y() - p2.Y()) - (p1.Y() - p2.Y())*(point.Z() - p2.Z());
            baryCoord.X() /= dett;
            baryCoord.Y() = -(p0.Z() - p2.Z())*(point.Y() - p2.Y()) + (p0.Y() - p2.Y())*(point.Z() - p2.Z());
            baryCoord.Y() /= dett;
            baryCoord.Z() = 1 - baryCoord.X() - baryCoord.Y();
            break;
        case 2:
            dett = ((p0.Z() - p2.Z())*(p1.X() - p2.X())) - ((p0.X() - p2.X())*(p1.Z() - p2.Z()));
            baryCoord.X() = (p1.X() - p2.X())*(point.Z() - p2.Z()) - (p1.Z() - p2.Z())*(point.X() - p2.X());
            baryCoord.X() /= dett;
            baryCoord.Y() = -(p0.X() - p2.X())*(point.Z() - p2.Z()) + (p0.Z() - p2.Z())*(point.X() - p2.X());
            baryCoord.Y() /= dett;
            baryCoord.Z() = 1 - baryCoord.X() - baryCoord.Y();
            break;
        case 3:
            dett = ((p0.X() - p2.X())*(p1.Y() - p2.Y())) - ((p0.Y() - p2.Y())*(p1.X() - p2.X()));
            baryCoord.X() = (p1.Y() - p2.Y())*(point.X() - p2.X()) - (p1.X() - p2.X())*(point.Y() - p2.Y());
            baryCoord.X() /= dett;
            baryCoord.Y() = -(p0.Y() - p2.Y())*(point.X() - p2.X()) + (p0.X() - p2.X())*(point.Y() - p2.Y());
            baryCoord.Y() /= dett;
            baryCoord.Z() = 1 - baryCoord.X() - baryCoord.Y();
            break;
        }
        return baryCoord;
    }

    double randomDouble()
    {
        return double(rand()) / (RAND_MAX + 1);
    }

    void  latticeTriangulation(sn3DMeshData * mesh, double(*functor)(double, double), float x, float y, float w, float h,
                               int precision, double distortion)
    {
        // Create the vertex simplices
        double stepSize = w / (float)precision;
        for (int i = 0; i <= precision; i++)
        {
            for (int j = 0; j <= precision; j++)
            {
                double thisDistortion = distortion;
                if (j < 2 || i < 2 || j > precision - 2 || i >precision - 2) thisDistortion = 0;
                double px = -0.96 + j*stepSize + (randomDouble() - 0.5)*thisDistortion*stepSize;
                double py = -0.96 + i*stepSize + (randomDouble() - 0.5)*thisDistortion*stepSize;
                double pz = functor(px, py);
                mesh->AddVertex(Point3f(px, py, pz));
            }
        }

        // Create the face simplices
        for (int i = 0; i < precision; i++)
        {
            for (int j = 0; j < precision; j++)
            {
                // Top triangle
                int v0, v1, v2;
                v0 = i*(precision + 1) + j;
                v1 = i*(precision + 1) + j + 1;
                v2 = (i + 1)*(precision + 1) + j;
                mesh->AddFace(v0, v1, v2);
                v0 = i*(precision + 1) + j + 1;
                v1 = (i + 1)*(precision + 1) + j + 1;
                v2 = (i + 1)*(precision + 1) + j;
                mesh->AddFace(v0, v1, v2);
            }
        }
    }
}
