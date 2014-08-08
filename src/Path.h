#ifndef _PATH_H_
#define _PATH_H_

namespace canvas {
  struct path_component {
    enum Type { MOVE_TO, LINE_TO } type;
    double x0, y0;
  };
  
  class Path {
  public:
    Path() { }
    
    void moveTo(double x, double y) {
      data.push_back(path_component(MOVE_TO, x0, y0));
    }
    void lineTo(double x, double y) {
      data.push_back(path_component(LINE_TO, x0, y0));
    }

  private:
    std::vector<path_component> data;
  };
};

#endif
