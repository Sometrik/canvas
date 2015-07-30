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
    enum Type { MOVE_TO = 1, LINE_TO, ARC, CLOSE };

  PathComponent(Type _type) : type(_type), x0(0), y0(0), radius(0), sa(0), ea(0), anticlockwise(false) { }
  PathComponent(Type _type, double _x0, double _y0) : type(_type), x0(_x0), y0(_y0), radius(0), sa(0), ea(0), anticlockwise(false) { }
  PathComponent(Type _type, double _x0, double _y0, double _radius, double _sa, double _ea, bool _anticlockwise) : type(_type), x0(_x0), y0(_y0), radius(_radius), sa(_sa), ea(_ea), anticlockwise(_anticlockwise) { }
      
    Type type;
    double x0, y0, radius, sa, ea;
    bool anticlockwise;
  };
  
  class Path {
  public:
    Path(float _display_scale) : display_scale(_display_scale), current_point(0, 0) { }
    
    void moveTo(double x, double y) {
      data.push_back(PathComponent(PathComponent::MOVE_TO, x, y));
      current_point = Point(x, y);
    }
    void lineTo(double x, double y) {
      data.push_back(PathComponent(PathComponent::LINE_TO, x, y));
      current_point = Point(x, y);
    }
    void close() {
      if (!data.empty()) {
	data.push_back(PathComponent(PathComponent::CLOSE));
	current_point = Point(data.front().x0, data.front().y0);
      }
    }
    void arc(double x, double y, double radius, double sa, double ea, bool anticlockwise);
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

    float getDisplayScale() const { return display_scale; }

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
    
  private:
    float display_scale;
    std::vector<PathComponent> data;
    Point current_point;
  };
};

#endif
