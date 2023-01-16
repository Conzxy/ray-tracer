#include "str_cvt.hh"

#include <cstdlib>
#include <cstring>

namespace util {

std::optional<int> str2int(char const *str, int base)
{
  char *endptr = NULL;
  auto ret = strtol(str, &endptr, base);
  if (endptr && *endptr != '\0') {
    return {};
  }

  return ret;
}

std::optional<int> str2int(char const *str, size_t n, int base)
{
  char buf[64];
  strncpy(buf, str, n);
  return str2int(buf, base);
}

}
