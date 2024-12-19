//#include <stdafx.h>
#include "SceneRangeObj.h"

#include <windows.h>
#include <gl\gl.h>
#include <GL\glut.h>

#include "sn3DRangeRenderer.h"

#pragma warning(disable: 4996)
namespace sn3DCore
{
	static const int  RANGE_FILE_HEADER_STRING_LEN = 48;
	static const char *RANGE_FILE_HEADER_STRING_2_0 = "3DScan2.0 Range File.";
	static const char *RANGE_FILE_HEADER_STRING_2_1 = "3DScan2.1 Range File.";

	static char g_objInfo[50][MAX_INFO_LENTH];
	sn3DRangeModel::sn3DRangeModel()
	{
		m_refNum = 0;
		m_type = TYPE_RANGE;
		for (int i=0; i<3; i++)
		{
			m_trans[i]=0;
			for( int j=0; j<3; j++)
				m_rotate[i*3+j] = (i==j);		
		}

		m_bShowRef = true;
		for (int i=0; i<3; i++)
		{
			m_cameraT[i]=0;
			for( int j=0; j<3; j++)
				m_cameraR[i*3+j] = (i==j);		
		}
		m_cameraDir.x = 0.0;
		m_cameraDir.y = 0.0;
		m_cameraDir.z = 0.0;
		m_cameraPos.x = 0.0;
		m_cameraPos.y = 0.0; 
		m_cameraPos.z = 0.0;
		m_maxDepth = 10;
		m_maxDis = 1.2;
		m_minAngleTri=20;
	}

	sn3DRangeModel::~sn3DRangeModel()
	{
	}

	//  绘制模型
	void sn3DRangeModel::Render( GLW::DrawMode dm )
	{  
		glPushMatrix();
		if ( !IsH() )
		{
			float matrix[16];
			GetMatrixGL(matrix);

			glPushMatrix();
			m_render.SetDrawMode(dm); 

			m_render.Draw();

			if(m_bShowRef)
				m_render.RenderRefPoints();
			glPopMatrix();
		}
		glPopMatrix();
	}

	void sn3DRangeModel::RenderSelected()
	{
		GetRenderer()->RenderSelected();
	}

	void sn3DRangeModel::UpdateBox()
	{
		m_bbox.IsEmpty();
		m_range.GetBox(m_center, m_bbox.max, m_bbox.min);

		// 标志点在世界坐标系上
		double *R = m_rotate;
		double *T = m_trans;
		Point3f refPt;
		for (int i=0; i<m_refNum; i++)
		{
			Point3d &pt = m_refPts[i];
			refPt.x = float( R[0]*(pt.x-T[0]) + R[3]*(pt.y-T[1]) + R[6]*(pt.z-T[2]) );
			refPt.y = float( R[1]*(pt.x-T[0]) + R[4]*(pt.y-T[1]) + R[7]*(pt.z-T[2]) );
			refPt.z = float( R[2]*(pt.x-T[0]) + R[5]*(pt.y-T[1]) + R[8]*(pt.z-T[2]) );
			m_bbox.Add( refPt );
		}
	}

	void sn3DRangeModel::InitFlag(sn3DRangeData *range)
	{
		int vn = range->GetVN();
		int h  = range->GetH();
		int w  = range->GetW();
		m_pntFlags.resize( vn );
		for (int i=0;i<vn; i++)
			m_pntFlags[i] = VDELETED;
		int id;
		if( range->HasPoints() )
		{
			for (int i=0; i<h; i++)
			{
				for (int j=0; j<w; j++)
				{
					id = range->GetMap()[i][j];
					if( id>=0 )
						m_pntFlags[id] = 0;
				}
			}
		}
	}

	void sn3DRangeModel::ClearAllS()
	{
		int count = m_pntFlags.size();
		for (int i=0; i<count; i++)
		{
			if( IsS(i))
				ClearS(i);
		}
	}

	sn3DRangeData *sn3DRangeModel::CloneRange(int detail, bool bNormal, bool bFace )
	{
		printf("\nname===%s\n",m_strName.c_str());
		// 获取深度图数据
		sn3DRangeData *newRange = NULL;
		if(m_range.HasPoints()==false)
		{
			newRange = new sn3DRangeData;
			if( Load( GetFullPath(),*newRange ) == false )
			{
				delete newRange;
				return NULL;
			}	
		}
		else
			newRange = m_range.Clone();
		
		if( bFace )	bNormal = true;	
		newRange->SetMeshParameter( m_maxDepth, m_maxDis, m_minAngleTri);
		if( detail>1  )
		{
			if( bNormal)
				newRange->ResampleNorm(detail,bFace);
			else
				newRange->Resample( detail );
		}
		else
		{
			if( bNormal && newRange->HasNormal() == false )
				newRange->BuildNormal();

			if( bFace && newRange->HasFace() == false )
				newRange->BuildMesh();
		}
		return newRange;
	}

	void sn3DRangeModel::GetMatrixGL(float *matrix)
	{
		for (int i=0; i<16; i++)
			matrix[i] = 0.0f;

		for (int i=0; i<3; i++)
		{
			for (int j=0; j<3; j++)
			{
				matrix[i*4+j] = (float)m_rotate[j*3+i];
			}
		}
		matrix[12] = (float)m_trans[0];
		matrix[13] = (float)m_trans[1];
		matrix[14] = (float)m_trans[2];
		matrix[15] = 1.0f;
		return ;
	}

	void sn3DRangeModel::GetCameraRotateZ( double *rotate)
	{
		Point3d dirPt,posPt;
		this->GetCameraPos( posPt._v, dirPt._v );
		posPt.Normalize();

		double x = posPt.x;
		double y = posPt.y;
		double z = posPt.z;
		int signz = (z>0.0) ? 1 : -1;
		double yz = sqrt(y*y+z*z);

		double thetax = atan2( y, z );
		double thetay = atan2( x, yz );

		rotate[0] =  cos(thetay);
		rotate[1] = -sin(thetax)*sin(thetay);
		rotate[2] = -cos(thetax)*sin(thetay);

		rotate[3] =  0.0;
		rotate[4] =  cos(thetax);
		rotate[5] =  -sin(thetax);

		rotate[6] =  sin(thetay);
		rotate[7] =  sin(thetax)*cos(thetay);
		rotate[8] =  cos(thetax)*cos(thetay);
	}

	void sn3DRangeModel::SetMeshParam(float maxDepth, float maxDis, float minAngleTri)
	{
		m_maxDepth = maxDepth;
		m_maxDis   = maxDis;
		m_minAngleTri= m_minAngleTri;
		return;
	}

	void sn3DRangeModel::ReleaseData(bool all)
	{ 
		std::vector<int>().swap(m_pntFlags);
		m_range.ReleaseData(all);
	}

	bool sn3DRangeModel::SetRefPoint(int count, int *ids, double *icpx, double *icpy, double *icpz)
	{
		m_refNum = count;
		m_refID.resize(count);
		m_refPts.resize(count);
		m_refSelectFlag.resize(count);
		m_refValidFlag.resize(count);
		for (int i=0; i<count; i++)
		{
			m_refID[i] = ids[i];
			m_refPts[i][0] = icpx[i];
			m_refPts[i][1] = icpy[i];
			m_refPts[i][2] = icpz[i];
			m_refSelectFlag[i] = REFPOINT_UNSELECTED;
			m_refValidFlag[i]  = REFPOINT_VALID;
		}
		return true;
	}

	void sn3DRangeModel::SetCamera( double *R, double *T)
	{
		for (int i=0; i<3; i++)
			m_cameraT[i] = T[i];
		for (int i=0; i<9; i++)
			m_cameraR[i] = R[i];

		for (int i=0;i<3; i++)
			m_cameraPos[i] = -1.0*(R[0*3+i]*T[0] + R[1*3+i]*T[1] + R[2*3+i]*T[2]) ;

		m_cameraDir = m_cameraPos;
		m_cameraDir.Normalize();
	}
	void sn3DRangeModel::GetCamera( double *R, double *T)
	{
		for (int i=0; i<3; i++)
			T[i] = m_cameraT[i];
		for (int i=0; i<9; i++)
			R[i] = m_cameraR[i];
	}

	void sn3DRangeModel::SetCameraPos( double *pos, double *dir)
	{
		for (int i=0; i<3; i++)
		{
			m_cameraPos[i] = pos[i];
			m_cameraDir[i] = dir[i];
		}
	}

	void sn3DRangeModel::GetCameraPos( double *pos, double *dir)
	{
		for (int i=0; i<3; i++)
		{
			pos[i] = m_cameraPos[i];
			dir[i] = m_cameraDir[i];
		}
	}

	bool sn3DRangeModel::UpdateFlag()
	{
		if( m_range.HasPoints() == false && Load()== false )
			return false;
		bool modify = false;
		int vn=0;
		int w = m_range.GetW();
		int h = m_range.GetH();
		MatrixData<int> &maps = m_range.GetMap(); 

		for (int i=0; i<h; i++)
		{
			for (int j=0; j<w; j++)
			{
				int id = maps[i][j];
				if( id>=0 && IsD(id) )
				{
					maps[i][j] = -1;
					modify = true;
				}
			}
		}
		return modify;
	}

	bool sn3DRangeModel::ModifyData()
	{
		m_range.ReleaseData(false);
		m_range.UpdateBox();
		UpdateBox();
		return true;
	}

	const char *sn3DRangeModel::GetFullPath()
	{
		static std::string name;
		name = m_strPath;
		name += GetName();
		name += ".";
		name += GetExtName();
		return name.c_str();
	}

	bool sn3DRangeModel::Load(std::string  fileName,sn3DRangeData &range)
	{
		FILE *fp = fopen( fileName.c_str(), "rb");
		if( !fp )
			return false;

		range.ReleaseData(true);

		// 老版本没有 hasCramerDir 和 字符串
		bool hasCamerDir=true;
		char strHeader[RANGE_FILE_HEADER_STRING_LEN] = {0};
		fread(strHeader,sizeof(strHeader), 1, fp);
		if( strcmp(strHeader, RANGE_FILE_HEADER_STRING_2_0) != 0 )
		{
			fseek(fp,0,SEEK_SET);
			hasCamerDir = false;
		}

		//文件头
		fread(&range.w,	sizeof(int),	1,	fp);
		fread(&range.h,	sizeof(int),	1,	fp);
		fread(&range.vn,	sizeof(int),	1,	fp);
		fread(g_objInfo,	MAX_INFO_LENTH,50,  fp);

		int refNum;
		double rotate[9], trans[3];
		fread(&refNum,	sizeof(int),	1,	fp);
		fread(rotate,		sizeof(double),	9,	fp);
		fread(trans,		sizeof(double),	3,	fp);
		fread(m_cameraPos._v,	sizeof(double), 3, fp);
		if( hasCamerDir )
			fread(m_cameraDir._v,sizeof(double), 3, fp);

		//深度图索引
		int* maps = new int[range.w*range.h];
		fread(maps, sizeof(int), range.w*range.h, fp);
		range.m_maps.Create( range.w, range.h, maps);
		
		//读取点数据
		range.m_pnts.resize(range.vn);
		if( range.vn )
			fread( &(range.m_pnts[0][0]), sizeof(float), 3*range.vn, fp);

		fclose(fp);
		return true;
	}

	bool sn3DRangeModel::Load(std::string  fileName)
	{
		FILE *fp = fopen( fileName.c_str(), "rb");
		if( !fp )
			return false;

		m_range.ReleaseData(true);
		
		// 老版本没有 hasCramerDir 和 字符串
		bool hasCamerDir=true;
		char strHeader[RANGE_FILE_HEADER_STRING_LEN] = {0};
		fread(strHeader,sizeof(strHeader), 1, fp);
		if( strcmp(strHeader, RANGE_FILE_HEADER_STRING_2_0) != 0 )
		{
			fseek(fp,0,SEEK_SET);
			hasCamerDir = false;
		}

		//文件头
		fread(&m_range.w,	sizeof(int),	1,	fp);
		fread(&m_range.h,	sizeof(int),	1,	fp);
		fread(&m_range.vn,	sizeof(int),	1,	fp);
		fread(g_objInfo,	MAX_INFO_LENTH,50,  fp);
		fread(&m_refNum,	sizeof(int),	1,	fp);
		fread(m_rotate,		sizeof(double),	9,	fp);
		fread(m_trans,		sizeof(double),	3,	fp);
		fread( m_cameraPos._v,	sizeof(double), 3, fp);
		if( hasCamerDir )
			fread( m_cameraDir._v,sizeof(double), 3, fp);

		//深度图索引
		int* maps = new int[m_range.w*m_range.h];
		fread(maps, sizeof(int), m_range.w*m_range.h, fp);
		m_range.GetMap().Create( m_range.w, m_range.h, maps);

		//读取点数据
		m_range.GetPoints().resize(m_range.vn);
		if( m_range.vn>0)
			fread( &(m_range.GetPoints()[0][0]), sizeof(float), 3*m_range.GetVN(), fp);

		//标志点匹配
		if( m_refNum>0)
		{
			m_refID.resize(m_refNum);
			fread(&(m_refID[0]),sizeof(int),	m_refNum, fp);

			m_refPts.resize(m_refNum);
			double* icpx = new double[m_refNum];
			double* icpy = new double[m_refNum];
			double* icpz = new double[m_refNum];
			fread( icpx,		sizeof(double), m_refNum, fp);
			fread( icpy,		sizeof(double), m_refNum, fp);
			fread( icpz,		sizeof(double), m_refNum, fp);
			for(int i=0; i<m_refNum; i++)
				m_refPts[i] = Point3d( icpx[i], icpy[i], icpz[i] );
			delete[] icpx; delete[] icpy; delete[] icpz;
		}
		fclose(fp);
		InitFlag(&m_range);
		m_range.UpdateBox();
		return true;
	}

	bool sn3DRangeModel::SaveRT()
	{
		std::string fileName = GetFullPath();
		FILE *fp = fopen( fileName.c_str(), "rb+");
		if( !fp )
			return false;

		// 老版本没有头
		char strHeader[RANGE_FILE_HEADER_STRING_LEN] = {0};
		fread(strHeader,sizeof(strHeader), 1, fp);
		if( strcmp(strHeader, RANGE_FILE_HEADER_STRING_2_0) != 0 )
		{
			fseek(fp,0,SEEK_SET);
		}

		fseek(fp,sizeof(int),SEEK_CUR);		//m_range.w
		fseek(fp,sizeof(int),SEEK_CUR);		//m_range.h
		fseek(fp,sizeof(int),SEEK_CUR);		//m_range.vn	
		fseek(fp,MAX_INFO_LENTH*50,SEEK_CUR);//objInfo
		fseek(fp,sizeof(int),SEEK_CUR);		//m_refNum
		fwrite(m_rotate,sizeof(double),	9,	fp);
		fwrite(m_trans, sizeof(double),	3,	fp);

		fclose(fp);
		return true;
	}

	bool sn3DRangeModel::Save(std::string  fileName)
	{
		FILE *fp = fopen( fileName.c_str(), "wb");
		if( !fp )
			return false;
		// 文件头
		char strHeader[RANGE_FILE_HEADER_STRING_LEN] = {0};
		strcpy(strHeader,RANGE_FILE_HEADER_STRING_2_0);
		fwrite(strHeader,sizeof(strHeader), 1, fp);

		fwrite(&m_range.w, sizeof(int), 1, fp);
		fwrite(&m_range.h, sizeof(int), 1, fp);
		fwrite(&m_range.vn,sizeof(int),1,fp);
		strcpy( g_objInfo[0], m_strPath.c_str() );
		strcpy( g_objInfo[1], m_strName.c_str() );
		fwrite( g_objInfo, MAX_INFO_LENTH, 50, fp);

		fwrite(&m_refNum,sizeof(int),	1,	fp);
		fwrite(m_rotate,sizeof(double),	9,	fp);
		fwrite(m_trans, sizeof(double),	3,	fp);
		fwrite( m_cameraPos._v,sizeof(double), 3, fp);
		fwrite( m_cameraDir._v,sizeof(double), 3, fp);

		// 数据区
		if( m_range.h>0 && m_range.w>0 )
			fwrite(&(m_range.m_maps[0][0]), sizeof(int), m_range.w*m_range.h, fp);
		if(m_range.vn>0)
			fwrite(&(m_range.m_pnts[0][0]), sizeof(float), 3*m_range.vn, fp);
		// 标志点匹配
		if( m_refNum>0)
		{
			m_refID.resize(m_refNum);
			fwrite(&(m_refID[0]),sizeof(int),	m_refNum, fp);

			for (int i=0; i<m_refNum; i++)
				fwrite(&(m_refPts[i][0]),	sizeof(double), 1, fp);
			for (int i=0; i<m_refNum; i++)
				fwrite(&(m_refPts[i][1]),	sizeof(double), 1, fp);
			for (int i=0; i<m_refNum; i++)
				fwrite(&(m_refPts[i][2]),	sizeof(double), 1, fp);
		}
		fclose(fp);
		return true;
	}

	// 输出模型在局部坐标系，没有变换
	bool sn3DRangeModel::SaveSTLA(std::string  name)
	{

		FILE *fp = fopen( name.c_str(), "w");
		if(fp==NULL)
			return false;
		fprintf(fp,"solid test.stl\n");

		sn3DRangeData*  range = CloneRange( 1,true,true );
		if( range == NULL )
			return false;

		int faceN = range->m_faces.size();
		for(int i=0;i<faceN;i++)
		{
			fprintf(fp,"facet normal 0 0 0\n");
			fprintf(fp,"outer loop\n");

			int id0 = range->m_faces[i][0];
			int id1 = range->m_faces[i][1];
			int id2 = range->m_faces[i][2];

			if( id0<0 || id1<0 || id2<0 )
				continue;
			if( HasFlag() && (IsD(id0) || IsD(id1) || IsD(id2)) )
				continue;
			Point3f  &p0 = range->m_pnts[id0];
			Point3f  &p1 = range->m_pnts[id1];
			Point3f  &p2 = range->m_pnts[id2];
			fprintf(fp,"vertex %f %f %f\n",p0.x, p0.y, p0.z);
			fprintf(fp,"vertex %f %f %f\n",p1.x, p1.y, p1.z  );
			fprintf(fp,"vertex %f %f %f\n",p2.x, p2.y, p2.z );

			fprintf(fp,"endloop\n");
			fprintf(fp,"endfacet\n");
		}
		fprintf(fp,"endsolid test.stl\n");
		fclose(fp);
		delete range;
		return true;
	}

	// 乘以变换，在全局坐标系下
	bool sn3DRangeModel::SaveSTLB(std::string  name)
	{
		FILE *fp = fopen( name.c_str(),"wb");
		if(fp==NULL)
			return false;
		
		sn3DRangeData*  range = CloneRange( 1,true,true );
		if( range == NULL )
			return false;

		int faceNum = range->m_faces.size();
		fseek(fp, 80, SEEK_SET);

		fwrite(&faceNum, sizeof(int), 1, fp);

		// 变换点
		//////////////////////////////////////////////////////////////////////
		float R[9],T[3];
		for (int i=0; i<9; i++)
			R[i] = (float)m_rotate[i];
		for (int i=0; i<3; i++)
			T[i] = (float)m_trans[i];

		std::vector<Point3f> pnts;
		pnts.resize(range->m_pnts.size());
		for (int i=0;i<range->h; i++)
		{
			for (int j=0; j<range->w; j++)
			{
				int id = range->m_maps[i][j];
				if(id<0)
					continue;
				Point3f  &p = range->m_pnts[id];
				pnts[id].x = R[0]*p.x + R[1]*p.y + R[2]*p.z + T[0];
				pnts[id].y = R[3]*p.x + R[4]*p.y + R[5]*p.z + T[1];
				pnts[id].z = R[6]*p.x + R[7]*p.y + R[8]*p.z + T[2];
			}
		}
		//////////////////////////////////////////////////////////////////////
		short attr=0;
		float norm[3];	
		norm[0] = 0.0;
		norm[1] = 0.0;
		norm[2] = 0.0;

		for(int k=0;k<faceNum;k++)
		{
			fwrite(norm,sizeof(float),3,fp);
			int id0 = range->m_faces[k][0];
			int id1 = range->m_faces[k][1];
			int id2 = range->m_faces[k][2];

			if( id0<0 || id1<0 || id2<0 )
				continue;

			Point3f  &p0 = pnts[id0];
			Point3f  &p1 = pnts[id1];
			Point3f  &p2 = pnts[id2];

			fwrite(p0._v,sizeof(float),3,fp);
			fwrite(p1._v,sizeof(float),3,fp);
			fwrite(p2._v,sizeof(float),3,fp);

			fwrite(&attr,2,1,fp); 
		}

		fclose(fp);

		return true;
	}
}