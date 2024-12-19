#include "SceneColorMeshObj.h"

#include <windows.h>
#include <gl/glew.h>
#include <gl\gl.h>

#include "GeoData/MeshTopology.h"
#include "HierarchyHistogram.h"

ColorMeshModel::ColorMeshModel()
{
    m_meshType = 300;
}

ColorMeshModel::ColorMeshModel(sn3DMeshModel * model,std::vector<float> & func)
{
    AddMesh();
    boost::shared_ptr<sn3DMeshData> data = model->GetMeshData();

    int vn = data->vn;
    int fn = data->fn;
    for(int i=0;i<vn;i++)
    {
        m_mesh->AddVertex(data->V(i)->m_coord, data->V(i)->m_color);
    }
    for(int i=0;i<fn;i++)
    {
        Face * f = data->F(i);
        m_mesh->AddFace(f->V(0)->Index(),f->V(1)->Index(),f->V(2)->Index());
    }

    m_func = func;
}

void ColorMeshModel::SetVertexColor(sn3DMeshModel * model,std::vector<float> & values)
{
    AddMesh();
    boost::shared_ptr<sn3DMeshData> data = model->GetMeshData();

    int vn = data->vn;
    int fn = data->fn;
    for(int i=0;i<vn;i++)
    {
        m_mesh->AddVertex(data->V(i)->m_coord, data->V(i)->m_color);
    }
    for(int i=0;i<fn;i++)
    {
        Face * f = data->F(i);
        m_mesh->AddFace(f->V(0)->Index(),f->V(1)->Index(),f->V(2)->Index());
    }

    ///////////////
    m_func = values;

    AnalizeData(values,m_minv,m_maxv);

    float offset = (m_maxv-m_minv)*0.3;

    m_minv+= (m_maxv-m_minv)*0.1;
    m_maxv-= (m_maxv-m_minv)*0.31;

    m_minv = 0.0;//-1.661262;
    m_maxv = 0.1;//0.95;

    m_minv = -0.8855;
    m_maxv =0.983372;

    for(int i=0;i<vn;i++)
    {
        float v = values[i];

        if(v>m_maxv)v = m_maxv;
        if(v<m_minv)v = m_minv;

        Color4f color;
        color.ColorRamp(m_maxv,m_minv,v);
        m_mesh->m_vertexColor.push_back(color);
    }
}

void ColorMeshModel::SetVertexColor(std::vector<float> & values)
{
    int vn = m_mesh->vn;

    m_mesh->m_vertexColor.clear();
    m_func = values;

    AnalizeData(values,m_minv,m_maxv);

    float offset = (m_maxv-m_minv)*0.3;

    m_minv+= (m_maxv-m_minv)*0.1;
    m_maxv-= (m_maxv-m_minv)*0.31;

    m_minv = -1.661262;
    m_maxv = 0.95;

    for(int i=0;i<vn;i++)
    {
        float v = values[i];

        if(v>m_maxv)v = m_maxv;
        if(v<m_minv)v = m_minv;

        Color4f color;
        color.ColorRamp(m_maxv,m_minv,v);
        m_mesh->m_vertexColor.push_back(color);
    }
}

void ColorMeshModel::MinMaxColor()
{
    int vn = m_mesh->vn;

    float minq,maxq;

    minq = FLT_MAX;
    maxq = -FLT_MAX;
    for(int i=0;i<vn;i++)
    {
        if( fabs(m_func[i]) < minq) minq =fabs(m_func[i]);
        if( fabs(m_func[i]) > maxq)  maxq =fabs(m_func[i]);
    }

    m_minv = minq;
    m_maxv = maxq;

    float offset = (m_maxv-m_minv)*0.1;

    m_minv-=5*offset;

    for(int i=0;i<vn;i++)
    {
        float v = m_func[i];
        Color4f color;
        color.ColorRamp(m_maxv,m_minv,v);
        m_mesh->m_vertexColor.push_back(color);
    }
}   
void ColorMeshModel::HistogramColor()
{

    AnalizeData(m_func,m_minv,m_maxv);

    float offset = (m_maxv-m_minv)*0.1;

    m_maxv-=offset;

    int vn = m_mesh->vn;

    for(int i=0;i<vn;i++)
    {
        float v = m_func[i];

        if(v>m_maxv)v = m_maxv;
        if(v<m_minv)v = m_minv;

        Color4f color;
        color.ColorRamp(m_maxv,m_minv,v);
        m_mesh->m_vertexColor.push_back(color);
    }
}

void ColorMeshModel::AnalizeData(std::vector<float> & values,float & minv,float & maxv)
{
    HierarchyHistogram hist;

    hist.SetData(values);

    hist.GetSaliencyRange(minv,maxv);
}
