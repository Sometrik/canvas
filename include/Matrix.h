#ifndef _CANVAS_MATRIX_H_
#define _CANVAS_MATRIX_H_

class Matrix {
 public:
  Matrix(double _a, double _b, double _c, double _d, double _e, double _f)
    : a(_a), b(_b), c(_c), d(_d), e(_e), f(_f) { }

 private:
  double a, b, c, d, e, f;
};

#endif
