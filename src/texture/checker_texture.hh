#ifndef TEXTURE_CHECKER_TEXTURE_HH__
#define TEXTURE_CHECKER_TEXTURE_HH__

#include "texture.hh"

namespace rt {

class CheckerTexture : public Texture {
 public:
   // CheckerTexture() = default;
   CheckerTexture(Color const &c1, Color const &c2);
   CheckerTexture(TextureSPtr even, TextureSPtr odd);

   Color value(double u, double v, Point3F const &p) const override;
 private:
   TextureSPtr even_;
   TextureSPtr odd_;
};

}

#endif
