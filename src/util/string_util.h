#ifndef UTIL_STRING_UTIL_H_
#define UTIL_STRING_UTIL_H_

#include <string>

#ifdef INCLUDE_KANON_STRING_VIEW
#include "kanon/string/string_view.h"
#endif

#include <cstring>
#include <cassert>

#if __cplusplus >= 201703L
#include <string_view>
#endif

namespace util {

namespace detail {

/*
 * The following functions are deprecated
 */
inline size_t Strlen(std::string const &str) noexcept
{
  return str.size();
}

inline size_t Strlen(char const *str) noexcept
{
  return strlen(str);
}

#ifdef INCLUDE_KANON_STRING_VIEW
inline size_t Strlen(kanon::StringView const &str) noexcept
{
  return str.size();
}
#endif

template <typename S, typename... Args>
size_t StrSize_impl(S const &head, Args &&... strs) noexcept
{
  return Strlen(head) + StrSize_impl(strs...);
}

template <typename S>
size_t StrSize_impl(S const &head) noexcept
{
  return Strlen(head);
}

template <typename... Args>
size_t StrSize(Args &&... args)
{
  return StrSize_impl(args...);
}

/*
 * 利用模板递归实例化在编译时展开模板，从而避免了运行时循环开销
 * 由于递归的特性，可以在最后的出口函数进行预分配，
 * 并从尾部开始拷贝最后一个字符串
 *
 * 为兼容C-style，std::string，kanon::StringView(kanon)，
 * std::string_view(c++17)，重载了四个版本
 */

#define STRAPPEND_STL_STYLE                                                    \
  do {                                                                         \
    size += head.size();                                                       \
    StrAppend_impl(str, size, index, strs...);                                 \
    assert(index >= head.size());                                          \
    index -= head.size();                                                      \
    memcpy(&str[index], head.data(), head.size());                             \
  } while (0)

/*
 * 注意，这里必须前向声明，不然递归实例化时，实例化点可能看不到在下面定义的其他StrAppend_impl()，因此重载决议并不会考虑它们
 * 因此诸如
 * `auto s = StrCat("A", std::string("A"), std::string_view("A"),
 * kanon::StringView("A"))`就会报错
 */
template <typename... Args>
void StrAppend_impl(std::string &str, size_t &size, size_t &index,
                    std::string const &head, Args &&... strs);
#ifdef INCLUDE_KANON_STRING_VIEW
template <typename... Args>
void StrAppend_impl(std::string &str, size_t &size, size_t &index,
                    kanon::StringView const &head, Args &&... strs);
#endif

template <typename... Args>
void StrAppend_impl(std::string &str, size_t &size, size_t &index,
                    char const *head, Args &&... strs);

template <typename... Args>
void StrAppend_impl(std::string &str, size_t &size, size_t &index,
                    std::string_view const &head, Args &&... strs);

#ifdef INCLUDE_KANON_STRING_VIEW
template <typename... Args>
void StrAppend_impl(std::string &str, size_t &size, size_t &index,
                    std::string const &head, Args &&... strs)
{
  STRAPPEND_STL_STYLE;
}

template <typename... Args>
void StrAppend_impl(std::string &str, size_t &size, size_t &index,
                    kanon::StringView const &head, Args &&... strs)
{
  STRAPPEND_STL_STYLE;
}
#endif

template <typename... Args>
void StrAppend_impl(std::string &str, size_t &size, size_t &index,
                    char const *head, Args &&... strs)
{
  auto len = strlen(head);
  size += len;
  StrAppend_impl(str, size, index, strs...);
  assert(index >= len);
  index -= len;
  memcpy(&str[index], head, len);
}

#ifdef CXX_STANDARD_17
template <typename... Args>
void StrAppend_impl(std::string &str, size_t &size, size_t &index,
                    std::string_view const &head, Args &&... strs)
{
  STRAPPEND_STL_STYLE;
}
#endif

#define STRAPPEND_STL_STYLE_EXIT                                               \
  do {                                                                         \
    size += head.size();                                                       \
    str.resize(size);                                                          \
    index = size - head.size();                                                \
    memcpy(&str[index], head.data(), head.size());                             \
  } while (0)

#ifdef INCLUDE_KANON_STRING_VIEW
template <>
inline void StrAppend_impl<>(std::string &str, size_t &size, size_t &index,
                             kanon::StringView const &head)
{
  STRAPPEND_STL_STYLE_EXIT;
}
#endif

template <>
inline void StrAppend_impl<>(std::string &str, size_t &size, size_t &index,
                             std::string const &head)
{
  STRAPPEND_STL_STYLE_EXIT;
}

template <>
inline void StrAppend_impl<>(std::string &str, size_t &size, size_t &index,
                             char const *head)
{
  auto len = strlen(head);
  size += len;
  str.resize(size);
  index = size - len;
  memcpy(&str[index], head, len);
}

#ifdef CXX_STANDARD_17
template <>
inline void StrAppend_impl<>(std::string &str, size_t &size, size_t &index,
                             std::string_view const &head)
{
  STRAPPEND_STL_STYLE_EXIT;
}
#endif

} // namespace detail

/**
 * \brief Append strings efficiently than str.append() with loop or sequence
 *
 * The function will preallocate the space and append them to the \p str,
 * and expand them in compile time, so there is no runtime overhead.
 * \note
 *  The performance is better than the absl::StrAppend()
 */
template <typename... Args>
void StrAppend(std::string &str, Args &&... strs)
{
  size_t size = 0;
  size_t index = 0;
  detail::StrAppend_impl(str, size, index, strs...);
}

/**
 * \brief Concatenate the strings to a single string efficiently
 *
 * Trivially, a rookie may concatenate the string in the following:
 * ```cpp
 * std::string str = s1 + s2 + s3;
 * // Worse
 * str = str + s1 + s2 + s3;
 * ```
 * This approach will create many temporary object, therefore, to avoid it,
 * maybe:
 * ```cpp
 * std::string str;
 * str += s1;
 * str += s2;
 * str += s3;
 * ```
 * This is OK, but the memory need reallocate multiple times.
 * The better approach is preallocate throught the reserve() method:
 * ```cpp
 * std::string str;
 * str.reserve(s1.size() + s2.size() + s3.size())
 * str += ...
 * ```
 * But the progress is cumbersome, and this function will do it.
 * \note
 *  In fact, this a wrapper of the StrAppend(empty-string, strs...)
 */
template <typename... Args>
std::string StrCat(Args &&... strs)
{
  std::string ret;
  StrAppend(ret, strs...);
  return ret;
}
} // namespace util

#endif // UTIL_STRING_UTIL_H_
