#include "gm/onb.hh"

using namespace gm;

int main()
{
  {
  Onb onb(Vec3F(0.999, 0, 0));
  std::cout << "u = " << onb.u << '\n'
            << "v = " << onb.v << '\n'
            << "w = " << onb.w << '\n';
  }
  {
  Onb onb(Vec3F(0, 0, 1));
  std::cout << "u = " << onb.u << '\n'
            << "v = " << onb.v << '\n'
            << "w = " << onb.w << '\n';
  }
  {
  Onb onb(Vec3F(0, 1, 0));
  std::cout << "u = " << onb.u << '\n'
            << "v = " << onb.v << '\n'
            << "w = " << onb.w << '\n';
    
  }
}
