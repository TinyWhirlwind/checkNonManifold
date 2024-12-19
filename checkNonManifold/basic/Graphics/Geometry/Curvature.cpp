#include "Curvature.h"

Curvature::Curvature()
{
	m_Frame = nullptr;
	m_FaceCornerArea = nullptr;
	VertexArea = nullptr;
}
Curvature::~Curvature()
{
	if (m_Frame != nullptr) 
		delete m_Frame;
}
void Curvature::ComputerCurvatrue(std::vector<Face *> & faces, std::vector<Vertex *> & vertices)
{
	if (m_Frame != nullptr) 
		delete m_Frame;
	m_Frame = new LocalFrame[vertices.size()];
	m_FaceCornerArea = new Point3f[faces.size()];
	VertexArea = new float[vertices.size()];
	memset(VertexArea, 0, vertices.size() * sizeof(float));
	ComputeVertexArea(faces, vertices);
	VertexCurvature(faces, vertices);
	delete m_FaceCornerArea;
	delete VertexArea;
}

void Curvature::ComputeVertexArea(std::vector<Face *> & faces, std::vector<Vertex *> & vertices)
{
	Point3f edge[3], faceNorm;
	float   lengthSq[3], edgeWeight[3], faceArea;

	int end = (int)faces.size();
	for (int faceIndex = 0; faceIndex < end; faceIndex++)
	{
		if (!(faces[faceIndex])->IsD()) {
			edge[0] = faces[faceIndex]->P(2) - faces[faceIndex]->P(1);
			edge[1] = faces[faceIndex]->P(0) - faces[faceIndex]->P(2);
			edge[2] = faces[faceIndex]->P(1) - faces[faceIndex]->P(0);

			lengthSq[0] = edge[0].Norm() * edge[0].Norm();
			lengthSq[1] = edge[1].Norm() * edge[1].Norm();
			lengthSq[2] = edge[2].Norm() * edge[2].Norm();

			faceNorm = edge[0] ^ edge[1];
			faceArea = 0.5f * faceNorm.Norm();

			edgeWeight[0] = lengthSq[0] * (lengthSq[1] + lengthSq[2] - lengthSq[0]);
			edgeWeight[1] = lengthSq[1] * (lengthSq[2] + lengthSq[0] - lengthSq[1]);
			edgeWeight[2] = lengthSq[2] * (lengthSq[0] + lengthSq[1] - lengthSq[2]);

			if (edgeWeight[0] <= 0.0f)
			{
				m_FaceCornerArea[faceIndex][1] = -0.25f * lengthSq[2] * faceArea / (edge[0] * edge[2]);
				m_FaceCornerArea[faceIndex][2] = -0.25f * lengthSq[1] * faceArea / (edge[0] * edge[1]);
				m_FaceCornerArea[faceIndex][0] = faceArea - m_FaceCornerArea[faceIndex][1] - m_FaceCornerArea[faceIndex][2];
			}
			else if (edgeWeight[1] <= 0.0f)
			{
				m_FaceCornerArea[faceIndex][2] = -0.25f * lengthSq[0] * faceArea / (edge[1] * edge[0]);
				m_FaceCornerArea[faceIndex][0] = -0.25f * lengthSq[2] * faceArea / (edge[1] * edge[2]);
				m_FaceCornerArea[faceIndex][1] = faceArea - m_FaceCornerArea[faceIndex][2] - m_FaceCornerArea[faceIndex][0];
			}
			else if (edgeWeight[2] <= 0.0f)
			{
				m_FaceCornerArea[faceIndex][0] = -0.25f * lengthSq[1] * faceArea / (edge[2] * edge[1]);
				m_FaceCornerArea[faceIndex][1] = -0.25f * lengthSq[0] * faceArea / (edge[2] * edge[0]);
				m_FaceCornerArea[faceIndex][2] = faceArea - m_FaceCornerArea[faceIndex][0] - m_FaceCornerArea[faceIndex][1];
			}
			else
			{
				const float fScale = 0.5f * faceArea / (edgeWeight[0] + edgeWeight[1] + edgeWeight[2]);
				for (int j = 0; j < 3; j++)
					m_FaceCornerArea[faceIndex][j] = fScale * (edgeWeight[(j + 1) % 3] + edgeWeight[(j + 2) % 3]);
			}

			VertexArea[faces[faceIndex]->V(0)->Index()] += m_FaceCornerArea[faceIndex][0];
			VertexArea[faces[faceIndex]->V(1)->Index()] += m_FaceCornerArea[faceIndex][1];
			VertexArea[faces[faceIndex]->V(2)->Index()] += m_FaceCornerArea[faceIndex][2];
		}
	}
}

void Curvature::VertexCurvature(std::vector<Face *> & faces, std::vector<Vertex *> & vertices)
{
	/* 初始化顶点局部标架 */
	for (int i = 0; i < (int)vertices.size(); i++)
	{
		unsigned int index = vertices[i]->Index();
		if (!vertices[i]->IsD())
		{
			m_Frame[index].uDir = vertices[i]->VFp()->V((vertices[i]->VFi() + 1) % 3)->P() - vertices[i]->P();
			m_Frame[index].normal = vertices[i]->N();
			m_Frame[index].uDir = m_Frame[index].uDir^m_Frame[index].normal;
			m_Frame[index].uDir /= m_Frame[index].uDir.Norm();
			m_Frame[index].vDir = m_Frame[index].normal ^ m_Frame[index].uDir;
		}
	}

	/* 计算面及顶点上曲率张量 */
	LocalFrame faceFrame, vertexFrame;
	Point3f edge[3];
	for (int faceIndex = 0; faceIndex < (int)faces.size(); faceIndex++)
	{
		if (!faces[faceIndex]->IsD()) {
			edge[0] = faces[faceIndex]->P(2) - faces[faceIndex]->P(1);
			edge[1] = faces[faceIndex]->P(0) - faces[faceIndex]->P(2);
			edge[2] = faces[faceIndex]->P(1) - faces[faceIndex]->P(0);

			/* 初始化面上局部标架  */

			faceFrame.uDir = edge[0] / edge[0].Norm();
			faceFrame.normal = edge[0] ^ edge[1];
			faceFrame.normal /= faceFrame.normal.Norm();
			faceFrame.vDir = faceFrame.normal^faceFrame.uDir;
			faceFrame.vDir /= faceFrame.vDir.Norm();

			/* 计算此标架下面上的曲率张量 */
			Point3f m;//方程右边项与求解结果存放在此处
			m.X() = m.Y() = m.Z() = 0;
			double w[3][3];
			for (int temp_i = 0; temp_i < 3; temp_i++)
				for (int temp_j = 0; temp_j < 3; temp_j++)
					w[temp_i][temp_j] = 0;
			Point3f normalDiff;
			for (int j = 0; j < 3; j++)
			{
				float u = edge[j] * faceFrame.uDir;
				float v = edge[j] * faceFrame.vDir;
				w[0][0] += u * u;
				w[0][1] += u * v;
				w[2][2] += v * v;

				normalDiff = faces[faceIndex]->V((j + 2) % 3)->N() - faces[faceIndex]->V((j + 1) % 3)->N();
				float dnu = normalDiff*faceFrame.uDir;
				float dnv = normalDiff*faceFrame.vDir;
				m[0] += dnu * u;
				m[1] += dnu * v + dnv * u;
				m[2] += dnv * v;
			}
			w[1][1] = w[0][0] + w[2][2];
			w[1][2] = w[0][1];
			Point3f diag;
			choldc(w, diag);
			cholsl(w, diag, m, m);

			faceFrame.k1 = m[0];
			faceFrame.k12 = m[1];
			faceFrame.k2 = m[2];

			/* 计算顶点上相应标架下的曲率张量 */
			for (int j = 0; j < 3; j++)
			{
				unsigned int index = faces[faceIndex]->V(j)->Index();

				vertexFrame.normal = m_Frame[index].normal;
				vertexFrame.uDir = m_Frame[index].uDir;
				vertexFrame.vDir = m_Frame[index].vDir;

				ProjectCurvature(faceFrame, vertexFrame);

				float wt = m_FaceCornerArea[faceIndex][j] / VertexArea[index];
				m_Frame[index].k1 += wt*vertexFrame.k1;
				m_Frame[index].k12 += wt*vertexFrame.k12;
				m_Frame[index].k2 += wt*vertexFrame.k2;
			}
		}
	}
	/* 计算顶点主曲率、主方向  */
	for (int i = 0; i < (int)vertices.size(); i++)
	{
		if (!vertices[i]->IsD())
		{
			DiagonalizeCurvature(m_Frame[vertices[i]->Index()]);
		}
	}
}

void Curvature::ProjectCurvature(LocalFrame & faceFrame, LocalFrame & vertexFrame)
{
	Point3f r_uDir, r_vDir;//vertex标架旋转后的u,v单位向量

	/* 将两局部标架旋转到同一平面 */
	r_uDir = vertexFrame.uDir;
	r_vDir = vertexFrame.vDir;

	float ndot = vertexFrame.normal * faceFrame.normal;
	if (ndot <= -1.0f)
	{
		r_uDir = -r_uDir;
		r_vDir = -r_vDir;
	}
	else {
		Point3f perp_old = faceFrame.normal - vertexFrame.normal * ndot;
		Point3f dperp = (vertexFrame.normal + faceFrame.normal) * (1.0f / (1 + ndot));
		r_uDir = r_uDir - dperp *(r_uDir * perp_old);
		r_vDir = r_vDir - dperp *(r_vDir * perp_old);
	}

	/* 在同一平面内变换曲率张量  */
	float uu = r_uDir * faceFrame.uDir;
	float uv = r_uDir * faceFrame.vDir;
	float vu = r_vDir * faceFrame.uDir;
	float vv = r_vDir * faceFrame.vDir;

	vertexFrame.k1 = faceFrame.k1 * uu * uu + faceFrame.k12 * (2.0f  * uu * uv) + faceFrame.k2* uv * uv;
	vertexFrame.k12 = faceFrame.k1 * uu * vu + faceFrame.k12 * (uu * vv + vu * uv) + faceFrame.k2 * uv * vv;
	vertexFrame.k2 = faceFrame.k1 * vu * vu + faceFrame.k12 * (2.0f  * vu * vv) + faceFrame.k2 * vv * vv;
}

void Curvature::DiagonalizeCurvature(LocalFrame & frame)
{
	Point3f pdir1, pdir2;
	float k1, k2;
	float c = 1, s = 0, tt = 0;
	if (frame.k12 != 0.0f)
	{
		const float h = 0.5f * (frame.k2 - frame.k1) / frame.k12;
		tt = (h < 0.0f) ? 1.0f / (h - sqrt(1.0f + h * h)) : 1.0f / (h + sqrt(1.0f + h * h));
		c = 1.0f / sqrt(1.0f + tt * tt);
		s = tt * c;
	}
	k1 = frame.k1 - tt * frame.k12;
	k2 = frame.k2 + tt * frame.k12;
	if (k1 >= k2)
	{
		pdir1 = frame.uDir * c - frame.vDir * s;
	}
	else
	{
		float temp = k1;
		k1 = k2; k2 = temp;

		pdir1 = frame.uDir * s + frame.vDir * c;
	}
	pdir2 = frame.normal^pdir1;
	frame.k1 = k1;
	frame.k2 = k2;
	frame.k12 = 0;
	frame.uDir = pdir1;
	frame.vDir = pdir2;
}

void Curvature::choldc(double a[][3], Point3f & p)
{
	int i, j, k;
	float sum;
	for (i = 0; i < 3; i++) 
	{
		for (j = i; j < 3; j++) 
		{
			for (sum = a[i][j], k = i - 1; k >= 0; k--) sum -= a[i][k] * a[j][k];
			if (i == j) p[i] = sqrt(sum);
			else a[j][i] = sum / p[i];
		}
	}
}

void Curvature::cholsl(double a[][3], Point3f &p, Point3f &b, Point3f &x)
{
	int i, k;
	float sum;
	for (i = 0; i < 3; i++) {
		for (sum = b[i], k = i - 1; k >= 0; k--) sum -= a[i][k] * x[k];
		x[i] = sum / p[i];
	}
	for (i = 2; i >= 0; i--) {
		for (sum = x[i], k = i + 1; k < 3; k++) sum -= a[k][i] * x[k];
		x[i] = sum / p[i];
	}
}
