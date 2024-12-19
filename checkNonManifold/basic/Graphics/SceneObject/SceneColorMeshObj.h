#ifndef COLOR_MESH_OBJ
#define COLOR_MESH_OBJ

/*************************************************
 ColorMeshModel 类 
 模型颜色绘制，伪彩图
 主要包括: 
 1.几何数据, 点、面、及其拓扑结构
 2.变换数据,平移向量、旋转四元数、齐次变换矩阵
 3.绘制对象                                     
***************************************************/

#include "SceneObject/SceneMeshObj.h"
#include "Common/Histogram.h"

class ColorMeshModel : public sn3DMeshModel
{
public:
   void   SetVertexColor(sn3DMeshModel * model,std::vector<float> & values);

   void   SetVertexColor(std::vector<float> & values);

   void  MinMaxColor();

   void  HistogramColor();

public:
   ColorMeshModel();
   ColorMeshModel(sn3DMeshModel * model,std::vector<float> & func);

   float m_minv;
   float m_maxv;

   std::vector<Point3f> m_field;
   std::vector<Point3f> m_field1;

   std::vector<float> m_kmin;
   std::vector<float> m_kmax;

public:

   void AnalizeData(std::vector<float> & values,float & minv,float & maxv);  // 分析数据

   std::vector<float> m_func;

   unsigned int m_list;
   Histogramf m_distribution;
};
#endif