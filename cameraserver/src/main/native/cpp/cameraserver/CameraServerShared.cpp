// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/cameraserver/CameraServerShared.hpp"

#include <memory>
#include <utility>

#include "wpi/util/mutex.hpp"

namespace {
class DefaultCameraServerShared : public wpi::CameraServerShared {
 public:
  void ReportUsage(std::string_view resource, std::string_view data) override {}
  void SetCameraServerErrorV(fmt::string_view format,
                             fmt::format_args args) override {}
  void SetVisionRunnerErrorV(fmt::string_view format,
                             fmt::format_args args) override {}
  void ReportDriverStationErrorV(fmt::string_view format,
                                 fmt::format_args args) override {}
  std::pair<std::thread::id, bool> GetRobotMainThreadId() const override {
    return std::pair{std::thread::id(), false};
  }
};
}  // namespace

static std::unique_ptr<wpi::CameraServerShared> cameraServerShared = nullptr;
static wpi::util::mutex setLock;

namespace wpi {
CameraServerShared* GetCameraServerShared() {
  std::unique_lock lock(setLock);
  if (!cameraServerShared) {
    cameraServerShared = std::make_unique<DefaultCameraServerShared>();
  }
  return cameraServerShared.get();
}
}  // namespace wpi

extern "C" {
void CameraServer_SetCameraServerShared(wpi::CameraServerShared* shared) {
  std::unique_lock lock(setLock);
  cameraServerShared.reset(shared);
}
}  // extern "C"
