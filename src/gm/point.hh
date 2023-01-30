#ifndef GM_POINT_HH__
#define GM_POINT_HH__

#include <iostream>

#include "vec.hh"

namespace gm {

/**
 * 并不复用Vec3表示Point3
 * 因为Point有些操作是没有的，避免不经意的错误
 */
template <typename T>
class Point3
{
 public:
  T x;
  T y;
  T z;
  
  Point3() = default;

  Point3(T x_, T y_, T z_) noexcept
    : x(x_)
    , y(y_)
    , z(z_)
  {
  }
  
  Point3(Vec3<T> const &v) noexcept
    : x(v.x)
    , y(v.y)
    , z(v.z)
  {
  }

  T operator[](int idx) const noexcept { return reinterpret_cast<T const*>(this)[idx]; }
  T& operator[](int idx) noexcept { return reinterpret_cast<T*>(this)[idx]; }

  // Point3 / number
  Point3& operator/=(T d) noexcept
  {
    x /= d;
    y /= d;
    z /= d;
    return *this;
  }

  Point3 operator/(T d) const noexcept
  {
    return {
      x / d,
      y / d,
      z / d,
    };
  }

  // Point3 * number
  Point3& operator*=(T m) noexcept
  {
    x *= m;
    y *= m;
    z *= m;
    return *this;
  }

  Point3 operator*(T m) const noexcept
  {
    return {
      x * m,
      y * m,
      z * m,
    };
  }

  // Point3 + Point3 is meaningless
  
  // Point3 + Vector = Point3
  Point3& operator+=(Vec3<T> const &v) noexcept
  {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }
  
  Point3 operator+(Vec3<T> const &v) const noexcept
  {
    return {
      x + v.x,
      y + v.y,
      z + v.z,
    };
  }
  
  Point3 operator+(T m) const noexcept
  {
    return {
      x + m,
      y + m,
      z + m,
    };
  }
  
  Point3 &operator+=(T m) noexcept
  {
    x += m;
    y += m;
    z += m;
    return *this;
  }

  // Point3 - Vector = Point3
  Point3& operator-=(Vec3<T> const &v) noexcept
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  Point3 operator-(Vec3<T> const &v) const noexcept
  {
    return {
      x - v.x,
      y - v.y,
      z - v.z,
    };
  }

  Vec3<T> operator-(Point3 const &p) const noexcept
  {
    return {
      x - p.x,
      y - p.y,
      z - p.z
    };
  }

  Point3 operator-(T m) const noexcept
  {
    return {
      x - m,
      y - m,
      z - m,
    };
  }
  
  Point3 &operator-=(T m) noexcept
  {
    x -= m;
    y -= m;
    z -= m;
    return *this;
  }

  Point3 operator-() const noexcept
  {
    return {
      -x, -y, -z,
    };
  }
};

// number / Point = Point
// number * Point = Point
template <typename T>
inline Point3<T> operator*(T d, Point3<T> const &p) noexcept
{
  return p * d;
}

template <typename T>
inline Point3<T> operator/(T d, Point3<T> const &p) noexcept
{
  return {
    d / p.x,
    d / p.y,
    d / p.z,
  };
}

template <typename T>
std::ostream& operator<<(std::ostream &os, Point3<T> const &v) noexcept
{
  return os << '(' << v.x << ", " << v.y << ", "<< v.z << ')';
}

using Point3f = Point3<float>;
using Point3F = Point3<double>;
using Point3i = Point3<int>;

} // namespace gm

namespace rt {

using gm::Point3F;

}

#endif
