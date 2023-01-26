#include "ray.hh"

#include <iostream>

namespace rt {

std::ostream &operator<<(std::ostream &os, Ray const &ray)
{

  return os << "===== Ray info(start)=====\n"
            << "origin: " << ray.origin() << '\n'
            << "direction: " << ray.direction() << '\n'
            << "===== Ray info(end)=====";
}

} // namespace rt