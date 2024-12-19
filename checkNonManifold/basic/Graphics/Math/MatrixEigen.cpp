#include "MatrixEigen.h"
#include <math.h>

#pragma warning (disable : 4244)

MatrixEigen::MatrixEigen()
{
	eigvec1 = 0;
	eigvec2 = 0;
	eigvec3 = 0;
}

void MatrixEigen::Clear()
{
	if (eigvec1 != 0) delete eigvec1;
	if (eigvec2 != 0) delete eigvec2;
	if (eigvec3 != 0) delete eigvec3;

	eigvec1 = 0;
	eigvec2 = 0;
	eigvec3 = 0;
}

void MatrixEigen::Decomposition(float mat[], int n)
{
	Mat m(0.0, n, n);
	Vec  vecEigenvalue(0.0, n);
	Mat  matEigenvector(0.0, n, n);
	int nrot;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			m[i][j] = mat[i * n + j];
		}
	}

	jacobi(m, vecEigenvalue, matEigenvector, nrot);
	eigsrt(vecEigenvalue, matEigenvector);

	if (eigvec1 != 0) delete eigvec1;
	if (eigvec2 != 0) delete eigvec2;
	if (eigvec3 != 0) delete eigvec3;

	eigvec1 = new float[n];
	eigvec2 = new float[n];
	eigvec3 = new float[n];

	eigval1 = vecEigenvalue[0];
	eigval2 = vecEigenvalue[1];
	eigval3 = vecEigenvalue[2];

	for (int i = 0; i < n; i++)
	{
		eigvec1[i] = matEigenvector[i][0];
		eigvec2[i] = matEigenvector[i][1];
		eigvec3[i] = matEigenvector[i][2];
	}
}

void MatrixEigen::rot(Mat& a, const double s, const double tau, const int i, const int j, const int k, const int l)
{
	double g, h;

	g = a[i][j];
	h = a[k][l];
	a[i][j] = g - s * (h + g * tau);
	a[k][l] = h + s * (g - h * tau);
}

void MatrixEigen::jacobi(Mat& a, Vec& d, Mat& v, int& nrot)
{
	int i, j, ip, iq;
	double tresh, theta, tau, t, sm, s, h, g, c;

	int n = d.size();
	Vec b(n), z(n);
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
			{
				sm += fabs(a[ip][iq]);
			}
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
				{
					a[ip][iq] = 0.0;
				}
				else if (fabs(a[ip][iq]) > tresh)
				{
					h = d[iq] - d[ip];
					if ((fabs(h) + g) == fabs(h))
					{
						t = (a[ip][iq]) / h;
					}
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

void MatrixEigen::eigsrt(Vec& d, Mat& v)
{
	int i, j, k;
	double p;

	int n = d.size();
	for (i = 0; i < n - 1; i++)
	{
		p = d[k = i];
		for (j = i; j < n; j++)
		{
			if (d[j] >= p) p = d[k = j];
		}
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

MatrixEigen::Mat::Mat(const double& a, int n, int m) : nn(n), mm(m), v(new  double* [n])
{
	int i, j;
	v[0] = new double[m * n];
	for (i = 1; i < n; i++)
	{
		v[i] = v[i - 1] + m;
	}
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			v[i][j] = a;
		}
	}
}

inline double* MatrixEigen::Mat::operator[](const int i)
{
	return v[i];
}

MatrixEigen::Mat::~Mat()
{
	if (v != 0)
	{
		delete[](v[0]);
		delete[](v);
	}
}

MatrixEigen::Vec::Vec(int n) : nn(n), v(new double[n])
{
}

MatrixEigen::Vec::Vec(const  double& a, int n) : nn(n), v(new double[n])
{
	for (int i = 0; i < n; i++)
	{
		v[i] = a;
	}
}

inline double& MatrixEigen::Vec::operator[](const int i)
{
	return v[i];
}

inline int MatrixEigen::Vec::size() const
{
	return nn;
}

MatrixEigen::Vec::~Vec()
{
	if (v != 0)
	{
		delete[](v);
	}
}