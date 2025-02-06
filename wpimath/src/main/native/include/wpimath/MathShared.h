// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>

#include <fmt/format.h>
#include <wpi/SymbolExports.h>

#include "units/time.h"

namespace wpi::math {

class WPILIB_DLLEXPORT MathShared {
 public:
  virtual ~MathShared() = default;
  virtual void ReportErrorV(fmt::string_view format, fmt::format_args args) = 0;
  virtual void ReportWarningV(fmt::string_view format,
                              fmt::format_args args) = 0;
  virtual void ReportUsage(std::string_view resource,
                           std::string_view data) = 0;
  virtual units::second_t GetTimestamp() = 0;

  template <typename S, typename... Args>
  inline void ReportError(const S& format, Args&&... args) {
    ReportErrorV(format, fmt::make_format_args(args...));
  }

  template <typename S, typename... Args>
  inline void ReportWarning(const S& format, Args&&... args) {
    ReportWarningV(format, fmt::make_format_args(args...));
  }
};

class WPILIB_DLLEXPORT MathSharedStore {
 public:
  static MathShared& GetMathShared();

  static void SetMathShared(std::unique_ptr<MathShared> shared);

  static void ReportErrorV(fmt::string_view format, fmt::format_args args) {
    GetMathShared().ReportErrorV(format, args);
  }

  template <typename S, typename... Args>
  static inline void ReportError(const S& format, Args&&... args) {
    ReportErrorV(format, fmt::make_format_args(args...));
  }

  static void ReportWarningV(fmt::string_view format, fmt::format_args args) {
    GetMathShared().ReportWarningV(format, args);
  }

  template <typename S, typename... Args>
  static inline void ReportWarning(const S& format, Args&&... args) {
    ReportWarningV(format, fmt::make_format_args(args...));
  }

  static void ReportUsage(std::string_view resource, std::string_view data) {
    GetMathShared().ReportUsage(resource, data);
  }

  static units::second_t GetTimestamp() {
    return GetMathShared().GetTimestamp();
  }
};

}  // namespace wpi::math
