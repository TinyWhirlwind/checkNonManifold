#ifndef _SIMILARITY_
#define _SIMILARITY_

#include "quaternion.h"
#include "matrix44.h"

namespace sn3DCore
{
template <class S,class RotationType = Quaternion<S> > class Similarity 
{
public:
  Similarity() {}
  Similarity(const RotationType &q) { SetRotate(q); }
  Similarity(const Point3<S> &p) { SetTranslate(p); }
  Similarity(S s) { SetScale(s); }
  Similarity(S alpha, S beta, S gamma)
	{
		rot.FromEulerAngles(alpha, beta, gamma);
		tra = Point3<S>(0, 0, 0);
		sca = 1;
	}
  
  Similarity operator*(const Similarity &affine) const;
  Similarity &operator*=(const Similarity &affine);
  //Point3<S> operator*(const Point3<S> &p) const;
  
  
  Similarity &SetIdentity();
  Similarity &SetScale(const S s);
	Similarity &SetTranslate(const Point3<S> &t);	
  ///use radiants for angle.
  Similarity &SetRotate(S angle, const Point3<S> & axis); 
  Similarity &SetRotate(const RotationType &q);

  Matrix44<S> Matrix() const;
  Matrix44<S> InverseMatrix() const;
  void FromMatrix(const Matrix44<S> &m);

  RotationType rot;
  Point3<S> tra;
  S sca;  
};

template <class S,class RotationType> Similarity<S,RotationType> &Invert(Similarity<S,RotationType> &m);
template <class S,class RotationType> Similarity<S,RotationType> Inverse(const Similarity<S,RotationType> &m);
template <class S,class RotationType> Point3<S> operator*(const Similarity<S,RotationType> &m, const Point3<S> &p);


template <class S,class RotationType> Similarity<S,RotationType> Similarity<S,RotationType>::operator*(const Similarity &a) const {
  Similarity<S,RotationType> r;
  r.rot = rot * a.rot;
  r.sca = sca * a.sca;
  r.tra = (rot.Rotate(a.tra)) * sca + tra;
  return r;
}

template <class S,class RotationType> Similarity<S,RotationType> &Similarity<S,RotationType>::operator*=(const Similarity &a) {  
  rot = rot * a.rot;
  sca = sca * a.sca;
  tra = (rot.Rotate(a.tra)) * sca + tra;
  return *this;
}
  
template <class S,class RotationType> Similarity<S,RotationType> &Similarity<S,RotationType>::SetIdentity() {
  rot.SetIdentity();
  tra = Point3<S>(0, 0, 0);
  sca = 1;
  return *this;
}

template <class S,class RotationType> Similarity<S,RotationType> &Similarity<S,RotationType>::SetScale(const S s) {
  SetIdentity();
  sca = s;
  return *this;
}

template <class S,class RotationType> Similarity<S,RotationType> &Similarity<S,RotationType>::SetTranslate(const Point3<S> &t) {
  SetIdentity();
  tra = t;
  return *this;
}

template <class S,class RotationType> Similarity<S,RotationType> &Similarity<S,RotationType>::SetRotate(S angle, const Point3<S> &axis) {
  SetIdentity();
  rot.FromAxis(angle, axis);
  return *this;
}

template <class S,class RotationType> Similarity<S,RotationType> &Similarity<S,RotationType>::SetRotate(const RotationType &q) {
  SetIdentity();
  rot = q;  
  return *this;
}


template <class S,class RotationType> Matrix44<S> Similarity<S,RotationType>::Matrix() const {
  Matrix44<S> r;
  rot.ToMatrix(r);
  Matrix44<S> s = Matrix44<S>().SetScale(sca, sca, sca);
  Matrix44<S> t = Matrix44<S>().SetTranslate(tra[0], tra[1], tra[2]);
  return s*r*t;  // scale * rot * trans
}

template <class S,class RotationType> Matrix44<S> Similarity<S,RotationType>::InverseMatrix() const {
  return Inverse(Matrix());
}


template <class S,class RotationType> void Similarity<S,RotationType>::FromMatrix(const Matrix44<S> &m) {
  sca = (S)pow(m.Determinant(), 1/3);  
  assert(sca != 0);
  Matrix44<S> t = m * Matrix44<S>().SetScale(1/sca, 1/sca, 1/sca);
  rot.FromMatrix(t);
  tra[0] = t.ElementAt(3, 0);
  tra[1] = t.ElementAt(3, 1);
  tra[2] = t.ElementAt(3, 2);
}

template <class S,class RotationType> Similarity<S,RotationType> &Invert(Similarity<S,RotationType> &a) {  
  a.rot.Invert();
  a.sca = 1/a.sca;
  a.tra = a.rot.Rotate(-a.tra)*a.sca;
  return a;
}

template <class S,class RotationType> Similarity<S,RotationType> Inverse(const Similarity<S,RotationType> &m) {
  Similarity<S,RotationType> a = m;
  return Invert(a);
}


template <class S,class RotationType> Similarity<S,RotationType> Interpolate(const Similarity<S,RotationType> &a, const Similarity<S,RotationType> &b, const S t) {
  Similarity<S,RotationType> r;
  r.rot = interpolate(a.rot, b.rot, t);
  r.tra = t * a.tra + (1-t) * b.tra;
  r.sca = t * a.sca + (1-t) * b.sca;
  return r;
}

template <class S,class RotationType> Point3<S> operator*(const Similarity<S,RotationType> &m, const Point3<S> &p) {
  Point3<S> r = m.rot.Rotate(p);
  r *= m.sca;
  r += m.tra;
  return r;
}



class Similarityf:public Similarity<float>{};

class Similarityd:public Similarity<double>{};
}


#endif
