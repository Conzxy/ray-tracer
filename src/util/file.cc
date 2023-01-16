#include "file.hh"

#if defined(__linux__) || defined(__unix__)
#include <sys/stat.h>
#endif

#include <assert.h>
#include <stdexcept>
#include <string.h>

using namespace util;

File::File(char const* filename, int mode)
  : fp_(NULL)
{
  if (false == Open(filename, mode)) {
    std::string buf;
    buf.reserve(strlen(filename) + 32);
    ::snprintf(&*buf.begin(), buf.capacity(),
     "Failed to open file: %s",
      filename);

    throw FileException(buf);
  }
}

File::File(std::string const& filename, int mode)
  : fp_(NULL)
{  
  if (false == Open(filename, mode)) {
    std::string buf;
    buf.reserve(filename.size() + 32);
    buf = "Failed to open file: ";
    buf += filename;

    throw FileException(buf);
  }
}

File::~File() noexcept
{
  if (fp_) {
    ::fclose(fp_);
  }
}

bool File::Open(char const* filename, int mode)
{
  if (mode & WRITE) {
    fp_ = ::fopen(filename, "r+");
  }
  else if (mode & READ) {
    if (mode & APP) {
      fp_ = ::fopen(filename, "a+");
    }
    else if (mode & TRUNC) {
      fp_ = ::fopen(filename, "w+");
    }
    else {
      fp_ = ::fopen(filename, "r");
    }
  }
  else if (mode & APP) {
    fp_ = ::fopen(filename, "a");
  }
  else if (mode & TRUNC) {
    fp_ = ::fopen(filename, "w");
  }

  return fp_ != NULL;
}

size_t File::Read(void* buf, size_t len)
{
  // According the description of fread,
  // this is maybe not necessary.
  // But regardless of whether it does or not something like readn,
  // We should make sure read complete even if there is no short read occurred.

  auto p = reinterpret_cast<char*>(buf);
  size_t remaining = len;
  size_t readn = 0;

  while (remaining > 0) {
    readn = ::fread(p, 1, remaining, fp_);

    if (readn < len) {
      // Error occcurred or eof
      if (::feof(fp_) != 0) {
        remaining -= readn;
        break;
      }
      else if (::ferror(fp_) != 0){
        if (errno == EINTR) {
          continue;
        }
      }

      return -1;
    }
    else {
      p += readn;
      remaining -= readn;
    }
  }

  return len - remaining;
}

int File::ReadLine(std::string& line, const bool need_newline)
{
  char buf[4096];
  char* ret = NULL;
  size_t n = 0;

  line.clear();

  do {
    ret = ::fgets(buf, sizeof buf, fp_);

    if (ret == NULL) {
      if (::ferror(fp_)) {
        if (errno == EINTR) {
          continue;
        }
      }
      else if (::feof(fp_)) {
        return E_EOF;
      }

      return E_ERROR;
    }

    assert(ret == buf);
    n = strlen(buf);

    if (n >= 1 && buf[n-1] == '\n') {
      if (!need_newline) {
        if (n >= 2 && buf[n-2] == '\r') {
          line.append(buf, n - 2);
        }
        else {
          line.append(buf, n - 1);
        }
      } else {
        line.append(buf, n);
      }
      break;
    }

    line.append(buf, n);

  } while (n == (sizeof(buf) - 1));

  return 0;
}

size_t File::GetFileSize(char const *path) noexcept
{
#if defined(__linux__) || defined(__unix__)
  struct stat stat_buffer;
  auto ret = stat(path, &stat_buffer);
  if (ret < 0) {
    return -1;
  }
  return stat_buffer.st_size;
#else
  File file;
  if (!file.Open(path)) {
    return -1;
  }

  file.SeekEnd(0);
  return file.GetCurrentPosition();
#endif
}

size_t File::GetFileSize() const noexcept
{
  // TODO error handling
  auto self = const_cast<File*>(this);
  auto old_pos = self->GetCurrentPosition();
  self->SeekEnd(0);
  auto fsize = self->GetCurrentPosition();
  self->SeekBegin(old_pos);
  return fsize;
}
