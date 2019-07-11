/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

void SetCameraServerShared(std::unique_ptr<CameraServerShared> shared);
CameraServerShared* GetCameraServerShared();
}  // namespace frc
