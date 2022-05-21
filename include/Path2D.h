#ifndef _CANVAS_PATH2D_H_
#define _CANVAS_PATH2D_H_

#include <Point.h>
#include <Matrix.h>

#include <vector>

namespace canvas {
  class PathComponent {
  public:
    enum Type { MOVE_TO = 1, LINE_TO, ARC, CLOSE };

  PathComponent(Type _type) : type(_type), x0(0), y0(0), radius(0), sa(0), ea(0), anticlockwise(false) { }
  PathComponent(Type _type, double _x0, double _y0) : type(_type), x0(_x0), y0(_y0), radius(0), sa(0), ea(0), anticlockwise(false) { }
  PathComponent(Type _type, double _x0, double _y0, double _radius, double _sa, double _ea, bool _anticlockwise) : type(_type), x0(_x0), y0(_y0), radius(_radius), sa(_sa), ea(_ea), anticlockwise(_anticlockwise) { }
      
    Type type;
    double x0, y0, radius, sa, ea;
    bool anticlockwise;
  };
  
  class Path2D {
  public:
    Path2D() : current_point(0, 0) { }
    
    void moveTo(float x, float y) {
      data.push_back(PathComponent(PathComponent::MOVE_TO, x, y));
      current_point = Point(x, y);
    }
    void moveTo(Point p) {
      data.push_back(PathComponent(PathComponent::MOVE_TO, p.x, p.y));
      current_point = std::move(p);
    }
    void lineTo(float x, float y) {
      data.push_back(PathComponent(PathComponent::LINE_TO, x, y));
      current_point = Point(x, y);
    }
    void lineTo(Point p) {
      data.push_back(PathComponent(PathComponent::LINE_TO, p.x, p.y));
      current_point = std::move(p);
    }
    void closePath() {
      if (!data.empty()) {
	data.push_back(PathComponent(PathComponent::CLOSE));
	current_point = Point(data.front().x0, data.front().y0);
      }
    }
    void arc(const Point & p, double radius, double sa, double ea, bool anticlockwise);
    void arcTo(const Point & p1, const Point & p2, double radius);

    void rect(double x, double y, double w, double h) {
      moveTo(x, y);
      lineTo(x + w, y);
      lineTo(x + w, y + h);
      lineTo(x, y + h); 
      closePath();
    }

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

    void getExtents(double & min_x, double & min_y, double & max_x, double & max_y) const {
      if (data.empty()) {
	min_x = min_y = max_x = max_y = 0;
      } else {
	auto it = data.begin();
	min_x = max_x = it->x0;
	min_y = max_y = it->y0;
	for (auto & pc : data) {
	  if (pc.x0 < min_x) min_x = pc.x0;
	  if (pc.y0 < min_y) min_y = pc.x0;
	  if (pc.x0 < max_x) max_x = pc.y0;
	  if (pc.y0 < max_y) max_y = pc.y0;
	}
      }
    }

    bool empty() const { return data.empty(); }
    bool isInside(double x, double y) const;
    std::size_t size() const { return data.size(); }

    Path2D transform(const Matrix & matrix) const {
      Path2D new_path;
      for (auto pc : data) {
	auto p = matrix.multiply(pc.x0, pc.y0);
	pc.x0 = p.x;
	pc.y0 = p.y;
#if 0
	pc.sa = matrix.transformAngle(pc.sa);
	pc.ea = matrix.transformAngle(pc.ea);
#endif
	pc.radius = matrix.transformSize(pc.radius);
	new_path.data.push_back(pc);
      }
      new_path.current_point = matrix.multiply(current_point);
      return new_path;
    }

  private:
    std::vector<PathComponent> data;
    Point current_point;
  };
};

#endif
