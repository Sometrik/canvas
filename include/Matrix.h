#ifndef _CANVAS_MATRIX_H_
#define _CANVAS_MATRIX_H_

class Matrix {
 public:
 Matrix() : a(1.0), b(0.0), c(0.0), d(1.0), e(0.0), f(0.0) { }
  Matrix(double _a, double _b, double _c, double _d, double _e, double _f)
    : a(_a), b(_b), c(_c), d(_d), e(_e), f(_f) { }

  Matrix operator* (const Matrix & other) {
    Matrix r;
    return r;
  }

  const Matrix & operator*= (const Matrix & other) {
    return *this;
  }
  
 private:
  double a, b, c, d, e, f;
};

#endif
