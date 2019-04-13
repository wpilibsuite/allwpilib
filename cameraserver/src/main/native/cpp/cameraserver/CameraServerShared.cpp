/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

namespace frc {

static std::unique_ptr<CameraServerShared> cameraServerShared = nullptr;
static wpi::mutex setLock;

void SetCameraServerShared(std::unique_ptr<CameraServerShared> shared) {
  std::unique_lock<wpi::mutex> lock(setLock);
  cameraServerShared = std::move(shared);
}
CameraServerShared* GetCameraServerShared() {
  std::unique_lock<wpi::mutex> lock(setLock);
  if (!cameraServerShared) {
    cameraServerShared = std::make_unique<DefaultCameraServerShared>();
  }
  return cameraServerShared.get();
}
}  // namespace frc
