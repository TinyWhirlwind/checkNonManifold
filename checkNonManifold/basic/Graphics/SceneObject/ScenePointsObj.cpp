#include"stdafx.h"
#include"ScenePointsObj.h"

#include<windows.h>
#include<gl/gl.h>

namespace sn3DCore
{
	sn3DPointsModel::sn3DPointsModel()
	{
		m_pointCloud = 0;
		m_type = TYPE_POINTS;
		SetTypeName("PointsModel");
	}

	sn3DPointsModel::sn3DPointsModel(boost::shared_ptr<sn3DPointsModel> pointmodel)
	{
		m_type = pointmodel->GetType();
		m_pointCloud.reset(new sn3DPointsData(pointmodel->GetPointCloud()));
	}

	void sn3DPointsModel::Render()
	{
		boost::shared_ptr<sn3DPointsRenderer> render = GetRenderer();

		float colorv[4];	colorv[3] = 1.0;
		float colord[4];	colord[3] = 1.0;
		float colors[4];	colors[3] = 1.0;

		colorv[0] = 0.8f; colorv[1] = 0.8f; colorv[2] = 0.8f;
		colord[0] = 0.8f; colord[1] = 0.8f; colord[2] = 0.8f;
		colors[0] = 0.1f; colors[1] = 0.1f; colors[2] = 0.1f;

		glMaterialfv(GL_FRONT, GL_AMBIENT, colorv);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, colord);
		glMaterialfv(GL_FRONT, GL_SPECULAR, colors);

		colorv[0] = 0.5f; colorv[1] = 0.5f; colorv[2] = 0.5f;
		colord[0] = 0.5f; colord[1] = 0.5f; colord[2] = 0.5f;
		colors[0] = 0.0f; colors[1] = 0.0f; colors[2] = 0.0f;

		glMaterialfv(GL_BACK, GL_AMBIENT, colorv);
		glMaterialfv(GL_BACK, GL_DIFFUSE, colord);
		glMaterialfv(GL_BACK, GL_SPECULAR, colors);

		if (!IsH())
		{
			glPushMatrix();
			m_render->Draw();
			RenderSelected();
			glPopMatrix();
		}
	}

	void sn3DPointsModel::RenderSelected()
	{
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT | GL_POINT_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f, 0.0, 0.0, 1.0f);
		glPointSize(2.0);
		glPopAttrib();
	}

	void sn3DPointsModel::RenderBox()
	{
		m_render->DrawSolidBox(m_bbox.max, m_bbox.min);
	}

	void sn3DPointsModel::UpdateAll()
	{
		UpdateBox();
		UpdateDisplay();
	}

	void sn3DPointsModel::UpdateDisplay()
	{
		if(m_render)
			m_render->Update();
		UpdateSelectionDisplay();
	}

	void  sn3DPointsModel::UpdateSelectionDisplay()
	{

	}

	void sn3DPointsModel::UpdatePointDisplay()
	{

	}

	void sn3DPointsModel::UpdateBox()
	{
		if (!m_pointCloud)  return;
		//compute center	
		m_center = Point3f(0.0, 0.0, 0.0);

		int count = 0; //未删除顶点计数

		size_t vn = m_pointCloud->vn;
		for (size_t i = 0; i < vn; i++)
		{
			Point3f p = m_pointCloud->m_vertices[i].P();
			m_center.X() += p.X();
			m_center.Y() += p.Y();
			m_center.Z() += p.Z();
			count++;
		}

		m_center.X() /= (float)count;
		m_center.Y() /= (float)count;
		m_center.Z() /= (float)count;

		//compute box	
		Point3f max, min;

		max = Point3f(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		min = Point3f(FLT_MAX, FLT_MAX, FLT_MAX);

		for (size_t i = 0; i < vn; i++)
		{
			Point3f p = m_pointCloud->m_vertices[i].P();

			if (min.X() > p.X()) min.X() = p.X();
			if (min.Y() > p.Y()) min.Y() = p.Y();
			if (min.Z() > p.Z()) min.Z() = p.Z();
			if (max.X() < p.X()) max.X() = p.X();
			if (max.Y() < p.Y()) max.Y() = p.Y();
			if (max.Z() < p.Z()) max.Z() = p.Z();
		}

		m_bbox.max = max;
		m_bbox.min = min;
	}

	void sn3DPointsModel::UpdateDirection()
	{


	}

	int sn3DPointsModel::GetPointSize()
	{
		if (m_pointCloud == 0)
		{
			return 0;
		}
		else
		{
			return (int)m_pointCloud->n_vertices();
		}
	}

	bool sn3DPointsModel::IsSelected()
	{
		if (this->m_pointCloud == 0) 
			return false;

		size_t vn = m_pointCloud->vn;
		int count = 0;
		for (size_t i = 0; i < vn; i++)
		{
			if (m_pointCloud->V((int)i)->IsS())
				count++;
		}

		if (count > 0)return true;
		else return false;
	}

	void sn3DPointsModel::SetPoints(int vn, float *points)
	{
		boost::shared_ptr<sn3DPointsData> data = AddPointCloud();

		for (int i = 0; i < vn; i++)
		{
			Point3f p;
			p[0] = points[i * 3];
			p[1] = points[i * 3 + 1];
			p[2] = points[i * 3 + 2];
			data->AddVertex(p);
		}
	}

	Point3f sn3DPointsModel::GetPoint(int index)
	{
		if (m_pointCloud == 0)
		{
			return Point3f(0.0, 0.0, 0.0);
		}
		else
		{
			return m_pointCloud->P(index);
		}
	}

	boost::shared_ptr<sn3DPointsData> sn3DPointsModel::AddPointCloud()
	{
		m_pointCloud.reset(new sn3DPointsData);
		m_render->SetGeometry(m_pointCloud);

		return m_pointCloud;
	}

	boost::shared_ptr<sn3DPointsData> sn3DPointsModel::AddPointCloud(size_t num)
	{
		m_pointCloud.reset(new sn3DPointsData);
		m_render->SetGeometry(m_pointCloud);

		for (size_t i = 0; i < num; ++i)
			m_pointCloud->AddPoint(Point3f(0.0f, 0.0f, 0.0f));

		return m_pointCloud;
	}

	void sn3DPointsModel::Clear()
	{
		if (!m_pointCloud)
			return;
		m_pointCloud->Clear();
		m_render->Clear();
	}
}