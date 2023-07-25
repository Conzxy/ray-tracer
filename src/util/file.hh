#ifndef KURO_UTIL_FILE_H__
#define KURO_UTIL_FILE_H__

#include <stddef.h>
#include <stdexcept>
#include <stdint.h>
#include <stdio.h>
#include <string>

#include "../util/noncopyable.hh"

namespace util {

struct FileException : std::runtime_error {
  explicit FileException(char const *msg)
    : std::runtime_error(msg)
  {
  }

  explicit FileException(std::string const &msg)
    : std::runtime_error(msg)
  {
  }
};

/**
 * Wrap Standard C library File API
 * into RAII style
 */
class File : kanon::noncopyable {
 public:
  enum Errno {
    E_EOF = 1,
    E_ERROR,
  };

  enum OpenMode : uint8_t {
    READ = 0x1,
    WRITE = 0x2,
    TRUNC = 0x4,
    APP = 0x8,
  };

  File() = default;

  /**
   * \brief Open file in specific mode
   * \param filename The path of file
   * \param mode File open mode(SHOULD be the bit operation of OpenMode)
   *
   * \exception FileException
   */
  File(char const *filename, int mode);
  File(std::string const &filename, int mode);

  /**
   * Like the constructor but don't throw exception
   * instead return false to indicates failure
   *
   * \return
   *  true -- Success
   */
  bool Open(std::string const &filename, int mode)
  {
    return Open(filename.data(), mode);
  }

  bool Open(char const *filename, int mode);

  ~File() noexcept;

  /**
   * \brief Read file content in len size
   * \param buf The buffer store contents
   * \param len The length you want read
   *
   * \return
   *  -1 -- failure
   *  < len -- eof
   */
  size_t Read(void *buf, size_t len);

  /**
   * \brief Read a line terminated \r\n(Win) or \n(Linux/Unix)
   * \param need_newline Save the newline characters
   *
   * \return
   *  E_EOF -- eof
   *  E_ERROR -- error
   *  0 -- OK
   */
  int ReadLine(std::string &line, const bool need_newline = true);

  bool Write(void const *buf, size_t len) noexcept
  {
    return ::fwrite(buf, 1, len, fp_) < len;
  }

  bool Flush() noexcept { return ::fflush(fp_) != 0; }
  void Rewind() noexcept { ::rewind(fp_); }
  bool SeekCurrent(long offset) noexcept { return Seek(offset, SEEK_CUR) == 0; }
  bool SeekBegin(long offset) noexcept { return Seek(offset, SEEK_SET) == 0; }
  bool SeekEnd(long offset) noexcept { return Seek(offset, SEEK_END) == 0; }
  long GetCurrentPosition() noexcept { return ::ftell(fp_); }
  size_t GetFileSize() const noexcept;

  FILE *fp() const noexcept { return fp_; }
  FILE *GetFileHandler() const noexcept { return fp_; }
  bool IsValid() const noexcept { return fp_ != NULL; }

  static size_t GetFileSize(char const *path) noexcept;

  static const size_t kInvalidReturn = static_cast<size_t>(-1); /* Deprecated */
  static const size_t INVALID_RETURN = static_cast<size_t>(-1);

 private:
  int Seek(long offset, int whence) noexcept
  {
    return ::fseek(fp_, offset, whence);
  }

  FILE *fp_ = nullptr;
};

} // namespace util

#endif
