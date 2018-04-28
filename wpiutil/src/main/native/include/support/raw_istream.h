/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_SUPPORT_RAW_ISTREAM_H_
#define WPIUTIL_SUPPORT_RAW_ISTREAM_H_

#include <algorithm>
#include <cstddef>
#include <system_error>

#include "llvm/SmallVector.h"
#include "llvm/StringRef.h"
#include "llvm/Twine.h"

namespace wpi {

class raw_istream {
 public:
  raw_istream() = default;
  virtual ~raw_istream() = default;

  raw_istream& read(char& c) {
    read_impl(&c, 1);
    return *this;
  }

  raw_istream& read(unsigned char& c) {
    read_impl(&c, 1);
    return *this;
  }

  raw_istream& read(signed char& c) {
    read_impl(&c, 1);
    return *this;
  }

  raw_istream& read(void* data, size_t len) {
    read_impl(data, len);
    return *this;
  }

  size_t readsome(void* data, size_t len) {
    size_t readlen = std::min(in_avail(), len);
    if (readlen == 0) return 0;
    read_impl(data, readlen);
    return readlen;
  }

  // Read a line from an input stream (up to a maximum length).
  // The returned buffer will contain the trailing \n (unless the maximum length
  // was reached).  \r's are stripped from the buffer.
  // @param buf Buffer for output
  // @param maxLen Maximum length
  // @return Line
  llvm::StringRef getline(llvm::SmallVectorImpl<char>& buf, int maxLen);

  virtual void close() = 0;
  virtual size_t in_avail() const = 0;

  bool has_error() const { return m_error; }
  void clear_error() { m_error = false; }

  raw_istream(const raw_istream&) = delete;
  raw_istream& operator=(const raw_istream&) = delete;

 protected:
  void error_detected() { m_error = true; }

 private:
  virtual void read_impl(void* data, size_t len) = 0;

  bool m_error = false;
};

class raw_mem_istream : public raw_istream {
 public:
  explicit raw_mem_istream(llvm::StringRef mem);
  raw_mem_istream(const char* mem, size_t len) : m_cur(mem), m_left(len) {}
  void close() override;
  size_t in_avail() const override;

 private:
  void read_impl(void* data, size_t len) override;

  const char* m_cur;
  size_t m_left;
};

class raw_fd_istream : public raw_istream {
 public:
  raw_fd_istream(const llvm::Twine& filename, std::error_code& ec,
                 size_t bufSize = 4096);
  raw_fd_istream(int fd, bool shouldClose, size_t bufSize = 4096);
  ~raw_fd_istream() override;
  void close() override;
  size_t in_avail() const override;

 private:
  void read_impl(void* data, size_t len) override;

  char* m_buf;
  char* m_cur;
  char* m_end;
  size_t m_bufSize;
  int m_fd;
  bool m_shouldClose;
};

}  // namespace wpi

#endif  // WPIUTIL_SUPPORT_RAW_ISTREAM_H_
