#include "random.hh"

#include <random>

namespace util {

double random_double()
{
  static std::random_device rd;
  static std::mt19937 generator(rd());
  static std::uniform_real_distribution<> dist(0., 1.);
  return dist(generator);
}

double random_double(double rmin, double rmax)
{
  return rmin + (rmax - rmin) * random_double(); 
}

}
