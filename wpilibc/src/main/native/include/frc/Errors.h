// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <memory>
#include <stdexcept>
#include <string>

#include <fmt/format.h>

namespace frc {

/**
 * Runtime error exception.
 */
class RuntimeError : public std::runtime_error {
 public:
  RuntimeError(int32_t code, std::string&& loc, std::string&& stack,
               std::string&& message);
  RuntimeError(int32_t code, const char* fileName, int lineNumber,
               const char* funcName, std::string&& stack,
               std::string&& message);

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
const char* GetErrorMessage(int32_t* code);

/**
 * Reports an error to the driver station (using HAL_SendError).
 * Generally the FRC_ReportError wrapper macro should be used instead.
 *
 * @param[out] status error code
 * @param[in]  fileName source file name
 * @param[in]  lineNumber source line number
 * @param[in]  funcName source function name
 * @param[in]  format error message format
 * @param[in]  args error message format args
 */
void ReportErrorV(int32_t status, const char* fileName, int lineNumber,
                  const char* funcName, fmt::string_view format,
                  fmt::format_args args);

/**
 * Reports an error to the driver station (using HAL_SendError).
 * Generally the FRC_ReportError wrapper macro should be used instead.
 *
 * @param[out] status error code
 * @param[in]  fileName source file name
 * @param[in]  lineNumber source line number
 * @param[in]  funcName source function name
 * @param[in]  format error message format
 * @param[in]  args error message format args
 */
template <typename... Args>
inline void ReportError(int32_t status, const char* fileName, int lineNumber,
                        const char* funcName, fmt::string_view format,
                        Args&&... args) {
  ReportErrorV(status, fileName, lineNumber, funcName, format,
               fmt::make_format_args(args...));
}

/**
 * Makes a runtime error exception object. This object should be thrown
 * by the caller. Generally the FRC_MakeError wrapper macro should be used
 * instead.
 *
 * @param[out] status error code
 * @param[in]  fileName source file name
 * @param[in]  lineNumber source line number
 * @param[in]  funcName source function name
 * @param[in]  format error message format
 * @param[in]  args error message format args
 * @return runtime error object
 */
[[nodiscard]]
RuntimeError MakeErrorV(int32_t status, const char* fileName, int lineNumber,
                        const char* funcName, fmt::string_view format,
                        fmt::format_args args);

template <typename... Args>
[[nodiscard]]
inline RuntimeError MakeError(int32_t status, const char* fileName,
                              int lineNumber, const char* funcName,
                              fmt::string_view format, Args&&... args) {
  return MakeErrorV(status, fileName, lineNumber, funcName, format,
                    fmt::make_format_args(args...));
}

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

// C++20 relaxed the number of arguments to variadics, but Apple Clang's
// warnings haven't caught up yet: https://stackoverflow.com/a/67996331
#ifdef __clang__
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

/**
 * Reports an error to the driver station (using HAL_SendError).
 *
 * @param[out] status error code
 * @param[in]  format error message format
 */
#define FRC_ReportError(status, format, ...)                             \
  do {                                                                   \
    if ((status) != 0) {                                                 \
      ::frc::ReportError(status, __FILE__, __LINE__, __FUNCTION__,       \
                         FMT_STRING(format) __VA_OPT__(, ) __VA_ARGS__); \
    }                                                                    \
  } while (0)

/**
 * Makes a runtime error exception object. This object should be thrown
 * by the caller.
 *
 * @param[out] status error code
 * @param[in]  format error message format
 * @return runtime error object
 */
#define FRC_MakeError(status, format, ...)                   \
  ::frc::MakeError(status, __FILE__, __LINE__, __FUNCTION__, \
                   FMT_STRING(format) __VA_OPT__(, ) __VA_ARGS__)

/**
 * Checks a status code and depending on its value, either throws a
 * RuntimeError exception, calls ReportError, or does nothing (if no error).
 *
 * @param[out] status error code
 * @param[in]  format error message format
 */
#define FRC_CheckErrorStatus(status, format, ...)                            \
  do {                                                                       \
    if ((status) < 0) {                                                      \
      throw ::frc::MakeError(status, __FILE__, __LINE__, __FUNCTION__,       \
                             FMT_STRING(format) __VA_OPT__(, ) __VA_ARGS__); \
    } else if ((status) > 0) {                                               \
      ::frc::ReportError(status, __FILE__, __LINE__, __FUNCTION__,           \
                         FMT_STRING(format) __VA_OPT__(, ) __VA_ARGS__);     \
    }                                                                        \
  } while (0)

#define FRC_AssertMessage(condition, format, ...)                            \
  do {                                                                       \
    if (!(condition)) {                                                      \
      throw ::frc::MakeError(err::AssertionFailure, __FILE__, __LINE__,      \
                             __FUNCTION__,                                   \
                             FMT_STRING(format) __VA_OPT__(, ) __VA_ARGS__); \
    }                                                                        \
  } while (0)

#define FRC_Assert(condition) FRC_AssertMessage(condition, #condition)
