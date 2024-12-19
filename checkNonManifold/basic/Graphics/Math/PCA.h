#ifndef  _PCA_
#define  _PCA_

/* 该类利用主元分析 Principle Component Analysis 
   对模型进行分析,确定其三个主方向         
 */

#include <vector>
#include "Common/Point3.h"

class PCAMat {
private:
	int nn;
	int mm;
	double **v;
public:

	PCAMat(const double &a, int n, int m);	
	inline  double* operator[](const int i);
	~PCAMat();
};

class PCAVec {
private:
	int nn;	
	double *v;
public:
	explicit PCAVec(int n);		
	PCAVec(const double &a, int n);
	inline double & operator[](const int i);	
	inline int size() const;
	~PCAVec();
};

struct PCAInfo
{
	Point3f center;
	Point3f mainComponent;
	Point3f secondComponent;
	Point3f thirdComponent;
};
class PCA
{
public:
	static void Compute(const std::vector<Point3f>& points, PCAInfo& info);

public:   
	void Compute(float x[],float y[],float z[],int n,float center[3],
		float mainComponent[3],float secondComponent[3],float thirdComponent[3]);//主元分析
	void Compute(float x[],float y[],float z[],int n,float center[3],
		float eigen[3],float mainComponent[3],float secondComponent[3],float thirdComponent[3]);
	void rot(PCAMat &a, const double s, const double tau, const int i,const int j, const int k, const int l);
    void jacobi(PCAMat &a, PCAVec &d, PCAMat &v, int &nrot);
    void eigsrt(PCAVec &d, PCAMat &v);
};


#endif