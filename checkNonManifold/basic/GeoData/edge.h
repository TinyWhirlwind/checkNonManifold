#ifndef  _EDGE_
#define  _EDGE_

/*******************************
  Edge 类

  定义了各类几何数据中的边对象

********************************/

namespace sn3DCore
{
class Edge
{
public:			
////////////////////////////   属性访问  ///////////////////////////////////

    int & V( const int j){return v[j];}              // 获取顶点

	int & F( const int j){return f[j];}              // 获取顶点

	int  &  Index(){return m_index;}   //  获得顶点索引

	int m_id;

public:
//////////////////////////////////  标志位 ///////////////////////////////////

    bool IsD()                             // 删除位 
	{return (_flags & DELETED) != 0;}
	void SetD()	  { _flags |=DELETED;}
	void ClearD() { _flags &= (~DELETED);}

	bool IsS()                             // 选择位 
	{return (_flags & SELECTED) != 0;}
	void SetS()	{_flags |=SELECTED;}
	void ClearS(){_flags &= (~SELECTED);}

	bool IsV()                             // 选择位 
	{return (_flags & VISITED) != 0;}
	void SetV()	{_flags |=VISITED;}
	void ClearV(){_flags &= (~VISITED);}

	bool IsM() 
	{return (_flags & MODIFIED) != 0;}	   // 修改位
	void SetM(){_flags |= MODIFIED;}						
	void ClearM(){_flags &= ~MODIFIED;}		

	bool IsB()                             // 边界位
	{return (_flags & (BORDER)) != 0;}    
	void SetB()	{_flags |=(BORDER);}
	void ClearB(){_flags &= (~(BORDER));}

	bool IsUserBit(int userBit)            // 用户位
	{ return (_flags & userBit) != 0;}
	void SetUserBit(int userBit){_flags |=userBit; }
	void ClearUserBit(int userBit){_flags &= (~userBit);}

	static int &LastBitFlag()
	{
	  static int b =USER0;
	  return b;
	}
	static inline int NewBitFlag()
	{
	  LastBitFlag()=LastBitFlag()<<1;
	  return LastBitFlag();
	}
	static inline bool DeleteBitFlag(int bitval)
	{	
	  if(LastBitFlag()==bitval)
	  {
	   LastBitFlag()= LastBitFlag()>>1;
	   return true;
	  }
	  return false;
	}

	int & Flags (){return _flags;}  // 获取标志位

	void ClearFlags() {_flags=0;}   // 清除标志位


	enum{
	  DELETED     = 0x00000001,		// 删除位
	  NOTREAD     = 0x00000002,		// 读取位
	  NOTWRITE    = 0x00000004,		// 写入位
	  SELECTED    = 0x00000008,		// 选择位
	  VISITED     = 0x00000010,		// 访问位
	  MODIFIED    = 0x00000020,		// 修改位
	  BORDER      = 0x00000040,     // 边界位
	  USER0       = 0x00000080      // 用户位
	};

public:

    Edge(){}

	Edge(int v0,int v1){v[0]=v0;v[1]=v1;}
	
private:

	int  v[2];    // 端点
    int  f[2];    // 邻面
	int  m_index;				// 顶点索引序号，按顶点数组递增顺序
	int  _flags;  // 标志位	
	
	
};
}
#endif