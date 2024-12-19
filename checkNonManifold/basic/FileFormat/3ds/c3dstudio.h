#ifndef _C3DSTUDIO_
#define _C3DSTUDIO_

#include<list>
#include <stdio.h>
#include<string.h>
#include"3dsfile.h"
#include"../BaseReader.h"
#include"../../../Common/Point3.h"

using namespace std;

#define	POINT_LIGHT			1
#define SPOT_LIGHT			2
#define	DIRECTIONAL_LIGHT	3


class C3DStudio 
{
public:

  class Light 
 {
  public:
     	
    string	 m_Name;//灯光名称
	unsigned int	 m_LightGL; //OpenGL 光源序号
	int		 m_Type;  //光源类型
	bool	 m_Enable;	
	Point3f  m_Color;	//光源颜色
	float    m_C1, m_C2, m_C3;	//衰减系数
	Point3f  m_Location;	//光源位置	
    float	 m_NearRange;/* 光线追踪支持的衰减参数 */	
	float    m_FarRange;
	bool	 m_AttnOn;
	Point3f  m_Target;//聚光灯目标位置
	float	 m_Falloff; //GL_SPOT_CUTOFF
	float    m_Hotspot; //GL_SPOT_EXPONENT 
  };

 class Camera 
 {
  public:
	string   m_Name;	
	Point3f m_Eye;//像机位置
	Point3f m_Target;//像机目标
	Point3f m_Up;//上向量
    float m_ScreenWidth;//像机宽度(等于绘制窗口宽度，如需照相机投影平面宽度可计算)
	float m_ScreenHeight;//像机高度(等于绘制窗口高度)
	float m_Focal_length; //焦距?不太清楚照相机的概念
	float m_Fov;	//视景体仰角(角度非弧度)
	float m_RangeNear; //视景体近裁剪平面，实际读取的是3ds文件的打气环境影响值!!
	float m_RangeFar; //视景体远裁剪平面
	float m_Roll;	//3ds的一个角度，暂不用
  };

  class Surface
  {
   public:
    
	Surface(){m_Ambient = m_Specular = m_Reflected = m_Transmitted = m_Diffuse = Point3f(0.0,0.0,0.0);m_Index=1.0;}  
	string	  m_Name;			//材质名称
	string    m_BumpName;    //凹凸纹理文件名
	string    m_TextureName; //纹理文件名
    float	  m_Index;		//相对折射率
	Point3f	  m_Ambient;		
	Point3f	  m_Diffuse;		 
	Point3f	  m_Specular;		
	float	  m_Shininess;		// specularity factor
	float	  m_Shin_strength;   // the presence of the specular color in the hilight <not variable>
	Point3f	  m_Transmitted;	//透射颜色
	Point3f	  m_Reflected;	//反射颜色		
	short int m_PolygonMode;    
	short int m_ShadeModel;       
	bool	  m_TwoSided;
   };

  struct chunk3ds 
  {
   unsigned short int  tag;		
   unsigned long int   len;        
  };

	bool   Read3DSFile(std::string & file_name,BaseImporter * _bi);

	bool   Is3DSFile();

	FILE * fp;
    string 			m_Filename;
	float			m_Scale;
	unsigned long	m_Version;
	Point3f	backcolor;
	Point3f	ambient;

public:

	C3DStudio();

	~C3DStudio();

   std::list<Light *>      m_lightlist;
   std::list<Camera *>     m_cameralist;
   std::list<Surface *>    m_surfacelist;

private:

   BaseImporter * m_importer;
   Light   * newLight;
   Camera  * newCamera;
   Surface * newSurface;

   bool ReadMain(long Start, long Size);
   bool ReadEdit(long Start, long Size);
   bool ReadMaterial(long Start, long Size);
   bool ReadObject(long Start, long Size);
   bool ReadTriMesh(long Start, long Size);
   bool ReadLight( long Start, long Size);
   bool ReadCamera( long Start, long Size);
   bool ReadVertexArray();
   bool ReadFaceArray();
   bool ReadTexArray();
   bool ReadMeshMaterial();
   bool ReadSpotLight(long Start, long Size);
   bool ReadTextureMap(long Start, long Size,int type);
   bool ReadColor(Point3f & color);
   bool ReadPercentage(float &value);
   char * ReadAndAllocString3ds(unsigned int len);
		
};

#endif