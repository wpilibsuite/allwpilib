// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "cameraserver/CameraServerShared.h"

#include <wpi/mutex.h>

namespace {
class DefaultCameraServerShared : public frc::CameraServerShared {
 public:
  void ReportUsbCamera(int id) override {}
  void ReportAxisCamera(int id) override {}
  void ReportVideoServer(int id) override {}
  void SetCameraServerError(const wpi::Twine& error) override {}
  void SetVisionRunnerError(const wpi::Twine& error) override {}
  void ReportDriverStationError(const wpi::Twine& error) override {}
  std::pair<std::thread::id, bool> GetRobotMainThreadId() const override {
    return std::make_pair(std::thread::id(), false);
  }
};
}  // namespace

static std::unique_ptr<frc::CameraServerShared> cameraServerShared = nullptr;
static wpi::mutex setLock;

namespace frc {
CameraServerShared* GetCameraServerShared() {
  std::unique_lock lock(setLock);
  if (!cameraServerShared) {
    cameraServerShared = std::make_unique<DefaultCameraServerShared>();
  }
  return cameraServerShared.get();
}
}  // namespace frc

extern "C" {
void CameraServer_SetCameraServerShared(frc::CameraServerShared* shared) {
  std::unique_lock lock(setLock);
  cameraServerShared.reset(shared);
}
}  // extern "C"
