#ifndef _PATH_H_
#define _PATH_H_

#include <vector>

namespace canvas {
  class Point {
  public:
  Point(double _x, double _y) : x(_x), y(_y) { }
    double x, y;
  };

  class PathComponent {
  public:
    enum Type { MOVE_TO = 1, LINE_TO, ARC };

  PathComponent(Type _type, double _x0, double _y0) : type(_type), x0(_x0), y0(_y0), radius(0), sa(0), ea(0), anticlockwise(false) { }
  PathComponent(Type _type, double _x0, double _y0, double _radius, double _sa, double _ea, bool _anticlockwise) : type(_type), x0(_x0), y0(_y0), radius(_radius), sa(_sa), ea(_ea), anticlockwise(_anticlockwise) { }
      
    Type type;
    double x0, y0, radius, sa, ea;
    bool anticlockwise;
  };
  
  class Path {
  public:
  Path() : current_point(0, 0) { }
    
    void moveTo(double x, double y) {
      data.push_back(PathComponent(PathComponent::MOVE_TO, x, y));
      current_point = Point(x, y);
    }
    void lineTo(double x, double y) {
      data.push_back(PathComponent(PathComponent::LINE_TO, x, y));
      current_point = Point(x, y);
    }
    void arc(double x, double y, double radius, double sa, double ea, bool anticlockwise) {
      data.push_back(PathComponent(PathComponent::ARC, x, y, radius, sa, ea, anticlockwise));
    }
    void arcTo(double x1, double y1, double x2, double y2, double radius);

    const std::vector<PathComponent> & getData() const { return data; }

    void clear() {
      data.clear();
      current_point = Point(0, 0);
    }

    const Point & getCurrentPoint() const { return current_point; }

    void offset(double dx, double dy) {
      for (auto & pc : data) {
	pc.x0 += dx;
	pc.y0 += dy;
      }
    }
    
  private:
    std::vector<PathComponent> data;
    Point current_point;
  };
};

#endif
