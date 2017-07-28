/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_SUPPORT_RAW_ISTREAM_H_
#define WPIUTIL_SUPPORT_RAW_ISTREAM_H_

#include <algorithm>
#include <cstddef>

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

  raw_istream& read(void* data, std::size_t len) {
    read_impl(data, len);
    return *this;
  }

  std::size_t readsome(void* data, std::size_t len) {
    std::size_t readlen = std::min(in_avail(), len);
    if (readlen == 0) return 0;
    read_impl(data, readlen);
    return readlen;
  };

  virtual void close() = 0;
  virtual std::size_t in_avail() const = 0;

  bool has_error() const { return m_error; }
  void clear_error() { m_error = false; }

  raw_istream(const raw_istream&) = delete;
  raw_istream& operator=(const raw_istream&) = delete;

 protected:
  void error_detected() { m_error = true; }

 private:
  virtual void read_impl(void* data, std::size_t len) = 0;

  bool m_error = false;
};

class raw_mem_istream : public raw_istream {
 public:
  raw_mem_istream(const char* mem, std::size_t len) : m_cur(mem), m_left(len) {}
  void close() override;
  std::size_t in_avail() const override;

 private:
  void read_impl(void* data, std::size_t len) override;

  const char* m_cur;
  std::size_t m_left;
};

class raw_fd_istream : public raw_istream {
 public:
  raw_fd_istream(int fd, bool shouldClose, std::size_t bufSize = 4096);
  ~raw_fd_istream() override;
  void close() override;
  std::size_t in_avail() const override;

 private:
  void read_impl(void* data, std::size_t len) override;

  char* m_buf;
  char* m_cur;
  char* m_end;
  std::size_t m_bufSize;
  int m_fd;
  bool m_shouldClose;
};

}  // namespace wpi

#endif  // WPIUTIL_SUPPORT_RAW_ISTREAM_H_
