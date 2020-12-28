// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#ifndef WPIUTIL_WPI_UV_ERROR_H_
#define WPIUTIL_WPI_UV_ERROR_H_

#include <uv.h>

namespace wpi::uv {

/**
 * Error code.
 */
class Error {
 public:
  Error() = default;
  explicit Error(int err) : m_err(err) {}

  /**
   * Boolean conversion.  Returns true if error, false if ok.
   */
  explicit operator bool() const { return m_err < 0; }

  /**
   * Returns the error code.
   */
  int code() const { return m_err; }

  /**
   * Returns the error message.
   */
  const char* str() const { return uv_strerror(m_err); }

  /**
   * Returns the error name.
   */
  const char* name() const { return uv_err_name(m_err); }

 private:
  int m_err{UV_UNKNOWN};
};

}  // namespace wpi::uv

#endif  // WPIUTIL_WPI_UV_ERROR_H_
