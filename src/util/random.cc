#include "random.hh"

#include <random>

namespace util {

static std::random_device rd;
static std::mt19937 generator(rd());

double random_double()
{
  static std::uniform_real_distribution<> dist(0., 1.);
  return dist(generator);
}

double random_double(double rmin, double rmax)
{
  return rmin + (rmax - rmin) * random_double();
}

int random_int(int rmin, int rmax)
{
  // static std::uniform_int_distribution<int> dist(rmin, rmax);
  // return dist(generator);
  return int(random_double(rmin, rmax+1));
}

} // namespace util
