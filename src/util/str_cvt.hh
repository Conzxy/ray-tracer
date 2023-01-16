#ifndef UTIL_STR_CVT_HH__
#define UTIL_STR_CVT_HH__

#include <optional>

#include "../util/arithmetic_type.h"

namespace util {

// double str2double(char const *str, size_t n);
std::optional<int> str2int(char const *str, usize n, int base=10);
std::optional<int> str2int(char const *str, int base=10);

} // namespace util

#endif
