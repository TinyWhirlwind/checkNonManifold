#ifndef  _FACE_
#define  _FACE_

/*        Face 类

该类定义了各类几何数据中的面单元  

定义了面拓扑结构             

*/


#include "GeometryData.h"
#include "Vertex.h"
#include "Edge.h"

namespace sn3DCore
{
	class FaceTopology
	{
	public:

		int m_ffp[3]; // 关于该面的三条边共边链表中的下一个面   
		int m_ffi[3];    // 上述下一面中该边在该面中的索引
		int m_vfp[3]; // 关于该面的三个点的共点链表中的下一个面
		int m_vfi[3];    // 上述下一个面中该点在该面中的索引 

		FaceTopology() {m_vfp[0]=m_vfp[1]=m_vfp[2]=-1;
		m_ffp[0]=m_ffp[1]=m_ffp[2]=-1;}
	};

	class Face
	{

	public:	

		typedef float ScalarType;
		//////////////////////////////  面属性访问 ///////////////////////////////////

		Vertex *  V(int j)              //   获取第j个顶点顶点  
		{return &m_data->m_vertices[m_v[j]];}

		const Vertex* cV(int j)
		{return &m_data->m_vertices[m_v[j]];}

		Vertex& refV(int j)
		{return m_data->m_vertices[m_v[j]];}

		Vertex *  V1( int j )            //   获取第j个顶点的后一个顶点
		{ return &m_data->m_vertices[m_v[(j+1)%3]];}

		Vertex *  V2( int j )            //   获取第j个顶点的后两个顶点
		{ return &m_data->m_vertices[m_v[(j+2)%3]];}

		void SetV(int j,int index)        //   设置顶点索引
		{m_v[j] = index;}

		int  E(int j){return m_data->m_pEdges[m_index*3+j];}

		int & Flag() {return m_flag;}     //  获取标志

		int & Index(){return m_index;}    //  获得面索引

		Point3f & P(int j )               //   获得第j个顶点的几何坐标
		{return m_data->m_vertices[m_v[j]].m_coord;}

		Point3f & N()                    //   获取面法向
		{return m_data->m_faceNormal[m_index];}

		Point3f Color()                  //   获取面颜色
		{return Point3f(m_data->m_faceColor[m_index].X(),m_data->m_faceColor[m_index].Y(),m_data->m_faceColor[m_index].Z());}

		void SetColor(Point3f _color)    //   设置面颜色
		{m_data->m_faceColor[m_index].X() = _color.X();m_data->m_faceColor[m_index].Y() = _color.Y();
		m_data->m_faceColor[m_index].Z() = _color.Z();}

		Point4f ColorA()                 //   获取带alpha值的面颜色
		{return Point4f(m_data->m_faceColor[m_index].X(), m_data->m_faceColor[m_index].Y(), m_data->m_faceColor[m_index].Z(), m_data->m_faceColor[m_index].W());}

		void SetColor(Point4f _color)    //   设置带alpha值的面颜色
		{m_data->m_faceColor[m_index] = Color4f(_color.X(), _color.Y(), _color.Z(), _color.W());}

		Point2f  TexCoord(int j)                      //  获取纹理坐标
		{return m_data->m_faceTexCoord[m_index*3+j];}

		void     SetTexCoord(Point2f _tex,int j)      //  设置纹理坐标
		{m_data->m_faceTexCoord[m_index*3+j] = _tex;}

		Face* FFp(int j)                //   获取面面拓扑指针
		{
		  if(m_data->m_faceTopology[m_index].m_ffp[j]==-1)
				return 0;
		  else
			return &(m_data->m_faces[m_data->m_faceTopology[m_index].m_ffp[j]]);
		}

		void  SetFFp(int j,Face * fi)
		{
		  if(fi==0)m_data->m_faceTopology[m_index].m_vfp[j]=-1;
			else
			m_data->m_faceTopology[m_index].m_ffp[j] = fi->Index();
	     }

		int &FFi(int j)                  //   获取面面拓扑索引
		{return m_data->m_faceTopology[m_index].m_ffi[j];}

		Face* VFp(int j)                //   获取点面拓扑指针
		{
			if(m_data->m_faceTopology[m_index].m_vfp[j]==-1)
				return 0;
			else
			return &(m_data->m_faces[m_data->m_faceTopology[m_index].m_vfp[j]]);}

		void  SetVFp(int j,Face * fi)
		{
			if(fi==0)m_data->m_faceTopology[m_index].m_vfp[j]=-1;
			else
			m_data->m_faceTopology[m_index].m_vfp[j] = fi->Index();
		}

		int &VFi(int j)                  //   获取点面拓扑索引
		{return m_data->m_faceTopology[m_index].m_vfi[j];}	

		int & Identity(){return m_data->m_FaceIdentity[m_index];}

	    int & Identity1(){return m_data->m_FaceIdentity1[m_index];}

	    int & Identity2(){return m_data->m_FaceIdentity2[m_index];}

		void InitIMark()                  //  初始化整数标志
		{m_data->m_faceMark[m_index] = 0;}

		int &IMark()                      //  获取整数标志
		{return m_data->m_faceMark[m_index];}

		void SetGeometry(sn3DGeometryData * data){m_data =  data;}  //  设置几何数据对象	

		////////////////////////////    标志位   /////////////////////////////////

		bool  IsD()  {return (this->Flag() & DELETED) != 0;}	// 是否删除
		void  SetD() {this->Flag() |= DELETED;}						
		void  ClearD() {this->Flag() &=(~DELETED);}				

		bool  IsV() {return (this->Flag() & VISITED) != 0;}  	// 访问位
		void  SetV(){this->Flag() |= VISITED;}						
		void  ClearV()	{this->Flag() &= ~VISITED;}				

		bool  IsM() {return (this->Flag() & MODIFIED) != 0;}	// 修改位
		void  SetM(){this->Flag() |= MODIFIED;}						
		void  ClearM(){this->Flag() &= ~MODIFIED;}					

		bool  IsM1() {return (this->Flag() & MODIFIED1) != 0;}	// 修改位
		void  SetM1(){this->Flag() |= MODIFIED1;}						
		void  ClearM1(){this->Flag() &= ~MODIFIED1;}				

		bool  IsM2() {return (this->Flag() & MODIFIED2) != 0;}	// 修改位
		void  SetM2(){this->Flag() |= MODIFIED2;}					
		void  ClearM2(){this->Flag() &= ~MODIFIED2;}			

		bool  IsS() {return (this->Flag() & SELECTED) != 0;}	//选择位
		void  SetS() {this->Flag() |= SELECTED;}						
		void  ClearS(){this->Flag() &=(~SELECTED);}					

		bool IsB(int i)  {return (this->Flag() & (BORDER0<<i)) != 0;} // 边界位
		void SetB(int i)  {this->Flag() |=(BORDER0<<i);}		
		void ClearB(int i)	{this->Flag() &= (~(BORDER0<<i));}	

		bool IsR()  {return (this->Flag() & NOTREAD) == 0;}	//读取位
		void SetR() {this->Flag() &=(~NOTREAD);}					
		void ClearR() {this->Flag() |=NOTREAD;}						

		bool IsW()  {return (this->Flag() & NOTWRITE)== 0;}	//写入位
		void SetW() {this->Flag() &=(~NOTWRITE);}				
		void ClearW() {this->Flag() |=NOTWRITE;}			

		bool  IsMultPolygon() {return (this->Flag() & MULTPOLYGON) != 0;}	// 修改位
		void  SetMultPolygon(){this->Flag() |= MULTPOLYGON;}						
		void  ClearMultPolygon(){this->Flag() &= ~MULTPOLYGON;}		

		bool  IsFeature() { return (this->Flag() & FEATURE) != 0; }	// 修改位
		void  SetFeature() { this->Flag() |= FEATURE; }
		void  ClearFeature() { this->Flag() &= ~FEATURE; }

		void  SetFlag(int flag) {this->Flag()=flag;}				// 设置标志	
		void  ClearFlag() {this->Flag()=0;}							// 清除标志


		enum { 		
			DELETED     = 0x01,		   // 删除位
			VISITED     = 0x02,        // 访问位
			SELECTED    = 0x04,        // 选择位		
			MODIFIED    = 0x08,        // 修改位  	
			MODIFIED1   = 0x10,        // 修改位
			MODIFIED2   = 0x40,        // 修改志
			BORDER0     = 0x80,        // 边界位
			NOTWRITE    = 0x1000,	   // 写入位
			NOTREAD     = 0x2000,	   // 读取位
			MULTPOLYGON = 0x4000,       // 多边形
			FEATURE = 0x8000       // 特征
		};

	public:

		Face(){m_data = 0;m_flag=0;m_index=0;m_v[0]=0;m_v[1]=0;m_v[2]=0;}

		Face(sn3DGeometryData * data,int index){m_data=data;m_flag=0;m_index=index;m_v[0]=0;m_v[1]=0;m_v[2]=0;}

		Face(int p1,int p2, int p3,sn3DGeometryData * data,int index)
		{m_v[0]=p1;m_v[1]=p2;m_v[2]=p3;m_flag=0;m_index=index;m_data = data;}

		Face  operator = (Face face)
		{
			m_flag = face.m_flag;
			m_data = face.m_data;
			m_v[0] = face.m_v[0];
			m_v[1] = face.m_v[1];
			m_v[2] = face.m_v[2];
			return face;
		}
	public:

		sn3DGeometryData * m_data; // 几何数据对象

		int m_index;				// 面索引序号

		int  m_v[3];               // 顶点集

		int  m_flag;               // 面标志位
	};
}
#endif