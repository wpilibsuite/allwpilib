/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#ifndef WPIUTIL_WPI_UV_ERROR_H_
#define WPIUTIL_WPI_UV_ERROR_H_

#include <uv.h>

namespace wpi {
namespace uv {

/**
 * Error code.
 */
class Error {
 public:
  Error() : m_err(UV_UNKNOWN) {}
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
  int m_err;
};

}  // namespace uv
}  // namespace wpi

#endif  // WPIUTIL_WPI_UV_ERROR_H_
