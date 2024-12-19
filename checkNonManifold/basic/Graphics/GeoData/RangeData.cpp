#include "RangeData.h"
#include <float.h>

namespace sn3DCore
{
	sn3DRangeData::sn3DRangeData()
	{
		vn = 0;		//  点数
		fn = 0;		//	面个数
		w  = 0;		//  深度图宽
		h  = 0;		//  深度图高
		SetMeshParameter(10.0f, 1.2f, 5.0f);
	}

	sn3DRangeData::~sn3DRangeData()
	{
		ReleaseData(true);
	}

	bool sn3DRangeData::SetPoints(int width, int height, int* ids, int npt, double *x, double *y, double *z)
	{
		ReleaseData(true);
		vn = npt;
		fn = 0;
		w = width;
		h = height;

		m_maps.Create(w,h,ids);

		m_pnts.resize(vn);
		for (int i=0; i<npt; ++i)
		{
			m_pnts[i][0] = (float)x[i];
			m_pnts[i][1] = (float)y[i];
			m_pnts[i][2] = (float)z[i];
		}
		return true;
	}

	bool sn3DRangeData::SetMap(int width, int height, int *ids)
	{
		ReleaseData(true);
		vn = 0;
		fn = 0;
		w = width;
		h = height;
		m_maps.Create(w,h,ids);
		return true;
	}
	
	void sn3DRangeData::SetMeshParameter(float maxDepth, float maxDis, float minAngleTri)
	{
		m_maxDepth2	= maxDepth*maxDepth;
		m_maxDis2   = maxDis*maxDis;

		m_minAngleTri2	= float ( cos( minAngleTri*M_PI/180.0) );
		m_minAngleTri2	= m_minAngleTri2 * m_minAngleTri2;
		return ;
	}

	void sn3DRangeData::ReleaseData( bool realeseAll )
	{
		std::vector<Point3f>().swap(m_pntNorm);
		std::vector<Triangle>().swap(m_faces);
		std::vector<Point3f>().swap(m_pntColor);
		std::vector<float>().swap(m_pntDist);
		if( realeseAll )
		{
			std::vector<Point3f>().swap(m_pnts);
			m_maps.Release();
		}
	}

	void sn3DRangeData::ReleaseNorm()
	{
		std::vector<Point3f>().swap(m_pntNorm);
	}

	void sn3DRangeData::ReleaseFace()
	{
		std::vector<Triangle>().swap(m_faces);
	}

	void sn3DRangeData::ReleaseDist()
	{
		std::vector<float>().swap(m_pntDist);
	}

	void sn3DRangeData::ReleaseColor()
	{
		std::vector<Point3f>().swap(m_pntColor);
	}

	bool sn3DRangeData::GetBox(Point3f &cnt, Point3f &max,Point3f &min)
	{
		cnt = m_center;
		max = m_box.max;
		min = m_box.min;
		return true;
	}

	void sn3DRangeData::UpdateBox()
	{
		GetCenter( m_center, m_box.max, m_box.min );
	}

	bool sn3DRangeData::GetCenter( Point3f &cnt, Point3f &max,Point3f &min )
	{
		cnt[0] = cnt[1] = cnt[2] = 0.0f;
		min[0] = min[1] = min[2] = FLT_MAX;
		max[0] = max[1] = max[2] = -1.0f * FLT_MAX;
		if(vn<=0)
			return false;

		int count = 0;
		for (int i=0; i<h; i++)
		{
			for (int j=0; j<w; j++)
			{
				int id = m_maps[i][j];
				if( id >= 0 && id < vn)
				{
					cnt += m_pnts[id];
					for (int k=0; k<3; k++)
					{
						if( min[k] > m_pnts[id][k] )
							min[k] = m_pnts[id][k];
						if( max[k] < m_pnts[id][k]  )
							max[k] = m_pnts[id][k];
					}
					count++;
				} // if id>=0
			}// for 
		}
		if( count > 0 )
		{
			cnt[0] /= count;
			cnt[1] /= count;
			cnt[2] /= count;
		}
		return true;
	}
	
	bool sn3DRangeData::BuildNormal()
	{
		if( vn <=0 )
			return false;

		MatrixData<char> NbCount(w,h,NULL);
		char init = 0;
		NbCount.Fill(init);

		//           上左 上中 上右 右 下右 下中 下左  左
		int NBy[8] = { -1,	-1,	-1,	0,	1,	1,	 1,	 0};
		int NBx[8] = { -1,	 0,	 1, 1,	1,	0,	-1,	-1};
	
		// 用NbCount记录他邻居点
		for (int i=1;i<h-1; ++i)
		{
			for (int j=1;j<w-1; ++j)
			{		
				if( m_maps[i][j]<0 )
					continue;
				for (int t=0;t<8; ++t)
					NbCount[NBy[t]+i][NBx[t]+j]++;			
			}
		}

		m_pntNorm.resize(vn);
		Point3f normZero(0.0f,0.0f,0.0f);	
		for (int i=0; i<vn; i++)
			m_pntNorm[i] = normZero;

		// 通过8个邻居计算法向量
		for (int i=1; i<h-1; ++i)
		{
			for(int j=1; j<w-1; ++j)
			{
				if( m_maps[i][j]<0 || NbCount[i][j]<8 )
					continue;

				Point3f*   norm = &m_pntNorm[m_maps[i][j]];
				const Point3f* p= &m_pnts[ m_maps[NBy[7]+i][NBx[7]+j] ];

				for (int t=0; t<8; ++t)
				{
					const Point3f *c = &m_pnts[m_maps[NBy[t]+i][NBx[t]+j]];
					norm->x += (p->z + c->z) * (p->y - c->y);
					norm->y += (p->x + c->x) * (p->z - c->z);
					norm->z += (p->y + c->y) * (p->x - c->x);
					p = c;
				}
				(*norm) *= -1.0f;
				norm->Normalize();
			}
		}

		// 无法计算法向量点,法向量设置为（0.0 0.0 1.0）
		Point3f defaultNorm(0.424264f,0.424264f,0.8f);
		for (int i=0; i<h; i++)
		{
			for(int j=0; j<w; j++)
			{
				if( m_maps[i][j]<0)
					continue;
				if( NbCount[i][j]== 8 )
					continue;
				int count = 0;
				int ptId = m_maps[i][j];
				
				for (int t=0; t<8; t++)
				{ 
					int ii = i+NBy[t];
					int jj = j+NBx[t];
					if(ii>=0 && ii<h && jj>=0 && jj<w && NbCount[ii][jj] ==8 )
					{
						int iid = m_maps[ii][jj];
						if( iid<0 )
							continue;
						m_pntNorm[ptId] += m_pntNorm[iid];
						count++;
					}
				}
				if( count == 0 )
					m_pntNorm[ptId] = defaultNorm;
				else
					m_pntNorm[ptId].Normalize();
			}
		}
		return true;
	}

	void sn3DRangeData::BuildMesh( )
	{
		if( w<2 || h<2 || m_pntNorm.size()<=0 )
			return;

		// allocate face ids memory
		std::vector<Triangle>	faceList;
		// every four neighbor points can make two triangle ?
		faceList.resize( (w-1)*(h-1)*2 );
		int faceNum = 0;

		//    0------1
		//    |  X   |
		//    2------3
		for (int i=0; i<h-1; i++)
		{
			for (int j=0; j<w-1; j++)
			{
				int id0 = m_maps[i  ][j  ];
				int id1 = m_maps[i  ][j+1];
				int id2 = m_maps[i+1][j  ];
				int id3 = m_maps[i+1][j+1];

				int faceID = ( (w-1)*i + j ) * 2;
				faceNum += DivideQuad( faceList[faceID], faceList[faceID+1], id0, id1, id2, id3);
			}// for (int j=0; j<m_w-1; j++)
		}//for (int i=0; i<m_h-1; i++)

		printf("faceNum faceNum= %d\n",faceNum);
		// copy the 
		fn = 0;
		m_faces.resize(faceNum);
		for (unsigned int i=0; i<faceList.size(); i++)
		{
			if( faceList[i][0]>=0 && faceList[i][1]>=0 && faceList[i][2]>=0 )
			{
				m_faces[fn][0] = faceList[i][0];
				m_faces[fn][1] = faceList[i][1];
				m_faces[fn][2] = faceList[i][2];
				fn++;
			}
		}
		printf("faceNum fn= %d\n",fn);
	}

	int sn3DRangeData::BuildTriangle(Triangle &tri,int id0, int id1,int id2)
	{
		int count = 1;
		if(TriangleRules( id0, id1, id2 ) == false )
		{
			id0 = id1 = id2 = -1;
			count = 0;
		}
		SetTriangle( tri, id0,  id1, id2 );
		return count;
	}
	bool sn3DRangeData::TriangleRules( int id0, int id1,int id2 )
	{
		Point3f v01 = m_pnts[id1] - m_pnts[id0];
		Point3f v12 = m_pnts[id2] - m_pnts[id1];
		Point3f v02 = m_pnts[id2] - m_pnts[id0];

		// 计算空间距离与平面距离
		float dxy01 = v01[0]*v01[0] + v01[1]*v01[1];
		float dxy12 = v12[0]*v12[0] + v12[1]*v12[1];
		float dxy02 = v02[0]*v02[0] + v02[1]*v02[1];

		float dxyz01 = dxy01 + v01[2]*v01[2];
		float dxyz12 = dxy12 + v12[2]*v12[2];
		float dxyz02 = dxy02 + v02[2]*v02[2]; 
		
		// 1 深度比较
		if( m_maxDepth2*dxy01<=dxyz01 || m_maxDepth2*dxy12<=dxyz12 || m_maxDepth2*dxy02<=dxyz02 )
			return false;
		// 2 距离比较
		if(dxyz01>=m_maxDis2 || dxyz02>=m_maxDis2 || dxyz12>=m_maxDis2)
			return false;

		// 计算点积 
		float dotProduct0 = v01[0]*v02[0] + v01[1]*v02[1] + v01[2]*v02[2]; // 可以不管符号
		float dotProduct1 = v01[0]*v12[0] + v01[1]*v12[1] + v01[2]*v12[2]; 
		float dotProduct2 = v12[0]*v02[0] + v12[1]*v02[1] + v12[2]*v02[2]; 
		// 3 三角形内角不超过。。。去掉狭长三角形
		if(    dxyz01*dxyz02*m_minAngleTri2 < dotProduct0*dotProduct0
			|| dxyz01*dxyz12*m_minAngleTri2 < dotProduct1*dotProduct1
			|| dxyz02*dxyz12*m_minAngleTri2 < dotProduct2*dotProduct2)
			return false;
		return true;
	}
	int  sn3DRangeData::DivideQuad(Triangle &tri0,Triangle &tri1, int id0, int id1, int id2, int id3)
	{
		int faceCount = 0;
		int np = (id0>=0) + (id1>=0) + (id2>=0) + (id3>=0);

		if( np < 3 )
		{
			SetTriangle( tri0, -1, -1, -1);
			SetTriangle( tri1, -1, -1, -1);
		}
		else if(  np == 4 )
		{
			if( distance2(id0, id3) < distance2( id1,id2) )
			{													//  0-------1    0-------1	
				faceCount += BuildTriangle( tri0,id0,id2,id3);	//  |       |    | 0 \ 1 |
				faceCount += BuildTriangle( tri1,id0,id3,id1);	//  2-------3    2-------3
			}									 
			else
			{													//  0-------1    0-------1
				faceCount += BuildTriangle( tri0,id0,id2,id1);	//  |       |    | 0 / 1 |
				faceCount += BuildTriangle( tri1,id1,id2,id3);	//  2-------3    2-------3
			}
			//faceCount = 2;
		}
		else
		{
			if( id0<0 )
			{													//  0       1
				SetTriangle (tri0, -1, -1, -1);					//      / 1 |
				faceCount = BuildTriangle(tri1,id1,id2,id3);		//  2-------3
			}
			else if( id1<0 )
			{													//  0       1
				faceCount = BuildTriangle(tri0 ,id0,id2,id3);	//  | 0 \   
				SetTriangle (tri1, -1, -1, -1);					//  2-------3
			}
			else if( id2<0 )
			{													//  0-------1
				SetTriangle (tri0, -1, -1, -1);					//      \ 1 |
				faceCount = BuildTriangle(tri1,id0,id3,id1);		//  2       3
			}
			else // id3
			{													//  0-------1
				faceCount = BuildTriangle(tri0 ,id0,id2,id1);	//  | 0 /    
				SetTriangle (tri1, -1, -1, -1);					//  2       3
			}
			//faceCount = 1;
		}// np == 3

		return faceCount;
	}

	void sn3DRangeData::Compact()
	{
		int ptCount=m_pnts.size();
		if(ptCount==0)
			return;

		// 记录那些数据要删除掉
		std::vector<int> usedFlags;	usedFlags.resize(ptCount);
		std::memset( &(usedFlags[0]),0, sizeof(int)*ptCount);
		for (int i=0; i<h; i++)
			for (int j=0; j<w; j++)
				if( m_maps[i][j]>=0 )
				 usedFlags[m_maps[i][j]]++;

		// 移动数据并记录id
		std::vector<int> remap;		 remap.resize(ptCount);
		int id=0;
		for (int i=0; i<ptCount; i++)
		{
			if(usedFlags[i]>0)
			{
				if(id!=i)
					m_pnts[id]= m_pnts[i];
				remap[i]  = id;
				id++;
			}
			else
				remap[i] = -1;
		}
		m_pnts.resize(id);
		vn = id;

		// 更新图索引
		for (int i=0; i<h; i++)
			for (int j=0; j<w; j++)
				if( m_maps[i][j]>=0 )
					m_maps[i][j] = remap[m_maps[i][j]];
		
		return ;
	}

	bool sn3DRangeData::Resample( int rate )
	{
		if( vn<=0 )
			return false;
		if( rate<=1 || rate>=w || rate>=h )
			return true;

		int width  = w/rate;
		int height = h/rate;

		int *ids = new int[width*height];

		vn = 0;fn=0;
		for(int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				if( (ids[i*width+j]=m_maps[i*rate][j*rate]) >=0 )
					vn++;
			}
		}

		w = width;
		h = height;
		m_maps.Create(width, height, ids);
		ReleaseNorm();
		ReleaseFace();
		Compact();
		return true;
	}

	bool sn3DRangeData::ResampleNorm(int rate, bool bFace)
	{
		if( vn<=0 )
			return false;
		if( rate<=1 || rate>=w || rate>=h )
			return true;

		if(HasNormal()==false)
			BuildNormal();

		int newVn=0;
		int newWidth  = w/rate;
		int newHeight = h/rate;

		int *newIds = new int[newWidth*newHeight];
		for(int ii=0; ii<h; ii+=rate)
		{
			for (int jj=0; jj<w; jj+=rate)
				if( m_maps[ii][jj] >=0)	newVn++;
		}
		std::vector<Point3f> newPoint,newNorm;
		newPoint.resize(newVn);newNorm.resize(newVn);

		// 拷贝数据，包含法向
		newVn=0;
		for(int i=0,ii=0; i<newHeight; i++,ii+=rate)
		{
			for (int j=0,jj=0; j<newWidth; j++,jj+=rate)
			{
				if( m_maps[ii][jj] >=0 )
				{
					newIds[i*newWidth+j]=newVn;
					newPoint[newVn] = m_pnts[m_maps[ii][jj]];
					newNorm[newVn]  = m_pntNorm[m_maps[ii][jj]];
					newVn++;
				}
				else
					newIds[i*newWidth+j] = -1;
			}
		}

		//=============================================
		// 重建面
		if(bFace && newHeight>2 && newWidth>2 && newVn>6 )
		{
			std::vector<Triangle>	faceList;
			faceList.resize( (newHeight-1)*(newWidth-1)*2 );
			int faceNum = 0;

			for (int i=0; i<newHeight-1; i++)
			{
				for (int j=0; j<newWidth-1; j++)
				{
					int ii = i*rate;
					int jj = j*rate;
					bool bPass = true;
					// 判断四个顶点是否能形成三角形
					for (int v=0; v<rate&&bPass; v++)
					{
						for (int u=0; u<rate&&bPass; u++)
						{
							int id0 = m_maps[ii+v  ][jj+u  ];
							int id1 = m_maps[ii+v  ][jj+u+1];
							int id2 = m_maps[ii+v+1][jj+u  ];
							int id3 = m_maps[ii+v+1][jj+u+1];
							if( (id0>0) + (id1>0) + (id2>0) +(id3>0) == 4)
							{
								//  0-------1	0-------1
								//  | 0 \ 1 |	| 0 / 1 |
								//  2-------3	2-------3
								if( distance2(id0, id3) < distance2( id1,id2) )	
									bPass = TriangleRules( id0,id2,id3)&&TriangleRules( id0,id3,id1);										 
								else									  
									bPass = TriangleRules( id0,id2,id1)&&TriangleRules( id1,id2,id3);
							}
							else
								bPass = false;
						} // for u
					} // for v
					// 设置三角形
					int faceID = ( (newWidth-1)*i + j ) * 2;
					if( bPass==false)
					{
						faceList[faceID  ][0] =faceList[faceID  ][1]=faceList[faceID  ][2]= -1;
						faceList[faceID+1][0] =faceList[faceID+1][1]=faceList[faceID+1][2]= -1;
					}
					else
					{
						faceNum+=2;
						if( distance2(m_maps[ii][jj], m_maps[ii+rate][jj+rate]) < distance2( m_maps[ii][jj+rate],m_maps[ii+rate][jj]) )	
						{
							faceList[faceID  ][0] = newIds[i*newWidth+j];
							faceList[faceID  ][1] = newIds[(i+1)*newWidth+j];
							faceList[faceID  ][2] = newIds[(i+1)*newWidth+j+1];
							faceList[faceID+1][0] = newIds[i*newWidth+j];
							faceList[faceID+1][1] = newIds[(i+1)*newWidth+j+1];
							faceList[faceID+1][2] = newIds[i*newWidth+j+1];
						}									 
						else
						{
							faceList[faceID  ][0] = newIds[i*newWidth+j];
							faceList[faceID  ][1] = newIds[(i+1)*newWidth+j];
							faceList[faceID  ][2] = newIds[i*newWidth+j+1];
							faceList[faceID+1][0] = newIds[i*newWidth+j+1];
							faceList[faceID+1][1] = newIds[(i+1)*newWidth+j];
							faceList[faceID+1][2] = newIds[(i+1)*newWidth+j+1];
						}
					} // bPass

				}// for (int j=0; j<...
			}//for (int i=0; i<...

			fn = 0;
			m_faces.resize(faceNum);
			for (unsigned int i=0; i<faceList.size(); i++)
			{
				if( faceList[i][0]>=0 && faceList[i][1]>=0 && faceList[i][2]>=0 )
				{
					m_faces[fn][0] = faceList[i][0];
					m_faces[fn][1] = faceList[i][1];
					m_faces[fn][2] = faceList[i][2];
					fn++;
				}
			}
			printf("faceNum fn= %d\n",fn);
		}	
		// 面片重建结束
		//===================================================
		w = newWidth;
		h = newHeight;
		m_maps.Create(newWidth, newHeight, newIds);
		vn = newVn;
		m_pnts.swap(newPoint);
		m_pntNorm.swap(newNorm);
		ReleaseColor();
		ReleaseDist();
		return true;
	}

	sn3DRangeData *sn3DRangeData::Clone()
	{
		sn3DRangeData *range = new sn3DRangeData;
		
		// copy range
		range->w  = w;
		range->h  = h;
		range->m_maps = m_maps;

		// copy points
		range->vn			= vn;
		range->m_pnts		= m_pnts;
		range->m_pntNorm	= m_pntNorm;
		range->m_pntColor	= m_pntColor;
		range->m_pntDist	= m_pntDist;

		// copy face
		range->fn		= fn;
		range->m_faces	= m_faces;
		range->m_box	= m_box;							//  几何模型包围盒
		range->m_center = m_center;							//  模型中心

		// copy build mesh parameter
		range->m_maxDepth2  = m_maxDepth2;					// 最大深度 平方
		range->m_maxDis2	= m_maxDis2;					// 视角与法线的最大夹角的余弦平方
		range->m_minAngleTri2  = m_minAngleTri2;			// 三角形最小角的余弦平方
		return range;
	}
}