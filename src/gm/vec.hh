#ifndef GM_VEC_HH__
#define GM_VEC_HH__

#include <cmath>
#include <type_traits>
//#include <iosfwd>
#include <iostream>
#include <cstring>

#include "util.hh"

#include "../util/random.hh"

namespace gm {

/**
 * 不引入glm，对于非图形API没有必要依赖于它
 * （不够轻量级）
 */
template <typename T>
class Vec3
{
 public:
  /*
   * 由于不想通过.x(), .y(), .z()访问分量，
   * 并且由于c++的紧凑内存布局，所以可以O(1)通过偏移量访问分量
   * 因此将分量作为公共成员会很方便
   */
  T x;
  T y;
  T z;
  
  Vec3() = default;
  Vec3(T x_, T y_, T z_) noexcept
    : x(x_)
    , y(y_)
    , z(z_)
  {
  }
  
  T operator[](int idx) const noexcept { return reinterpret_cast<T const*>(this)[idx]; }
  T& operator[](int idx) noexcept { return reinterpret_cast<T*>(this)[idx]; }
  
  Vec3& operator/=(T d) noexcept
  {
    x /= d;
    y /= d;
    z /= d;
    return *this;
  }

  Vec3 operator/(T d) const noexcept
  {
    return {
      x / d,
      y / d,
      z / d,
    };
  }
  
  Vec3 &operator/=(Vec3 const &v) noexcept
  {
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return *this;
  }
  
  Vec3 operator/(Vec3 const &v) const noexcept
  {
    return {
      x / v.x,
      y / v.y,
      z / v.z,
    };
  }

  Vec3& operator*=(T m) noexcept
  {
    x *= m;
    y *= m;
    z *= m;
    return *this;
  }

  Vec3 operator*(T m) const noexcept
  {
    /*
     * 一般其实会转发给*=实现，比如：
     * auto ret = *this;
     * return ret *= m;
     *
     * 但是我这里想避免一次拷贝，因为计算很频繁
     */
    return {
      x * m,
      y * m,
      z * m,
    };
  }
  
  Vec3 &operator*=(Vec3 const &v) noexcept
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }
  
  Vec3 operator*(Vec3 const &v) const noexcept
  {
    return {
      x * v.x,
      y * v.y,
      z * v.z,
    };

  }

  Vec3& operator+=(Vec3 const &v) noexcept
  {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  Vec3& operator+=(T m) noexcept
  {
    x += m;
    y += m;
    z += m;
    return *this;
  }
  
  Vec3 operator+(T m) const noexcept
  {
    return {
      x + m,
      y + m,
      z + m,
    };
  }
  
  Vec3& operator-=(T m) noexcept
  {
    x -= m;
    y -= m;
    z -= m;
    return *this;
  }

  Vec3 operator-(T m) const noexcept
  {
    return {
      x - m,
      y - m,
      z - m,
    };
  }

  Vec3 operator+(Vec3 const &v) const noexcept
  {
    return {
      x + v.x,
      y + v.y,
      z + v.z,
    };
  }
  
  Vec3& operator-=(Vec3 const &v) noexcept
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  Vec3 operator-(Vec3 v) const noexcept
  {
    return {
      x - v.x,
      y - v.y,
      z - v.z,
    };
  }
  
  Vec3 operator-() const noexcept
  {
    return {
      -x, -y, -z,
    };
  }

  bool operator==(Vec3 const &rhs) const noexcept
  {
    return memcmp(this, &rhs, sizeof(Vec3)) == 0;
  }

  bool operator!=(Vec3 const &rhs) const noexcept
  {
    return !(*this == rhs);
  }

  double length_squared() const noexcept
  {
    return x * x + y * y + z * z;
  }

  double length() const noexcept
  {
    return std::sqrt(length_squared());
  }
  
  Vec3 normalize() const noexcept
  {
    return *this / length();
  }
  
  bool is_near_zero() const noexcept
  {
    return fabs(x) < gm::epsilon &&
           fabs(y) < gm::epsilon &&
           fabs(z) < gm::epsilon;
  }

  static Vec3 random() noexcept
  {
    return {
      util::random_double(),
      util::random_double(),
      util::random_double(),
    };
  }

  static Vec3 random(double rmin, double rmax) noexcept
  {
    return {
      util::random_double(rmin, rmax),
      util::random_double(rmin, rmax),
      util::random_double(rmin, rmax),
    };
  }
};

template <typename T>
inline Vec3<T> operator*(T m, Vec3<T> const &v) noexcept
{
  return v * m;
}

template <typename T>
inline Vec3<T> operator/(T m, Vec3<T> const &v) noexcept
{
  return {
    m / v.x,
    m / v.y,
    m / v.z,
  };
}

template <typename T1, typename T2>
inline double dot(Vec3<T1> const &v1, Vec3<T2> const &v2) noexcept
{
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template <typename T1, typename T2>
inline Vec3<double> cross(Vec3<T1> const &v1, Vec3<T2> const &v2) noexcept
{
  /*
   * | i     j     k  |
   * | v1.x v1.y v1.z |
   * | v2.x v2.y v2.z |
   */
  return {
    v1.y * v2.z - v1.z * v2.y,
    v1.z * v2.x - v1.x * v2.z,
    v1.x * v2.y - v1.y * v2.x,
  };
}

using Vec3F = Vec3<double>;
using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;

inline Vec3F lerp(Vec3F start, Vec3F end, double t) noexcept
{
  return {
    lerp(start.x, end.x, t),
    lerp(start.y, end.y, t),
    lerp(start.z, end.z, t),
  };
}

template <typename T>
std::ostream& operator<<(std::ostream &os, Vec3<T> const &v) noexcept
{
  return os << '(' << v.x << ", " << v.y << ", "<< v.z << ')';
}

Vec3F random_in_unit_sphere();
Vec3F random_on_unit_sphere_surface();

} // namespace gm

using gm::Vec3F;

#endif
