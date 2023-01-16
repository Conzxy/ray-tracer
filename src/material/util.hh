#ifndef MATERIAL_UTIL_HH__
#define MATERIAL_UTIL_HH__

#include "../gm/vec.hh"

namespace rt {

gm::Vec3F reflect(gm::Vec3F const &v, gm::Vec3F const &n);
gm::Vec3F refract(gm::Vec3F const &r, gm::Vec3F const &n, double eta_over_eta2);

} // namespace rt

#endif
