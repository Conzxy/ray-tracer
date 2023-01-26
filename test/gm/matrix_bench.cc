#include "gm/matrix.hh"

#include "gm/print.hh"

#include <benchmark/benchmark.h>

using namespace benchmark;
using namespace gm;

Matrix<double, 4, 4> a({
    {2, 3, 3, 3},
    {3, 3, 3, 1},
    {8, 2, 3, 3},
    {1, 2, 3, 5},
});

Matrix<double, 4, 3> b{
    {2, 3, 4},
    {2, 3, 2},
    {1, 2, 3},
    {8, 2, 3},
};

#define N 10000
static void matrix_multiple_meta(State &state)
{
  for (auto _ : state) {
    for (int i = 0; i < N; ++i) {
      auto c = a * b;
      (void)c;
    }
  }
}

static void matrix_multiple_raw(State &state)
{
  for (auto _ : state) {
    for (int i = 0; i < N; ++i) {
      auto c = MatrixMultiple2(a, b);
      (void)c;
    }
  }
}

BENCHMARK(matrix_multiple_meta);
BENCHMARK(matrix_multiple_raw);