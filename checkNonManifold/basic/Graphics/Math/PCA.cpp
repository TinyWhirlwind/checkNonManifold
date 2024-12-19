#include "stdafx.h"
#include "PCA.h"
#include <math.h>

#pragma warning (disable : 4244)

void PCA::Compute(const std::vector<Point3f>& points, PCAInfo& info)
{
	int np = points.size();
	std::vector<float> vecX(np), vecY(np), vecZ(np);
	for (int i = 0; i < np; ++i)
	{
		vecX[i] = points[i].X();
		vecY[i] = points[i].Y();
		vecZ[i] = points[i].Z();
	}

	PCA pca;
	pca.Compute(&vecX[0], &vecY[0], &vecZ[0], np, &info.center[0], &info.mainComponent[0], &info.secondComponent[0], &info.thirdComponent[0]);
}

/// 输入为 x ,y ,z ,n 分别为 模型点集的坐标和顶点数,输出中心点与三个特征方向(单位向量)
void PCA::Compute(float x[], float y[], float z[], int n, float center[3], float mainComponent[3], float secondComponent[3], float thirdComponent[3])
{
	float coxMat[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			coxMat[i][j] = 0.0f;

	/////////////////////// 第一步: 计算中心点
	float xMean, yMean, zMean;

	xMean = yMean = zMean = 0.0;

	for (int i = 0; i < n; i++)
	{
		xMean += x[i]; yMean += y[i]; zMean += z[i];
	}

	xMean /= n; yMean /= n; zMean /= n;

	center[0] = xMean;
	center[1] = yMean;
	center[2] = zMean;

	////////////////////////  第二步 :   计算协方差矩阵
	PCAMat pointOrigin(0.0, n, 3);
	PCAMat pointSubed(0.0, n, 3);
	PCAMat pointSubTran(0.0, 3, n);

	for (int i = 0; i < n; i++)
	{
		pointOrigin[i][0] = x[i];
		pointOrigin[i][1] = y[i];
		pointOrigin[i][2] = z[i];

		pointSubed[i][0] = x[i] - xMean;
		pointSubed[i][1] = y[i] - yMean;
		pointSubed[i][2] = z[i] - zMean;
	}

	for (int i = 0; i < n; i++)
	{
		pointSubTran[0][i] = pointSubed[i][0];
		pointSubTran[1][i] = pointSubed[i][1];
		pointSubTran[2][i] = pointSubed[i][2];
	}

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			float mul = 0.0;
			for (int k = 0; k < n; k++)
			{
				mul += (pointSubTran[i][k] * pointSubed[k][j]);
			}

			coxMat[i][j] = mul;
		}

	float maxMat = fabsf(coxMat[0][0]);
	for (int i = 0; i < 3; i++)
	{
		if (fabsf(coxMat[i][i]) > maxMat)
			maxMat = fabsf(coxMat[i][i]);
	}

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			coxMat[i][j] /= maxMat;
		}

	/////////////  第三步 : 计算协方差矩阵的特征值和特征方向
	PCAMat m(0.0, 3, 3);
	PCAVec  vecEigenvalue(0.0, 3);
	PCAMat  matEigenvector(0.0, 3, 3);
	int nrot;

	m[0][0] = coxMat[0][0]; m[0][1] = coxMat[0][1]; m[0][2] = coxMat[0][2];
	m[1][0] = coxMat[1][0]; m[1][1] = coxMat[1][1]; m[1][2] = coxMat[1][2];
	m[2][0] = coxMat[2][0]; m[2][1] = coxMat[2][1]; m[2][2] = coxMat[2][2];

	jacobi(m, vecEigenvalue, matEigenvector, nrot);
	eigsrt(vecEigenvalue, matEigenvector);

	//保存主元方向
	mainComponent[0] = matEigenvector[0][0];
	mainComponent[1] = matEigenvector[1][0];
	mainComponent[2] = matEigenvector[2][0];

	secondComponent[0] = matEigenvector[0][1];
	secondComponent[1] = matEigenvector[1][1];
	secondComponent[2] = matEigenvector[2][1];

	thirdComponent[0] = matEigenvector[0][2];
	thirdComponent[1] = matEigenvector[1][2];
	thirdComponent[2] = matEigenvector[2][2];
}

void PCA::Compute(float x[], float y[], float z[], int n, float center[3], float eigen[3], float mainComponent[3], float secondComponent[3], float thirdComponent[3])
{
	float coxMat[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			coxMat[i][j] = 0.0f;

	/////////////////////// 第一步: 计算中心点
	float xMean, yMean, zMean;

	xMean = yMean = zMean = 0.0;

	for (int i = 0; i < n; i++)
	{
		xMean += x[i]; yMean += y[i]; zMean += z[i];
	}

	xMean /= n; yMean /= n; zMean /= n;

	center[0] = xMean;
	center[1] = yMean;
	center[2] = zMean;

	////////////////////////  第二步 :   计算协方差矩阵
	PCAMat pointOrigin(0.0, n, 3);
	PCAMat pointSubed(0.0, n, 3);
	PCAMat pointSubTran(0.0, 3, n);

	for (int i = 0; i < n; i++)
	{
		pointOrigin[i][0] = x[i];
		pointOrigin[i][1] = y[i];
		pointOrigin[i][2] = z[i];

		pointSubed[i][0] = x[i] - xMean;
		pointSubed[i][1] = y[i] - yMean;
		pointSubed[i][2] = z[i] - zMean;
	}

	for (int i = 0; i < n; i++)
	{
		pointSubTran[0][i] = pointSubed[i][0];
		pointSubTran[1][i] = pointSubed[i][1];
		pointSubTran[2][i] = pointSubed[i][2];
	}

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			float mul = 0.0;
			for (int k = 0; k < n; k++)
			{
				mul += (pointSubTran[i][k] * pointSubed[k][j]);
			}

			coxMat[i][j] = mul;
		}

	float maxMat = fabsf(coxMat[0][0]);
	for (int i = 0; i < 3; i++)
	{
		if (fabsf(coxMat[i][i]) > maxMat)
			maxMat = fabsf(coxMat[i][i]);
	}

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			coxMat[i][j] /= maxMat;
		}

	/////////////  第三步 : 计算协方差矩阵的特征值和特征方向
	PCAMat m(0.0, 3, 3);
	PCAVec  vecEigenvalue(0.0, 3);
	PCAMat  matEigenvector(0.0, 3, 3);
	int nrot;

	m[0][0] = coxMat[0][0]; m[0][1] = coxMat[0][1]; m[0][2] = coxMat[0][2];
	m[1][0] = coxMat[1][0]; m[1][1] = coxMat[1][1]; m[1][2] = coxMat[1][2];
	m[2][0] = coxMat[2][0]; m[2][1] = coxMat[2][1]; m[2][2] = coxMat[2][2];

	jacobi(m, vecEigenvalue, matEigenvector, nrot);
	eigsrt(vecEigenvalue, matEigenvector);

	eigen[0] = vecEigenvalue[0];
	eigen[1] = vecEigenvalue[1];
	eigen[2] = vecEigenvalue[2];

	//保存主元方向
	mainComponent[0] = matEigenvector[0][0];
	mainComponent[1] = matEigenvector[1][0];
	mainComponent[2] = matEigenvector[2][0];

	secondComponent[0] = matEigenvector[0][1];
	secondComponent[1] = matEigenvector[1][1];
	secondComponent[2] = matEigenvector[2][1];

	thirdComponent[0] = matEigenvector[0][2];
	thirdComponent[1] = matEigenvector[1][2];
	thirdComponent[2] = matEigenvector[2][2];
}

void PCA::rot(PCAMat& a, const double s, const double tau, const int i, const int j, const int k, const int l)
{
	double g, h;

	g = a[i][j];
	h = a[k][l];
	a[i][j] = g - s * (h + g * tau);
	a[k][l] = h + s * (g - h * tau);
}

void PCA::jacobi(PCAMat& a, PCAVec& d, PCAMat& v, int& nrot)
{
	int i, j, ip, iq;
	double tresh, theta, tau, t, sm, s, h, g, c;

	int n = d.size();
	PCAVec b(n), z(n);
	for (ip = 0; ip < n; ip++)
	{
		for (iq = 0; iq < n; iq++) v[ip][iq] = 0.0;
		v[ip][ip] = 1.0;
	}
	for (ip = 0; ip < n; ip++)
	{
		b[ip] = d[ip] = a[ip][ip];
		z[ip] = 0.0;
	}
	nrot = 0;
	for (i = 1; i <= 50; i++)
	{
		sm = 0.0;
		for (ip = 0; ip < n - 1; ip++)
		{
			for (iq = ip + 1; iq < n; iq++)
				sm += fabs(a[ip][iq]);
		}
		if (sm == 0.0)
			return;
		if (i < 4)
			tresh = 0.2 * sm / (n * n);
		else
			tresh = 0.0;
		for (ip = 0; ip < n - 1; ip++)
		{
			for (iq = ip + 1; iq < n; iq++)
			{
				g = 100.0 * fabs(a[ip][iq]);
				if (i > 4 && (fabs(d[ip]) + g) == fabs(d[ip])
					&& (fabs(d[iq]) + g) == fabs(d[iq]))
					a[ip][iq] = 0.0;
				else if (fabs(a[ip][iq]) > tresh)
				{
					h = d[iq] - d[ip];
					if ((fabs(h) + g) == fabs(h))
						t = (a[ip][iq]) / h;
					else
					{
						theta = 0.5 * h / (a[ip][iq]);
						t = 1.0 / (fabs(theta) + sqrt(1.0 + theta * theta));
						if (theta < 0.0) t = -t;
					}
					c = 1.0 / sqrt(1 + t * t);
					s = t * c;
					tau = s / (1.0 + c);
					h = t * a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq] = 0.0;
					for (j = 0; j < ip; j++)
						rot(a, s, tau, j, ip, j, iq);
					for (j = ip + 1; j < iq; j++)
						rot(a, s, tau, ip, j, j, iq);
					for (j = iq + 1; j < n; j++)
						rot(a, s, tau, ip, j, iq, j);
					for (j = 0; j < n; j++)
						rot(v, s, tau, j, ip, j, iq);
					++nrot;
				}
			}
		}
		for (ip = 0; ip < n; ip++)
		{
			b[ip] += z[ip];
			d[ip] = b[ip];
			z[ip] = 0.0;
		}
	}
}

void PCA::eigsrt(PCAVec& d, PCAMat& v)
{
	int i, j, k;
	double p;

	int n = d.size();
	for (i = 0; i < n - 1; i++)
	{
		p = d[k = i];
		for (j = i; j < n; j++)
			if (d[j] >= p) p = d[k = j];
		if (k != i)
		{
			d[k] = d[i];
			d[i] = p;
			for (j = 0; j < n; j++)
			{
				p = v[j][i];
				v[j][i] = v[j][k];
				v[j][k] = p;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
PCAMat::PCAMat(const double& a, int n, int m) : nn(n), mm(m), v(new  double* [n])
{
	int i, j;
	v[0] = new double[m * n];
	for (i = 1; i < n; i++)
		v[i] = v[i - 1] + m;
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++)
			v[i][j] = a;
}

inline double* PCAMat::operator[](const int i)
{
	return v[i];
}

PCAMat::~PCAMat()
{
	if (v != 0)
	{
		delete[](v[0]);
		delete[](v);
	}
}

PCAVec::PCAVec(int n) : nn(n), v(new double[n])
{
}

PCAVec::PCAVec(const  double& a, int n) : nn(n), v(new double[n])
{
	for (int i = 0; i < n; i++)
		v[i] = a;
}

inline double& PCAVec::operator[](const int i)
{
	return v[i];
}

inline int PCAVec::size() const
{
	return nn;
}

PCAVec::~PCAVec()
{
	if (v != 0)
		delete[](v);
}