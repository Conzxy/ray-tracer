#include "matrix.hh"

#include <iostream>

namespace gm {

template <typename T, size_t M, size_t N>
std::ostream &operator<<(std::ostream &os, Matrix<T, M, N> const &m)
{
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      os << m[i][j] << " ";
    }
    os << '\n';
  }
  return os;
}

}