/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_RAW_ISTREAM_H_
#define WPIUTIL_WPI_RAW_ISTREAM_H_

#include <stdint.h>

#include <algorithm>
#include <cstddef>
#include <string>
#include <system_error>
#include <vector>

#include "wpi/ArrayRef.h"
#include "wpi/SmallVector.h"
#include "wpi/StringRef.h"
#include "wpi/Twine.h"

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
    return m_read_count;
  }

  raw_istream& readinto(SmallVectorImpl<char>& buf, size_t len) {
    size_t old_size = buf.size();
    buf.append(len, 0);
    read_impl(&buf[old_size], len);
    buf.resize(old_size + m_read_count);
    return *this;
  }

  raw_istream& readinto(SmallVectorImpl<uint8_t>& buf, size_t len) {
    size_t old_size = buf.size();
    buf.append(len, 0);
    read_impl(&buf[old_size], len);
    buf.resize(old_size + m_read_count);
    return *this;
  }

  raw_istream& readinto(std::vector<char>& buf, size_t len) {
    size_t old_size = buf.size();
    buf.insert(buf.end(), len, 0);
    read_impl(&buf[old_size], len);
    buf.resize(old_size + m_read_count);
    return *this;
  }

  raw_istream& readinto(std::vector<uint8_t>& buf, size_t len) {
    size_t old_size = buf.size();
    buf.insert(buf.end(), len, 0);
    read_impl(&buf[old_size], len);
    buf.resize(old_size + m_read_count);
    return *this;
  }

  raw_istream& readinto(std::string& buf, size_t len) {
    size_t old_size = buf.size();
    buf.insert(buf.end(), len, 0);
    read_impl(&buf[old_size], len);
    buf.resize(old_size + m_read_count);
    return *this;
  }

  // Read a line from an input stream (up to a maximum length).
  // The returned buffer will contain the trailing \n (unless the maximum length
  // was reached).  \r's are stripped from the buffer.
  // @param buf Buffer for output
  // @param maxLen Maximum length
  // @return Line
  StringRef getline(SmallVectorImpl<char>& buf, int maxLen);

  virtual void close() = 0;

  // Number of bytes available to read without potentially blocking.
  // Note this can return zero even if there are bytes actually available to
  // read.
  virtual size_t in_avail() const = 0;

  // Return the number of bytes read by the last read operation.
  size_t read_count() const { return m_read_count; }

  bool has_error() const { return m_error; }
  void clear_error() { m_error = false; }

  raw_istream(const raw_istream&) = delete;
  raw_istream& operator=(const raw_istream&) = delete;

 protected:
  void error_detected() { m_error = true; }
  void set_read_count(size_t count) { m_read_count = count; }

 private:
  virtual void read_impl(void* data, size_t len) = 0;

  bool m_error = false;
  size_t m_read_count = 0;
};

class raw_mem_istream : public raw_istream {
 public:
  // not const as we don't want to allow temporaries
  explicit raw_mem_istream(std::string& str)
      : raw_mem_istream(str.data(), str.size()) {}
  explicit raw_mem_istream(ArrayRef<char> mem)
      : raw_mem_istream(mem.data(), mem.size()) {}
  explicit raw_mem_istream(ArrayRef<uint8_t> mem)
      : raw_mem_istream(reinterpret_cast<const char*>(mem.data()), mem.size()) {
  }
  explicit raw_mem_istream(const char* str)
      : m_cur(str), m_left(std::strlen(str)) {}
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
  raw_fd_istream(const Twine& filename, std::error_code& ec,
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

#endif  // WPIUTIL_WPI_RAW_ISTREAM_H_
