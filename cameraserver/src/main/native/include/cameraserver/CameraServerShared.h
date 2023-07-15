// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <thread>
#include <utility>

#include <fmt/format.h>

namespace frc {
class CameraServerShared {
 public:
  virtual ~CameraServerShared() = default;
  virtual void ReportUsbCamera(int id) = 0;
  virtual void ReportAxisCamera(int id) = 0;
  virtual void ReportVideoServer(int id) = 0;
  virtual void SetCameraServerErrorV(fmt::string_view format,
                                     fmt::format_args args) = 0;
  virtual void SetVisionRunnerErrorV(fmt::string_view format,
                                     fmt::format_args args) = 0;
  virtual void ReportDriverStationErrorV(fmt::string_view format,
                                         fmt::format_args args) = 0;
  virtual std::pair<std::thread::id, bool> GetRobotMainThreadId() const = 0;

  template <typename S, typename... Args>
  inline void SetCameraServerError(const S& format, Args&&... args) {
    SetCameraServerErrorV(format, fmt::make_format_args(args...));
  }

  template <typename S, typename... Args>
  inline void SetVisionRunnerError(const S& format, Args&&... args) {
    SetVisionRunnerErrorV(format, fmt::make_format_args(args...));
  }

  template <typename S, typename... Args>
  inline void ReportDriverStationError(const S& format, Args&&... args) {
    ReportDriverStationErrorV(format, fmt::make_format_args(args...));
  }
};

CameraServerShared* GetCameraServerShared();
}  // namespace frc

extern "C" {
// Takes ownership
void CameraServer_SetCameraServerShared(frc::CameraServerShared* shared);
}  // extern "C"
