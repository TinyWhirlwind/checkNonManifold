#ifndef  _MatrixEigen_
#define  _MatrixEigen_

/* MatrixEigen
*  矩阵特征值计算
*/
class MatrixEigen
{
public:
	MatrixEigen();
	~MatrixEigen() {}

	// 特征值分解
	void Decomposition(float mat[], int n);
	void Clear();

	// 特征向量
	float* eigvec1;
	float* eigvec2;
	float* eigvec3;

	// 特征值
	float eigval1;
	float eigval2;
	float eigval3;

	class Mat
	{
	private:
		int nn;
		int mm;
		double** v;
	public:

		Mat(const double& a, int n, int m);
		inline  double* operator[](const int i);
		~Mat();
	};

	class Vec
	{
	private:
		int nn;
		double* v;
	public:
		explicit Vec(int n);
		Vec(const double& a, int n);
		inline double& operator[](const int i);
		inline int size() const;
		~Vec();
	};

private:
	void rot(Mat& a, const double s, const double tau, const int i, const int j, const int k, const int l);
	void jacobi(Mat& a, Vec& d, Mat& v, int& nrot);
	void eigsrt(Vec& d, Mat& v);
};
#endif