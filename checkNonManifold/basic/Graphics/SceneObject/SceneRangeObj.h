#ifndef _SN3D_RANGE_MODEL_
#define _SN3D_RANGE_MODEL_

/*  sn3DRangeData 类

该类封装了深度图数据表示及操作方法

*/

#include <stdio.h>
#include <fstream>

#include "SceneObject.h"
#include "sn3DRangeRenderer.h"

#include "GeoData/RangeData.h"
#include "ObjectMotion.h"

#ifndef MAX_INFO_LENTH
#define MAX_INFO_LENTH 512
#endif

namespace sn3DCore
{
	class snGridInfo
	{
	public:
		snGridInfo()
		{
			needSample = true;
			step = 0.02;
			name[0] = '\0';
		}
	public:
		double step;
		bool   needSample;
		char   name[MAX_INFO_LENTH];
	};

	class sn3DRangeRenderer;
	class sn3DRangeModel : public SceneObject
	{
		friend class sn3DRangeData;
	public:
		sn3DRangeModel();
		virtual ~sn3DRangeModel();

		sn3DRangeData*			CloneRange(int detail, bool bNormal, bool bFace);
		sn3DRangeData*			GetRange() { return &m_range; };
		virtual int				GetPointNum() { return m_range.GetVN(); };
		virtual int				GetFaceNum() { return m_range.GetFN(); };
		virtual void			GetMatrixGL(float *matrix);
		virtual void			ReleaseData(bool all);
		virtual sn3DRenderer*	GetRenderer() { return &m_render; };
		virtual void			Render(GLW::DrawMode dm);	//  绘制模型
		void					RenderSelected();
		virtual void            UpdateAll() {};
		virtual void			UpdateBox();


		///////////////////////  顶点单元标志位  ///////////////////////////////
		enum
		{
			VSELECTED = 0x04,              // 是否被选择
			VDELETED = 0x08				// 是否被删除
		};

		bool IsS(int index) { return (m_pntFlags[index] & VSELECTED) != 0; }	// 选择位
		void SetS(int index) { m_pntFlags[index] |= VSELECTED; }
		void ClearS(int index) { m_pntFlags[index] &= ~VSELECTED; }
		bool IsD(int index) { return (m_pntFlags[index] & VDELETED) != 0; }	// 删除位
		void SetD(int index) { m_pntFlags[index] |= VDELETED; }
		void ClearD(int index) { m_pntFlags[index] &= ~VDELETED; }
		void InitFlag(sn3DRangeData *range);
		bool HasFlag() { return m_pntFlags.size() > 0; }
		void ClearAllS();

		//////////////////////////  ///////////////////////////////////////
		// 更新标志位，更新索引，更新硬盘文件
		bool UpdateFlag();
		bool ModifyData();
		void Compact() { return m_range.Compact(); };

		////////////////////////// 文件存储 //////////////////////////////////
		virtual const char *GetExtName() { return "rge"; };
		const char *GetFullPath();
		bool Load() { return Load(GetFullPath()); };
		bool Save() { return Save(GetFullPath()); };
		bool SaveRT();
		bool Load(std::string  name);
		bool Save(std::string  name);
		bool SaveSTLA(std::string  name);
		bool SaveSTLB(std::string  name);

	protected:
		bool Load(std::string  name, sn3DRangeData &range);

	public:
		double m_rotate[9];
		double m_trans[3];
		char   m_objInfo[50][MAX_INFO_LENTH];
		snGridInfo m_gridInfo;
	protected:

		sn3DRangeRenderer	m_render;
		sn3DRangeData		m_range;
		std::vector<int>	m_pntFlags; //顶点标志位

	public:
		//////////////////////////////////////////////////////////////////////
		//							扫描仪
		//////////////////////////////////////////////////////////////////////

		// 相机参数
		void SetCamera(double *R, double *T);// 设置相机参数位置参数
		void GetCamera(double *R, double *T);
		void SetCameraPos(double *pos, double *dir);
		void GetCameraPos(double *pos, double *dir);
		void GetCameraRotateZ(double *rotate);
		void SetMeshParam(float maxDepth, float maxDis, float minAngleTri);

		//  标志点
		bool SetRefPoint(int count, int *ids, double *icpx, double *icpy, double *icpz);
		bool IsShowRefPoint() { return m_bShowRef; };
		void SetShowRefPoint(bool bshow) { m_bShowRef = bshow; };

		bool IsFrameRefPoint() { return m_isFrameRef; };
		void SetFrameRefPoint() { m_isFrameRef = true; };

		// 标志点选择
		enum
		{
			REFPOINT_SELECTED = 1,
			REFPOINT_UNSELECTED = 0,
			REFPOINT_VALID = 1,
			REFPOINT_INVALID = 0
		};
		bool IsRefpointSelect(int index) { return (m_refSelectFlag[index] == REFPOINT_SELECTED); }	// 标志点选中
		void SetRefpointSelect(int index) { m_refSelectFlag[index] = REFPOINT_SELECTED; }
		void ClearRefpointSelect(int index) { m_refSelectFlag[index] = REFPOINT_UNSELECTED; }

		bool IsRefpointValid(int index) { return (m_refValidFlag[index] == REFPOINT_VALID); }// 标志点有效
		void SetRefpointValid(int index) { m_refValidFlag[index] = REFPOINT_VALID; }
		void ClearRefpointValid(int index) { m_refValidFlag[index] = REFPOINT_INVALID; }

	public:
		int					 m_refNum;	// 标志点个数
		std::vector<Point3d> m_refPts;	// 标志点
		std::vector<int>	 m_refID;	// 标志点ID号
		std::vector<char>	 m_refSelectFlag; //标志点选择状态
		std::vector<char>	 m_refValidFlag; //标志点是否有效状态

	protected:
		bool m_bShowRef;
		bool m_isFrameRef;

		// 网格参数
		float m_maxDepth;
		float m_maxDis;
		float m_minAngleTri;

		// 相机
		double m_cameraR[9];// Xc = R*Xw + T
		double m_cameraT[3];
		Point3d	m_cameraPos;
		Point3d	m_cameraDir;
	};
}
#endif
