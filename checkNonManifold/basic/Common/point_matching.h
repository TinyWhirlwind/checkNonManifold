#ifndef _POINTMATCHING_
#define _POINTMATCHING_

#include "matrix33.h"
#include "quaternion.h"
#include "lin_algebra.h"

template<class ScalarType> 
class PointMatching 
{
public:
  typedef Point3<ScalarType> Point3x;
  typedef Matrix33<ScalarType> Matrix33x;
  typedef Matrix44<ScalarType> Matrix44x;
  typedef Quaternion<ScalarType> Quaternionx;


/*
Compute a similarity matching (rigid + uniform scaling)
simply create a temporary point set with the correct scaling factor


*/ 
static bool ComputeSimilarityMatchMatrix(		Matrix44x &res,
                                    std::vector<Point3x> &Pfix,		// vertici corrispondenti su fix (rossi)
						std::vector<Point3x> &Pmov) 		// normali scelti su mov (verdi)
{
	Quaternionx qtmp;
	Point3x tr;
	
	std::vector<Point3x> Pnew(Pmov.size());
	
	ScalarType scalingFactor=0;
	
	for(size_t i=0;i<( Pmov.size()-1);++i)
	{
			scalingFactor += Distance(Pmov[i],Pmov[i+1])/ Distance(Pfix[i],Pfix[i+1]);
			qDebug("Scaling Factor is %f",scalingFactor/(i+1));
	}
	scalingFactor/=(Pmov.size()-1);

	for(size_t i=0;i<Pmov.size();++i)
		Pnew[i]=Pmov[i]/scalingFactor;		
		
	
	bool ret=ComputeRigidMatchMatrix(res,Pfix,Pnew,qtmp,tr);
	if(!ret) return false;
	Matrix44x scaleM; scaleM.SetDiagonal(1.0/scalingFactor);
	
	res = res * scaleM;
	return true;
}



static bool ComputeRigidMatchMatrix(		Matrix44x &res,
                                    std::vector<Point3x> &Pfix,		// vertici corrispondenti su fix (rossi)
						std::vector<Point3x> &Pmov) 		// normali scelti su mov (verdi)
{
	Quaternionx qtmp;
	Point3x tr;
	return ComputeRigidMatchMatrix(res,Pfix,Pmov,qtmp,tr);
}

static bool ComputeRigidMatchMatrix(std::vector<ScalarType> weights,
									Matrix44x &res,
 									std::vector<Point3x> &Pfix,		// vertici corrispondenti su fix (rossi)
									std::vector<Point3x> &Pmov) 		// normali scelti su mov (verdi)
{
	Quaterniond qtmp;
	Point3d tr;
	return ComputeRigidMatchMatrix(res,Pfix,Pmov,qtmp,tr);
}


/* 
Calcola la matrice di rototraslazione 
che porta i punti Pmov su Pfix

Basata sul paper 

Besl, McKay
A method for registration o f 3d Shapes 
IEEE TPAMI Vol 14, No 2 1992


	Esempio d'uso 
			const int np=1000;
			std::vector<Point3x> pfix(np),pmov(np);

			Matrix44x Rot,Trn,RotRes;
			Rot.Rotate(30,Point3x(1,0,1));
			Trn.Translate(0,0,100);
			Rot=Trn*Rot;
			
			for(int i=0;i<np;++i){
				pfix[i]=Point3x(-150+rand()%1000,-150+rand()%1000,0);
				pmov[i]=Rot.Apply(pfix[i]);
			}
			
			ComputeRigidMatchMatrix(RotRes,pfix,pmov);
      
			RotRes.Invert();
			assert( RotRes==Rot);  
			assert( RotRes.Apply(pmov[i]) == pfix[i] );
			
*/
static
bool ComputeWeightedRigidMatchMatrix(Matrix44x &res,
                  std::vector<Point3x> &Pfix,		
									std::vector<Point3x> &Pmov,
									std::vector<double> weights,
									Quaternionx &q,
									Point3x &tr
									) 	
{

	Matrix33x tmp;
  Matrix33x ccm; 
	Point3x bfix,bmov; // baricenter of src e trg
	ccm.WeightedCrossCovariance(weights,Pmov,Pfix,bmov,bfix);
	Matrix33x cyc; // the cyclic components of the cross covariance matrix.

	cyc=ccm;
	tmp=ccm;
	tmp.Trasp();
	cyc-=tmp;

	Matrix44x QQ;
	QQ.Zero();
	Point3x D(cyc[1][2],cyc[2][0],cyc[0][1]);

  Matrix33x RM;
	RM.Zero();
	RM[0][0]=-ccm.Trace();
  RM[1][1]=-ccm.Trace();
  RM[2][2]=-ccm.Trace();
  RM+=ccm;
	ccm.Trasp();
	RM+=ccm;

	QQ[0][0] = ccm.Trace();
	QQ[0][1] = D[0]; QQ[0][2] = D[1]; QQ[0][3] = D[2];
	QQ[1][0] = D[0]; QQ[2][0] = D[1];	QQ[3][0] = D[2];

	int i,j;
  for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			QQ[i+1][j+1]=RM[i][j];

//  printf(" Quaternion Matrix\n");
//	print(QQ);
	Point4d d;
  Matrix44x v;
	int nrot;
	QQ.Jacobi(d,v,nrot);
//	printf("Done %i iterations\n %f %f %f %f\n",nrot,d[0],d[1],d[2],d[3]);
//	print(v);
	// Now search the maximum eigenvalue
	double maxv=0;
	int maxind=-1;
  for(i=0;i<4;i++)
		if(maxv<fabs(d[i])) {
			q=Quaternionx(v[0][i],v[1][i],v[2][i],v[3][i]);
			maxind=i;
			maxv=d[i];
		}
  // The corresponding eigenvector define the searched rotation,
		Matrix44x Rot;
	q.RotMatrix(Rot);
  // the translation (last row) is simply the difference between the transformed src barycenter and the trg baricenter
	tr= (bfix - Rot.Apply(bmov));
	//res[3][0]=tr[0];res[3][1]=tr[1];res[3][2]=tr[2];
	Matrix44x Trn;
	Trn.Translate(tr);
		
	res=Rot*Trn;
	return true;
}

static
bool ComputeRigidMatchMatrix(Matrix44x &res,
 						std::vector<Point3x> &Pfix,		
						std::vector<Point3x> &Pmov,
							Quaternionx &q,
							Point3x &tr) 	
{

	Matrix33x tmp;
  Matrix33x ccm; 
	Point3x bfix,bmov; // baricenter of src e trg
	ccm.CrossCovariance(Pmov,Pfix,bmov,bfix);
	Matrix33x cyc; // the cyclic components of the cross covariance matrix.

	cyc=ccm;
	tmp=ccm;
	tmp.Transpose();
	cyc-=tmp;

	Matrix44x QQ;
	QQ.SetZero();
	Point3x D(cyc[1][2],cyc[2][0],cyc[0][1]);

  Matrix33x RM;
	RM.SetZero();
	RM[0][0]=-ccm.Trace();
  RM[1][1]=-ccm.Trace();
  RM[2][2]=-ccm.Trace();
  RM+=ccm;
	ccm.Transpose();
	RM+=ccm;

	QQ[0][0] = ccm.Trace();
	QQ[0][1] = D[0]; QQ[0][2] = D[1]; QQ[0][3] = D[2];
	QQ[1][0] = D[0]; QQ[2][0] = D[1];	QQ[3][0] = D[2];

	int i,j;
  for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			QQ[i+1][j+1]=RM[i][j];

//  printf(" Quaternion Matrix\n");
//	print(QQ);
	Point4d d;
  Matrix44x v;
	int nrot;
	//QQ.Jacobi(d,v,nrot);
	Jacobi(QQ,d,v,nrot);
//	printf("Done %i iterations\n %f %f %f %f\n",nrot,d[0],d[1],d[2],d[3]);
//	print(v);
	// Now search the maximum eigenvalue
	double maxv=0;
	int maxind=-1;
  for(i=0;i<4;i++)
		if(maxv<fabs(d[i])) {
			q=Quaternionx(v[0][i],v[1][i],v[2][i],v[3][i]);
			maxind=i;
			maxv=d[i];
		}
  // The corresponding eigenvector define the searched rotation,
		Matrix44x Rot;
	q.ToMatrix(Rot);
  Transpose(Rot);
  // the translation (last row) is simply the difference between the transformed src barycenter and the trg baricenter
	tr= (bfix - Rot*bmov);
	//res[3][0]=tr[0];res[3][1]=tr[1];res[3][2]=tr[2];
	Matrix44x Trn;
	Trn.SetTranslate(tr);
		
	res=Trn*Rot;
	return true;
}

// Dati due insiemi di punti e normali corrispondenti calcola la migliore trasformazione 
// che li fa corrispondere
static bool ComputeMatchMatrix(		Matrix44x &res,
 						std::vector<Point3x> &Ps,		// vertici corrispondenti su src (rossi)
						std::vector<Point3x> &Ns, 		// normali corrispondenti su src (rossi)
						std::vector<Point3x> &Pt)		// vertici scelti su trg (verdi) 
//						vector<Point3x> &Nt) 		// normali scelti su trg (verdi)
{
	int sz=Ps.size();
  assert(0);
  // Da qui in poi non compila che ha bisogno dei minimiquadrati
#if 0

	Matrix<double> A(sz,12);
	Vector<double> b(sz);
	Vector<double> x(12);

	//inizializzo il vettore per minimi quadrati
	// la matrice di trasf che calcolo con LeastSquares cerca avvicinare il piu' 
	// possibile le coppie di punti che trovo ho scelto  
	// Le coppie di punti sono gia' trasformate secondo la matrice <In> quindi come scelta iniziale 
	// per il metodo minimiquadrati scelgo l'identica (e.g. se ho allineato a mano perfettamente e 
	// le due mesh sono perfettamente uguali DEVE restituire l'identica)
	
	res.SetIdentity();
	int i,j,k;
	for(i=0; i<=2; ++i)
		for(j=0; j<=3; ++j)
			x[i*4+j] = res[i][j];


	//costruzione della matrice
	for(i=0;i<sz;++i)
	{
		for(j=0;j<3;++j)
			for(k=0;k<4;++k)
				if(k<3)
				{
					A[i][k+j*4] = Ns[i][j]*Pt[i][k];
				}
				else
				{
					A[i][k+j*4] = Ns[i][j];
				}
		b[i] = Ps[i]*Ns[i];
	}
	const int maxiter = 4096;
	int iter;
	LSquareGC(x,A,b,1e-16,maxiter,iter);
	
	TRACE("LSQ Solution");
	for(int ind=0; ind<12; ++ind) {
		if((ind%4)==0) TRACE("\n");
		TRACE("%8.5lf ", x[ind]); 
	} TRACE("\n");

	if(iter==maxiter)
	{
		TRACE("I minimi quadrati non convergono!!\n");
		return false;
	}
	else { TRACE("Convergenza in %d passi\n",iter); }

	//Devo riapplicare la matrice di trasformazione globale a 
	//trg inserendo il risultato nel vettore trgvert contenente 
	//copia dei suoi vertici
	Matrix44x tmp;
	for(i=0; i<=2; ++i)
		for(j=0; j<=3; ++j)
			res[j][i] = x[i*4+j];
	res[0][3] = 0.0;
	res[1][3] = 0.0;
	res[2][3] = 0.0;
	res[3][3] = 1.0;
	
#endif
	return true;
}


};


#endif
