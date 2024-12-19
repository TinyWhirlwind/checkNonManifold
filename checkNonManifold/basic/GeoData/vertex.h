#ifndef  _VERTEX_
#define  _VERTEX_

/*  Vertex类 

定义了各类几何数据对象中的顶点对象

*/

#include "GeometryData.h"

namespace sn3DCore
{
class Face;
class VertexTopology
{
public:
    int m_face;  // 该顶点的一个面,作为共点面链表的首面   
	int m_index;   // 顶点在该面中的索引

	VertexTopology()
	{ m_face=-1; m_index=-1;}
};

class Vertex
{
public:

	typedef float ScalarType;
////////////////////    顶点属性访问    ////////////////////////

    int  &  Flag() {return m_flag;}	          //  获取标志位	

	int  &  Index(){return m_index;}   //  获得顶点索引

	Point3f& P()	{ return m_coord; }
	
	Point3f& N()	{ if(m_data!=0)return m_data->m_vertexNormal[m_index];
	else return _n;} //  获取法向

	Color4f& C()
	{
		return m_color;// m_data->m_vertexColor[m_index];
	} // 获取颜色

	Color4f& Color()
	{
		return m_color;
	}

	//Color4f & C() {return m_data->m_vertexColor[m_index];} // 获取颜色

		//Point3f Color()                          //  获取颜色
		//{return Point3f(m_data->m_vertexColor[m_index].X(),m_data->m_vertexColor[m_index].Y(),m_data->m_vertexColor[m_index].Z());}

		//void    SetColor(Point3f _color)          //  设置颜色
		//{m_data->m_vertexColor[m_index].X() = _color.X();m_data->m_vertexColor[m_index].Y() = _color.Y();m_data->m_vertexColor[m_index].Z() = _color.Z();}

		//Point4f  ColorA()                         //  获取带alpha值颜色  
		//{return m_data->m_vertexColor[m_index];}

		//void    SetColor(Point4f _color)          //  设置带alpha值颜色
		//{m_data->m_vertexColor[m_index] = _color;}

		Point2f  TexCoord()                       //  获取纹理坐标
		{return m_data->m_vertexTexCoord[m_index];}

		void     SetTexCoord(Point2f _tex)        //  设置纹理坐标
		{m_data->m_vertexTexCoord[m_index] = _tex;}

		Face *  VFp()                            //  获取点面拓扑指针
		{
			if(m_data->m_vertexTopology[m_index].m_face==-1)
			return 0;
			else
			return &(m_data->m_faces[m_data->m_vertexTopology[m_index].m_face]);}

		void  SetVFp(int fi)
		{m_data->m_vertexTopology[m_index].m_face = fi; }

		int & VFi()                               //  获取点面拓扑索引
		{return m_data->m_vertexTopology[m_index].m_index;}

		int & IMark()                             //  获取整数标志位
		{return m_data->m_vertexMark[m_index];}

		void InitIMark()                          //  初始化整数标志位
		{m_data->m_vertexMark[m_index] = 0;}

		int & Identity(){return m_data->m_VertexIdentity[m_index];}

		void SetGeometry(sn3DGeometryData * data){m_data =  data;}  //  设置几何数据对象

	public:

		//////////////////////////////////  标志位   ///////////////////////////////

		bool IsD()    {return (this->Flag() & DELETED) != 0;}	    // 删除位
		void SetD()   {this->Flag() |= DELETED;}				    	
		void ClearD() {this->Flag() &= (~DELETED);}				   

		bool IsR()    {return (m_flag & NOTREAD) == 0;}			    // 读取位
		void SetR()   {m_flag &=(~NOTREAD);}					    
		void ClearR() {m_flag |=NOTREAD;}						    

		bool IsB()      {return (this->Flag() & BORDER) != 0;}		// 边界位
		void SetB()		{this->Flag() |=BORDER;}                    
		void ClearB()	{this->Flag() &=~BORDER;}                   

		bool IsS()      {return (this->Flag() & SELECTED) != 0;}	// 选择位
		void SetS()		{this->Flag() |=SELECTED;}					
		void ClearS()	{this->Flag() &= ~SELECTED;}				

		bool IsV()   {return (this->Flag() & VISITED) != 0;}	// 访问位
		void SetV()		{this->Flag() |= VISITED;}						
		void ClearV()	{this->Flag() &= ~VISITED;}					

		bool IsM() {return (this->Flag() & MODIFIED) != 0;}	    // 修改位
		void SetM(){this->Flag() |= MODIFIED;}						
		void ClearM(){this->Flag() &= ~MODIFIED;}					

		bool IsM1() {return (this->Flag() & MODIFIED1) != 0;}	// 修改位
		void SetM1(){this->Flag() |= MODIFIED1;}					
		void ClearM1(){this->Flag() &= ~MODIFIED1;}				

		bool IsW()  {return (this->Flag() & NOTWRITE)== 0;}	    // 写入位
		void ClearW() {this->Flag() &=(~NOTWRITE);}					
		void SetW() {this->Flag() |=NOTWRITE;}			


		bool IsSelectMultiLayer()  {return (this->Flag() & SelectMultiLayer)!= 0;}	  
		void ClearSelectMultiLayer() {this->Flag() &=(~SelectMultiLayer);}					
		void SetSelectMultiLayer() {this->Flag() |=SelectMultiLayer;}	

		bool IsSelectAccurate()  {return (this->Flag() & SelectAccurate)!= 0;}	  
		void ClearSelectAccurate() {this->Flag() &=(~SelectAccurate);}					
		void SetSelectAccurate() {this->Flag() |=SelectAccurate;}	

		bool IsSelectEllipse()  {return (this->Flag() & SelectEllipse)!= 0;}	  
		void ClearSelectEllipse() {this->Flag() &=(~SelectEllipse);}					
		void SetSelectEllipse() {this->Flag() |=SelectEllipse;}	

		bool IsH()  {return (this->Flag() & HIDE)!= 0;}	  
		void ClearH() {this->Flag() &=(~HIDE);}					
		void SetH() {this->Flag() |=HIDE;}	

		bool IsReserved()  {return (this->Flag() & RESERVED)!= 0;}	  
		void ClearReserved() {this->Flag() &=(~RESERVED);}					
		void SetReserved() {this->Flag() |=RESERVED;}	

		bool IsImage()  {return (this->Flag() & IMAGE)!= 0;}	   
		void ClearImage() {this->Flag() &=(~IMAGE);}					
		void SetImage() {this->Flag() |=IMAGE;}	

		bool IsReduce()  {return (this->Flag() & REDUCE)!= 0;}	    
		void ClearReduce() {this->Flag() &=(~REDUCE);}					
		void SetReduce() {this->Flag() |=REDUCE;}	

		bool IsVertexEdge()  {return (this->Flag() & VERTEXEDGE)!= 0;}	   
		void ClearVertexEdge() {this->Flag() &=(~VERTEXEDGE);}					
		void SetVertexEdge() {this->Flag() |=VERTEXEDGE;}	

		bool IsVertexSplit()  {return (this->Flag() & VERTEXSPLIT)!= 0;}	   
		void ClearVertexSplit() {this->Flag() &=(~VERTEXSPLIT);}					
		void SetVertexSplit() {this->Flag() |=VERTEXSPLIT;}	

		bool IsVertexOverlap()  {return (this->Flag() & VERTEXOVERLAP)!= 0;}	  
		void ClearVertexOverlap() {this->Flag() &=(~VERTEXOVERLAP);}					
		void SetVertexOverlap() {this->Flag() |=VERTEXOVERLAP;}	

		bool IsMapped()  {return (this->Flag() & MAPPED)!= 0;}	  
		void ClearMapped() {this->Flag() &=(~MAPPED);}					
		void SetMapped() {this->Flag() |=MAPPED;}	

		bool IsTriangleOverlap()  {return (this->Flag() & TRIANGLEOVERLAP)!= 0;}	  
		void ClearTriangleOverlap() {this->Flag() &=(~TRIANGLEOVERLAP);}					
		void SetTriangleOverlap() {this->Flag() |=TRIANGLEOVERLAP;}	

		bool IsSelectNormal()  {return (this->Flag() & SELECTNORMAL)!= 0;}	   
		void ClearSelectNormal() {this->Flag() &=(~SELECTNORMAL);}					
		void SetSelectNormal() {this->Flag() |=SELECTNORMAL;}	

		bool IsLayer()  {return (this->Flag() & LAYER)!= 0;}	   
		void ClearLayer() {this->Flag() &=(~LAYER);}					
		void SetLayer() {this->Flag() |=LAYER;}	

		bool IsSV()   {return (this->Flag() & SMOOTHV) != 0;}
	    void SetSV()		{this->Flag() |=SMOOTHV;}
	    void ClearSV()	{this->Flag() &=~SMOOTHV;}

		bool IsRW() {return (this->Flag() & (NOTREAD | NOTWRITE)) == 0;}

		bool IsUserBit(int userBit){return (this->Flag() & userBit)!= 0;} //用户标志位
		void SetUserBit(int userBit){this->Flag() |=userBit;}		
		void ClearUserBit(int userBit){this->Flag() &= (~userBit);}  

		bool  IsFeature() { return (this->Flag() & FEATURE) != 0; }	// 修改位
		void  SetFeature() { this->Flag() |= FEATURE; }
		void  ClearFeature() { this->Flag() &= ~FEATURE; }

		void SetFlag(int flag) {this->Flag()=flag;}	          // 设置标志
		void ClearFlag() {this->Flag()=0;}                    // 清除标志

		static int &LastBitFlag()                             // 空闲标志
		{
			static int b = USER0;
			return b;
		}

		static inline int NewBitFlag()                        //分配新标志
		{
			LastBitFlag()=LastBitFlag()<<1;
			return LastBitFlag();
		}

		static inline bool DeleteBitFlag(int bitval)          // 清除新标志
		{	
			if(LastBitFlag()==bitval) 
			{
				LastBitFlag()= LastBitFlag()>>1;
				return true;
			}
			return false;
		}

		enum { 
			DELETED          = 0x00000001,		// 删除位
			VISITED          = 0x00000002,		// 访问位
			MODIFIED         = 0x00000004,		// 修改位		
			MODIFIED1        = 0x00000008,      // 修改位
			NOTWRITE         = 0x00000010,	    // 写入位
			SELECTED         = 0x00000020,	    // 选择位 
			NOTREAD          = 0x00000040,	    // 读取位 
			BORDER           = 0x00000080,		// 边界位
			SelectMultiLayer = 0x00000100,
			SelectAccurate   = 0x00000200,
			SelectEllipse    = 0x00000400,
			HIDE             = 0x00000800,
			RESERVED         = 0x00001000,
			IMAGE            = 0x00002000,
			REDUCE           = 0x00004000,
			VERTEXEDGE       = 0x00008000,
			VERTEXSPLIT      = 0x00010000,
			VERTEXOVERLAP    = 0x00020000,
			MAPPED           = 0x00040000,
			TRIANGLEOVERLAP  = 0x00080000,
			SELECTNORMAL     = 0x00100000,
			LAYER            = 0x00200000,
			SMOOTHV          = 0x00400000,
			FEATURE          = 0x00800000,
			USER0            = 0x00001000		// 用户位 
		};

	public:

		Vertex()
		{
			m_data = NULL;
			m_flag=0;
			m_index = 0;
			m_coord = Point3f(0.0,0.0,0.0);
			m_color = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
		}

		Vertex(int index)
		{
			m_data = NULL;
			m_flag=0; 
			m_index = index; 
			m_coord = Point3f(0.0,0.0,0.0);
			m_color = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
		}

		Vertex(float x, float y, float z, int index,sn3DGeometryData * data)
		{
			(void)data;
			m_data = NULL;
			m_coord.X()=x;
			m_coord.Y()=y;
			m_coord.Z()=z;
			m_flag=0;
			m_index=index;
			m_color = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
		Vertex(const Point3f & vpos,int index,sn3DGeometryData * data)
		{
			m_data = data;
			m_index=index;
			m_coord=vpos;
			m_flag=0;
			m_color = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
		}

		Vertex operator = (Vertex v)
		{
			m_data = v.m_data;	
			m_coord  = v.m_coord;
			m_flag = v.m_flag;
			m_color = v.m_color;
			return v; 
		}

		Vertex(const Vertex& v)
		{
			m_data = v.m_data;
			m_coord = v.m_coord;
			m_flag = v.m_flag;
			m_color = v.m_color;
			_n = v._n;
			m_index = v.m_index;
		}

public:

	Point3f  m_coord;			// 顶点三维坐标
	Point3f _n;
	Color4f m_color = Color4f(1.0f, 1.0f, 1.0f, 1.0f); 
public:
	sn3DGeometryData * m_data;	// 几何数据对象
	int  m_index;				// 顶点索引序号，按顶点数组递增顺序
	int  m_flag;				// 顶点标志位
};
}
#endif
