// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <exception>
#include <memory>
#include <string>

#include <wpi/Twine.h>

namespace frc {

/**
 * Runtime error exception.
 */
class RuntimeError : public std::runtime_error {
 public:
  RuntimeError(int32_t code, const wpi::Twine& message, const wpi::Twine& loc,
               wpi::StringRef stack);
  RuntimeError(int32_t code, const wpi::Twine& message, const char* fileName,
               int lineNumber, const char* funcName, wpi::StringRef stack);

  int32_t code() const noexcept { return m_data->code; }
  const char* loc() const noexcept { return m_data->loc.c_str(); }
  const char* stack() const noexcept { return m_data->stack.c_str(); }

  /**
   * Reports error to Driver Station (using HAL_SendError).
   */
  void Report() const;

 private:
  struct Data {
    int32_t code;
    std::string loc;
    std::string stack;
  };
  std::shared_ptr<Data> m_data;
};

/**
 * Gets error message string for an error code.
 */
const char* GetErrorMessage(int32_t code);

/**
 * Reports an error to the driver station (using HAL_SendError).
 * Generally the FRC_ReportError wrapper macro should be used instead.
 *
 * @param status error code
 * @param message error message details
 * @param fileName source file name
 * @param lineNumber source line number
 * @param funcName source function name
 */
void ReportError(int32_t status, const wpi::Twine& message,
                 const char* fileName, int lineNumber, const char* funcName);

/**
 * Makes a runtime error exception object. This object should be thrown
 * by the caller. Generally the FRC_MakeError wrapper macro should be used
 * instead.
 *
 * @param status error code
 * @param message error message details
 * @param fileName source file name
 * @param lineNumber source line number
 * @param funcName source function name
 * @return runtime error object
 */
[[nodiscard]] RuntimeError MakeError(int32_t status, const wpi::Twine& message,
                                     const char* fileName, int lineNumber,
                                     const char* funcName);

namespace err {
#define S(label, offset, message) inline constexpr int label = offset;
#include "frc/WPIErrors.mac"
#undef S
}  // namespace err

namespace warn {
#define S(label, offset, message) inline constexpr int label = offset;
#include "frc/WPIWarnings.mac"
#undef S
}  // namespace warn
}  // namespace frc

/**
 * Reports an error to the driver station (using HAL_SendError).
 *
 * @param status error code
 * @param message error message details
 */
#define FRC_ReportError(status, message)                                     \
  do {                                                                       \
    if ((status) != 0) {                                                     \
      ::frc::ReportError(status, message, __FILE__, __LINE__, __FUNCTION__); \
    }                                                                        \
  } while (0)

/**
 * Makes a runtime error exception object. This object should be thrown
 * by the caller.
 *
 * @param status error code
 * @param message error message details
 * @return runtime error object
 */
#define FRC_MakeError(status, message) \
  ::frc::MakeError(status, message, __FILE__, __LINE__, __FUNCTION__)

/**
 * Checks a status code and depending on its value, either throws a
 * RuntimeError exception, calls ReportError, or does nothing (if no error).
 *
 * @param status error code
 * @param message error message details
 */
#define FRC_CheckErrorStatus(status, message) \
  do {                                        \
    if ((status) < 0) {                       \
      throw FRC_MakeError(status, message);   \
    } else if ((status) > 0) {                \
      FRC_ReportError(status, message);       \
    }                                         \
  } while (0)

#define FRC_AssertMessage(condition, message)              \
  do {                                                     \
    if (!(condition)) {                                    \
      throw FRC_MakeError(err::AssertionFailure, message); \
    }                                                      \
  } while (0)

#define FRC_Assert(condition) FRC_AssertMessage(condition, #condition)
