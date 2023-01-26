#ifndef RT_GM_MATRIX_HH__
#define RT_GM_MATRIX_HH__

#include <array>
#include <initializer_list>
#include <stddef.h>

#include "vec.hh"
#include "point.hh"

namespace gm {

template <typename T, size_t R, size_t C>
class Matrix {
  static_assert(std::is_arithmetic<T>::value,
                "The element type of matrix must be floating or integer!");

  using Row = std::array<T, C>;
  using Data = std::array<Row, R>;

 public:
  Matrix() = default;
  Matrix(Data const &data)
    : data_(data)
  {
  }

  Matrix(std::initializer_list<std::initializer_list<T>> data)
  {
    SetData(data);
  }

  template <typename = typename std::enable_if<!std::is_same<
                typename std::decay<T>::type, Matrix>::value>::type>
  explicit Matrix(T init) noexcept
  {
    for (auto &row : data_)
      row.fill(init);
  }

  Matrix(Matrix const &) = default;
  Matrix(Matrix &&) = default;
  Matrix &operator=(Matrix const &) = default;
  Matrix &operator=(Matrix &&) = default;

  ~Matrix() = default;

  Row &operator[](size_t r) noexcept { return data_[r]; }
  Row const &operator[](size_t r) const noexcept { return data_[r]; }

  size_t row_num() const noexcept { return R; }
  size_t col_num() const noexcept { return C; }

  void SetData(std::initializer_list<std::initializer_list<T>> data)
  {
    for (size_t i = 0; i < R; ++i) {
      for (size_t j = 0; j < C; ++j)
        data_[i][j] = *((data.begin() + i)->begin() + j);
    }
  }

  Matrix &operator=(std::initializer_list<std::initializer_list<T>> data)
  {
    SetData(data);
    return *this;
  }

 private:
  Data data_;
};

namespace detail {

template <typename T, size_t M, size_t K, size_t N, size_t i, size_t j,
          size_t k>
struct matrix_multiple_nest_3 {
  static void f(Matrix<T, M, N> &c, Matrix<T, M, K> const &a,
                Matrix<T, K, N> const &b)
  {
    c[i][k] += a[i][j] * b[j][k];
    matrix_multiple_nest_3<T, M, K, N, i, j, k + 1>::f(c, a, b);
  }
};

template <typename T, size_t M, size_t K, size_t N, size_t i, size_t j>
struct matrix_multiple_nest_3<T, M, K, N, i, j, N - 1> {
  static void f(Matrix<T, M, N> &c, Matrix<T, M, K> const &a,
                Matrix<T, K, N> const &b)
  {
    c[i][N - 1] += a[i][j] * b[j][N - 1];
  }
};

template <typename T, size_t M, size_t K, size_t N, size_t i, size_t j>
struct matrix_multiple_nest_2 {
  static void f(Matrix<T, M, N> &c, Matrix<T, M, K> const &a,
                Matrix<T, K, N> const &b)
  {
    matrix_multiple_nest_3<T, M, K, N, i, j, 0>::f(c, a, b);
    matrix_multiple_nest_2<T, M, K, N, i, j + 1>::f(c, a, b);
  }
};

template <typename T, size_t M, size_t K, size_t N, size_t i>
struct matrix_multiple_nest_2<T, M, K, N, i, K - 1> {
  static void f(Matrix<T, M, N> &c, Matrix<T, M, K> const &a,
                Matrix<T, K, N> const &b)
  {
    matrix_multiple_nest_3<T, M, K, N, i, K - 1, 0>::f(c, a, b);
  }
};

template <typename T, size_t M, size_t K, size_t N, size_t i>
struct matrix_multiple_nest_1 {
  static void f(Matrix<T, M, N> &c, Matrix<T, M, K> const &a,
                Matrix<T, K, N> const &b)
  {
    matrix_multiple_nest_2<T, M, K, N, i, 0>::f(c, a, b);
    matrix_multiple_nest_1<T, M, K, N, i + 1>::f(c, a, b);
  }
};

template <typename T, size_t M, size_t K, size_t N>
struct matrix_multiple_nest_1<T, M, K, N, M - 1> {
  static void f(Matrix<T, M, N> &c, Matrix<T, M, K> const &a,
                Matrix<T, K, N> const &b)
  {
    matrix_multiple_nest_2<T, M, K, N, M - 1, 0>::f(c, a, b);
  }
};

} // namespace detail

template <typename T, size_t M, size_t K, size_t N>
Matrix<T, M, N> operator*(Matrix<T, M, K> const &a, Matrix<T, K, N> const &b)
{
  Matrix<T, M, N> c(0);
  detail::matrix_multiple_nest_1<T, M, K, N, 0>::f(c, a, b);
  return c;
}

inline Vec3F operator*(Matrix<double, 3, 3> const &m, Vec3F const &v)
{
  return {
      v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2],
      v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2],
      v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2],
  };
};

inline Point3F operator*(Matrix<double, 3, 3> const &m, Point3F const &v)
{
  return {
      v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2],
      v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2],
      v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2],
  };
};

/* Discard */
template <typename T, size_t M, size_t K, size_t N>
Matrix<T, M, N> MatrixMultiple2(Matrix<T, M, K> const &a,
                                Matrix<T, K, N> const &b) noexcept
{
  Matrix<T, M, N> c(0);
  for (size_t m = 0; m < M; ++m) {
    for (size_t k = 0; k < K; ++k) {
      for (size_t n = 0; n < N; ++n) {
        c[m][n] += a[m][k] * b[k][n];
      }
    }
  }
  return c;
}

using Matrix3x3F = Matrix<double, 3, 3>;

} // namespace gm

#endif