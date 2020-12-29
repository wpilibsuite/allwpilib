// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <thread>
#include <utility>

#include <wpi/Twine.h>

namespace frc {
class CameraServerShared {
 public:
  virtual ~CameraServerShared() = default;
  virtual void ReportUsbCamera(int id) = 0;
  virtual void ReportAxisCamera(int id) = 0;
  virtual void ReportVideoServer(int id) = 0;
  virtual void SetCameraServerError(const wpi::Twine& error) = 0;
  virtual void SetVisionRunnerError(const wpi::Twine& error) = 0;
  virtual void ReportDriverStationError(const wpi::Twine& error) = 0;
  virtual std::pair<std::thread::id, bool> GetRobotMainThreadId() const = 0;
};

CameraServerShared* GetCameraServerShared();
}  // namespace frc

extern "C" {
// Takes ownership
void CameraServer_SetCameraServerShared(frc::CameraServerShared* shared);
}  // extern "C"
