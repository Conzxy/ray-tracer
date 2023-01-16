#ifndef IMG_COLOR_HH__
#define IMG_COLOR_HH__

#include <cassert>
#include <cstring>

#include "../util/arithmetic_type.h"
#include "../gm/util.hh"
#include "../gm/vec.hh"

namespace img {

/**
 * Color 与 Point3，Vec3 有很多操作不同
 * 同时考虑alpha channel，不应该复用它们
 */
class Color {
 public:
  u8 b;
  u8 g;
  u8 r;
  u8 a;  

  Color() noexcept
    : b(255)
    , g(255)
    , r(255)
    , a(255)
  {
  }

  Color(int r_, int g_, int b_, int a_=255) noexcept
    : b(b_)
    , g(g_)
    , r(r_)
    , a(a_)
  {
  }

  Color(double r_, double g_, double b_, int a_=255) noexcept
    : b(b_ * 255.999)
    , g(g_ * 255.999)
    , r(r_ * 255.999)
    , a(a_)
  {
  }
  
  explicit Color(Vec3F rgb, int a_=255) noexcept
    : Color(rgb.x, rgb.y, rgb.z, a_)
  {
  }

  Color(u8* data, int bpp) noexcept
  {
    assert(bpp <= 4 && "Bytes per pixel must be not greater than 4");
    memcpy(this, data, bpp); 
  }
  
  // Color &operator=(Color const &) = default;

  u8 operator[](int i) const noexcept
  {
    assert(i < 4 && "Color index must be less than 4");
    return reinterpret_cast<u8 const*>(this)[i];
  }
  
  u8& operator[](int i) noexcept
  {
    assert(i < 4 && "Color index must be less than 4");
    return reinterpret_cast<u8*>(this)[i];
  }

  u8 const* data() const noexcept { return reinterpret_cast<u8 const*>(this); }
  
  // Color / number = Color
  Color& operator/=(double d) noexcept
  {
    r /= d;
    g /= d;
    b /= d;
    a /= d;
    return *this;
  }

  Color operator/(double d) const noexcept
  {
    auto ret = *this;
    return ret /= d;
  }
  
  // Color * number = Color
  Color& operator*=(double d) noexcept
  {
    r *= d;
    g *= d;
    b *= d;
    a *= d;
    return *this;
  }

  Color operator*(double d) const noexcept
  {
    auto ret = *this;
    return ret *= d;
  }

  bool operator==(Color const &rhs) const noexcept
  {
    return memcmp(this, &rhs, sizeof(Color)) == 0;
  }

  bool operator!=(Color const &rhs) const noexcept
  {
    return !(*this == rhs);
  }

  static Color black;
  static Color white;
};

inline std::ostream &operator<<(std::ostream &os, Color const &c)
{
  return os << "(r: " << (int)c.r << ", g: " << (int)c.g << ", b: " << (int)c.b << ")";
}

inline Color operator*(double d, Color const &c) noexcept
{
  return c * d;
}

inline Color operator/(double d, Color const &c) noexcept
{
  return Color {
    int(d / c.r),
    int(d / c.g),
    int(d / c.b),
    int(d / c.a),
  };
}

inline Color lerp(Color start, Color end, double t) noexcept
{
  return {
    (int)gm::lerp(start.r, end.r, t),
    (int)gm::lerp(start.g, end.g, t),
    (int)gm::lerp(start.b, end.b, t),
  };
}

} // namespace img

#endif
