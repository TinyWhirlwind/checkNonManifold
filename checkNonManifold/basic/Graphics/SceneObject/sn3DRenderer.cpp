#include "sn3DRenderer.h"

#include <QtCore/qdebug.h>
#include <GL\glew.h>
#include "SceneParam.h"

#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK,__FILE__, __LINE__)
#endif

namespace sn3DCore
{
	DisplayList::~DisplayList()
	{
		if (glIsList(list))
			glDeleteLists(list, 1);
	}

	sn3DRenderer::sn3DRenderer()
	{
		m_drawMode = (GLW::DrawMode)SceneParam::Inst().mDrawMode;	//DMFill;
		m_shadeMode = (GLW::ShadeMode)SceneParam::Inst().mShadeMode;	//SMSmooth;
		m_renderMethod = (GLW::RenderMethod)SceneParam::Inst().mRenderMethod;// RMShader;
		m_colorMode = (GLW::ColorMode)SceneParam::Inst().mColorMode; //CMPerMesh;
		m_flag = 0;
		m_bTransparent = false;
		m_bDepthMask = true;
		m_bDepthTest = true;
		m_bTwoSideLight = false;
		m_bLighting = true;
	}

	sn3DRenderer::~sn3DRenderer()
	{
	}

	void sn3DRenderer::Clear()
	{
		ClearDisplayList();
	}
	
	void sn3DRenderer::SetListType(ListType type)
	{
		m_listType = type;
	}
	
	void  sn3DRenderer::BuildList(ListType type)
	{
		m_listType = type;
		if (type.type == ListType::LTPoint)
			BuildPointList(/*type.detail*/);
		else
			BuildFaceList(/*type.detail*/);
		m_list.type = type;
	}

	void sn3DRenderer::AddMaterial(MaterialFace mf, MaterialInfo& info)
	{
		m_material[mf] = info;
	}

	void sn3DRenderer::ClearMaterial()
	{
		m_material.clear();
	}
	
	void sn3DRenderer::StartProperty()
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if (IsBlend())
		{			
			glEnable(GL_BLEND);
		}
		else
		{			
			glDisable(GL_BLEND);
		}

		if (IsDepthMask())
		{
			glDepthMask(GL_TRUE);
		}
		else
		{
			glDepthMask(GL_FALSE);
		}

		if (IsDepthTest())
		{
			glEnable(GL_DEPTH_TEST);		
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		if (IsLighting())
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);

		if (IsTwoSideLight())
		{
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
			glDisable(GL_CULL_FACE);
			if (!IsLighting())
				glEnable(GL_LIGHTING);
		}
		else
			glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
		
		// 材质
		if (!m_material.empty())
		{
			glDisable(GL_COLOR_MATERIAL);
		}
		else
			glEnable(GL_COLOR_MATERIAL);

		std::map<MaterialFace, MaterialInfo>::iterator iterMaterial = m_material.begin();
		GLenum materialI;
		for (; iterMaterial != m_material.end(); ++iterMaterial)
		{
			switch (iterMaterial->first)
			{
			case MF_FRONT:
				materialI = GL_FRONT;
				break;
			case MF_BACK:
				materialI = GL_BACK;
				break;
			case MF_FRONT_AND_BACK:
				materialI = GL_FRONT_AND_BACK;
				break;
			default:
				materialI = GL_FRONT_AND_BACK;
				break;
			}
			glMaterialfv(materialI, GL_AMBIENT, &(iterMaterial->second.ambient[0]));
			glMaterialfv(materialI, GL_DIFFUSE, &(iterMaterial->second.diffuse[0]));
			glMaterialfv(materialI, GL_SPECULAR, &(iterMaterial->second.specular[0]));
		}
	}

	void sn3DRenderer::EndProperty()
	{
		glPopAttrib();
	}

	void sn3DRenderer::DrawGeneral()
	{
		glPushMatrix();

		switch (m_drawMode)
		{
		case DMPoints: DrawPoints();  break;
		case DMWire: DrawWire();    break;
		case DMHidden: DrawHidden();  break;
		case DMFill: DrawFill();    break;
		case DMFillWire: DrawFillWire(); break;
		default: break;
		}
		glPopMatrix();
	}
	
	void sn3DRenderer::DrawHidden()
	{
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_DEPTH_TEST);

		glPolygonOffset(1.0, 1);
		glDisable(GL_LIGHTING);
		glDisable(GL_LINE_SMOOTH);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		DrawFill();
		glDisable(GL_POLYGON_OFFSET_FILL);

		glEnable(GL_LIGHTING);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		DrawWire();
		glPopAttrib();
	}

	void sn3DRenderer::DrawFillWire()
	{
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);

		// 绘制填充
		DrawFill();

		//绘制线框
		glEnable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_LIGHTING);
	
		glPolygonOffset(-1.f, -1.f);	//向外
		
		DrawWire();
		
		glPopAttrib();
	}

	void sn3DRenderer::NewList()
	{
		if (glIsList(m_list.list))
			glDeleteLists(m_list.list, 1);

		m_list.list = glGenLists(1);
	}

	void sn3DRenderer::ClearDisplayList()
	{
		if (glIsList(m_list.list))
		{
			glDeleteLists(m_list.list, 1);
			m_list.list = -1;
		}
	}

	void sn3DRenderer::SetDisplayList(int list)
	{
		this->m_list.list = list;
	}

	void sn3DRenderer::BuildList()
	{
		if(m_drawMode == DMPoints)
		{
			BuildPointList();
		}
		else
		{
			BuildFaceList();
		}
	}

	void sn3DRenderer::DrawBox(Point3f maxPoint, Point3f minPoint)
	{
		std::vector<Point3f> corner;
		corner.resize(8);
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					if (i % 2 == 0)
						corner[4 * k + 2 * j + i].X() = maxPoint.X();
					else
						corner[4 * k + 2 * j + i].X() = minPoint.X();
					if (j % 2 == 0)
						corner[4 * k + 2 * j + i].Y() = maxPoint.Y();
					else
						corner[4 * k + 2 * j + i].Y() = minPoint.Y();
					if (k % 2 == 0)
						corner[4 * k + 2 * j + i].Z() = maxPoint.Z();
					else
						corner[4 * k + 2 * j + i].Z() = minPoint.Z();
				}

		DrawBox(corner);
	}

	void sn3DRenderer::DrawBox(std::vector<Point3f> corner)
	{
		//glColor4f(0.9647f,0.588235f,0.278432f,1.0f);//color
		glColor4f(0.9647f, 0.588235f, 0.278432f, 0.5f);//color
		glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_LIGHTING_BIT);

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

		glDisable(GL_LIGHTING);
		glLineWidth(1.7f);

		glBegin(GL_LINES);
		glVertex3f(corner[0].X(), corner[0].Y(), corner[0].Z());
		glVertex3f(corner[1].X(), corner[1].Y(), corner[1].Z());

		glVertex3f(corner[3].X(), corner[3].Y(), corner[3].Z());
		glVertex3f(corner[2].X(), corner[2].Y(), corner[2].Z());

		glVertex3f(corner[7].X(), corner[7].Y(), corner[7].Z());
		glVertex3f(corner[6].X(), corner[6].Y(), corner[6].Z());

		glVertex3f(corner[4].X(), corner[4].Y(), corner[4].Z());
		glVertex3f(corner[5].X(), corner[5].Y(), corner[5].Z());


		glVertex3f(corner[0].X(), corner[0].Y(), corner[0].Z());
		glVertex3f(corner[2].X(), corner[2].Y(), corner[2].Z());

		glVertex3f(corner[1].X(), corner[1].Y(), corner[1].Z());
		glVertex3f(corner[3].X(), corner[3].Y(), corner[3].Z());

		glVertex3f(corner[5].X(), corner[5].Y(), corner[5].Z());
		glVertex3f(corner[7].X(), corner[7].Y(), corner[7].Z());

		glVertex3f(corner[4].X(), corner[4].Y(), corner[4].Z());
		glVertex3f(corner[6].X(), corner[6].Y(), corner[6].Z());


		glVertex3f(corner[0].X(), corner[0].Y(), corner[0].Z());
		glVertex3f(corner[4].X(), corner[4].Y(), corner[4].Z());

		glVertex3f(corner[1].X(), corner[1].Y(), corner[1].Z());
		glVertex3f(corner[5].X(), corner[5].Y(), corner[5].Z());

		glVertex3f(corner[2].X(), corner[2].Y(), corner[2].Z());
		glVertex3f(corner[6].X(), corner[6].Y(), corner[6].Z());

		glVertex3f(corner[3].X(), corner[3].Y(), corner[3].Z());
		glVertex3f(corner[7].X(), corner[7].Y(), corner[7].Z());

		glEnd();
		
		glPopAttrib();
	}

	void sn3DRenderer::DrawSolidBox(Point3f maxPoint, Point3f minPoint)
	{
		Point3f corner[8];
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
				{
					if (i % 2 == 0)
						corner[4 * k + 2 * j + i].X() = maxPoint.X();
					else
						corner[4 * k + 2 * j + i].X() = minPoint.X();
					if (j % 2 == 0)
						corner[4 * k + 2 * j + i].Y() = maxPoint.Y();
					else
						corner[4 * k + 2 * j + i].Y() = minPoint.Y();
					if (k % 2 == 0)
						corner[4 * k + 2 * j + i].Z() = maxPoint.Z();
					else
						corner[4 * k + 2 * j + i].Z() = minPoint.Z();
				}

		glColor4f(0.9647f, 0.588235f, 0.278432f, 0.5f);//color
		glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_LIGHTING_BIT);

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

		glDisable(GL_LIGHTING);
		glLineWidth(0.7f);

		glBegin(GL_LINES);
		glVertex3f(corner[0].X(), corner[0].Y(), corner[0].Z());
		glVertex3f(corner[1].X(), corner[1].Y(), corner[1].Z());
		glVertex3f(corner[3].X(), corner[3].Y(), corner[3].Z());
		glVertex3f(corner[2].X(), corner[2].Y(), corner[2].Z());
		glVertex3f(corner[7].X(), corner[7].Y(), corner[7].Z());
		glVertex3f(corner[6].X(), corner[6].Y(), corner[6].Z());
		glVertex3f(corner[4].X(), corner[4].Y(), corner[4].Z());
		glVertex3f(corner[5].X(), corner[5].Y(), corner[5].Z());
		glVertex3f(corner[0].X(), corner[0].Y(), corner[0].Z());
		glVertex3f(corner[2].X(), corner[2].Y(), corner[2].Z());
		glVertex3f(corner[1].X(), corner[1].Y(), corner[1].Z());
		glVertex3f(corner[3].X(), corner[3].Y(), corner[3].Z());
		glVertex3f(corner[5].X(), corner[5].Y(), corner[5].Z());
		glVertex3f(corner[7].X(), corner[7].Y(), corner[7].Z());
		glVertex3f(corner[4].X(), corner[4].Y(), corner[4].Z());
		glVertex3f(corner[6].X(), corner[6].Y(), corner[6].Z());
		glVertex3f(corner[0].X(), corner[0].Y(), corner[0].Z());
		glVertex3f(corner[4].X(), corner[4].Y(), corner[4].Z());
		glVertex3f(corner[1].X(), corner[1].Y(), corner[1].Z());
		glVertex3f(corner[5].X(), corner[5].Y(), corner[5].Z());
		glVertex3f(corner[2].X(), corner[2].Y(), corner[2].Z());
		glVertex3f(corner[6].X(), corner[6].Y(), corner[6].Z());
		glVertex3f(corner[3].X(), corner[3].Y(), corner[3].Z());
		glVertex3f(corner[7].X(), corner[7].Y(), corner[7].Z());

		glEnd();
		glPopAttrib();
	}

	void sn3DRenderer::Cylinder(int slices, float lenght, float width)
	{
		Point3f p0;
		Point3f p1;
		glPushMatrix();
		glScaled(lenght, width, width);

		float step = 6.28f / (float)slices;
		float angle = 0;

		//绘制圆柱侧面
		glBegin(GL_TRIANGLE_STRIP);
		for (int b = 0; b <= slices; ++b)
		{
			if (b == slices) angle = 0.0;
			p0 = Point3f(0, sin(angle), cos(angle));
			p1 = p0; p1[0] = 1.f;
			glNormal3f(p0[0], p0[1], p0[2]);
			glVertex3d(p0[0], p0[1], p0[2]);
			glVertex3d(p1[0], p1[1], p1[2]);
			angle += step;
		}
		glEnd();

		//绘制圆柱底面
		angle = 0;
		p0 = Point3f(0, 0, 0);

		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(-1, 0, 0);
		glVertex3d(p0[0], p0[1], p0[2]);
		for (int b = 0; b <= slices - 1; ++b)
		{
			glNormal3f(-1, 0, 0);
			p1 = Point3f(0, sin(angle), cos(angle));
			glVertex3d(p1[0], p1[1], p1[2]);
			angle += step;
		}
		glNormal3f(-1, 0, 0);
		glVertex3d(0, 0, 1);
		glEnd();

		//绘制圆柱顶面
		angle = 6.28f;
		p0 = Point3f(1, 0, 0);

		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(1, 0, 0);
		glVertex3d(p0[0], p0[1], p0[2]);
		for (int b = 0; b <= slices - 1; ++b)
		{
			glNormal3f(1, 0, 0);
			p1 = Point3f(1, sin(angle), cos(angle));
			glVertex3d(p1[0], p1[1], p1[2]);
			angle -= step; //逆时针
		}
		glNormal3f(1, 0, 0);
		glVertex3d(1, 0, 1);
		glEnd();
		glPopMatrix();
	}

	void sn3DRenderer::Cone(int slices, float lenght, float width)
	{
		Point3f p0;//顶点或中心点
		Point3f P[2];
		Point3f N[2];

		glPushMatrix();
		glScaled(lenght, width, width); //对高度1底面半径为1的圆锥进行缩放

		//绘制侧面
		p0 = Point3f(1.0, 0, 0);
		N[0] = Point3f(1.f, sinf(0), cosf(0)); //没有归一化
		P[0] = Point3f(0, sinf(0), cosf(0));

		for (int b = 1; b <= slices; ++b)
		{
			float angle = -6.28f*(float)b / (float)slices;//逆时针
			if (b == slices) angle = 0;

			N[1] = Point3f(1.f, sinf(angle), cosf(angle));
			P[1] = Point3f(0, sinf(angle), cosf(angle));

			glBegin(GL_TRIANGLES);
			Point3f n = ((P[0] - p0) ^ (P[1] - p0)).Normalize();
			glNormal3f(n[0], n[1], n[2]);
			glVertex3f(p0[0], p0[1], p0[2]);
			glNormal3f(N[0][0], N[0][1], N[0][2]);
			glVertex3f(P[0][0], P[0][1], P[0][2]);
			glNormal3f(N[1][0], N[1][1], N[1][2]);
			glVertex3f(P[1][0], P[1][1], P[1][2]);
			glEnd();

			N[0] = N[1];
			P[0] = P[1];
		}

		//绘制底面
		p0 = Point3f(0, 0, 0);
		P[0] = Point3f(0, sinf(0), cosf(0));

		for (int b = 1; b <= slices; ++b)
		{
			float angle = -6.28f*(float)b / (float)slices;
			if (b == slices) angle = 0;

			Point3f n = Point3f(-1.f, 0, 0);
			P[1] = Point3f(0, sinf(angle), cosf(angle));

			glBegin(GL_TRIANGLES);
			glNormal3f(n[0], n[1], n[2]);
			glVertex3f(P[0][0], P[0][1], P[0][2]);
			glVertex3f(p0[0], p0[1], p0[2]);
			glVertex3f(P[1][0], P[1][1], P[1][2]);
			glEnd();

			P[0] = P[1];
		}
		glPopMatrix();
	}
}