#ifndef _CANVAS_TRANSFORMATIONMATRIX_H_
#define _CANVAS_TRANSFORMATIONMATRIX_H_

#include <Point.h>
#include <cmath>

namespace canvas {
  class TransformationMatrix {
  public:
    TransformationMatrix() : a(1.0), b(0.0), c(0.0), d(1.0), e(0.0), f(0.0) { }
    TransformationMatrix(double _a, double _b, double _c, double _d, double _e, double _f)
      : a(_a), b(_b), c(_c), d(_d), e(_e), f(_f) { }
    
    TransformationMatrix operator* (const TransformationMatrix & other) {
      return multiply(*this, other);    
    }
    
    const TransformationMatrix & operator*= (const TransformationMatrix & other) {
      *this = multiply(*this, other);
      return *this;
    }
    
    Point multiply(double x, double y) const {
      return Point( x * a + y * c + e,
		    x * b + y * d + f
		    );    
    }
    
    Point multiply(const Point & p) const {
      return multiply(p.x, p.y);
    }
    
    double transformAngle(double alpha) const {
      double x = cos(alpha), y = sin(alpha);
      return atan2(x * b + y * d, x * a + y * c);
    }

    double transformSize(double s, bool use_sqrt = false) const {
      if (use_sqrt) return s * sqrt(a);
      else return s * a;
    }
    
  private:
    static TransformationMatrix multiply(const TransformationMatrix & A, const TransformationMatrix & B) {
      return TransformationMatrix( A.a * B.a + A.c * B.b,
				   A.b * B.a + A.d * B.b,
				   A.a * B.c + A.c * B.d,
				   A.b * B.c + A.d * B.d,
				   A.a * B.e + A.c * B.f + A.e,
				   A.b * B.e + A.d * B.f + A.f
				   );
    }
    
    double a, b, c, d, e, f;
  };
};

#endif
