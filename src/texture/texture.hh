#ifndef TEXTURE_HH__
#define TEXTURE_HH__

#include <memory>

#include "../gm/point.hh"
#include "../rt/color.hh"

namespace rt {

class Texture {
 public:
  virtual rt::Color value(double u, double v, Point3F const &p) const = 0;
};

using TextureSPtr = std::shared_ptr<Texture>;

}

#endif
