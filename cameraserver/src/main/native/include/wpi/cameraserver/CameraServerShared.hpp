// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <format>
#include <memory>
#include <string_view>
#include <thread>
#include <utility>

namespace wpi {
class CameraServerShared {
 public:
  virtual ~CameraServerShared() = default;
  virtual void ReportUsage(std::string_view resource,
                           std::string_view data) = 0;
  virtual void SetCameraServerErrorV(std::string_view format,
                                     std::format_args args) = 0;
  virtual void SetVisionRunnerErrorV(std::string_view format,
                                     std::format_args args) = 0;
  virtual void ReportDriverStationErrorV(std::string_view format,
                                         std::format_args args) = 0;
  virtual std::pair<std::thread::id, bool> GetRobotMainThreadId() const = 0;

  template <typename S, typename... Args>
  inline void SetCameraServerError(const S& format, Args&&... args) {
    SetCameraServerErrorV(format, std::make_format_args(args...));
  }

  template <typename S, typename... Args>
  inline void SetVisionRunnerError(const S& format, Args&&... args) {
    SetVisionRunnerErrorV(format, std::make_format_args(args...));
  }

  template <typename S, typename... Args>
  inline void ReportDriverStationError(const S& format, Args&&... args) {
    ReportDriverStationErrorV(format, std::make_format_args(args...));
  }
};

CameraServerShared* GetCameraServerShared();
}  // namespace wpi

extern "C" {
// Takes ownership
void CameraServer_SetCameraServerShared(wpi::CameraServerShared* shared);
}  // extern "C"
