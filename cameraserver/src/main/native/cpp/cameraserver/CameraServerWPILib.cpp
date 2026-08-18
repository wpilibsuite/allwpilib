// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <format>
#include <string_view>
#include <thread>
#include <utility>

#include "wpi/cameraserver/CameraServer.hpp"
#include "wpi/cameraserver/CameraServerShared.hpp"
#include "wpi/framework/RobotBase.hpp"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/system/Errors.hpp"

using namespace wpi;

namespace {

class WPILibCameraServerShared : public CameraServerShared {
 public:
  void ReportUsage(std::string_view resource, std::string_view data) override {
    HAL_ReportUsage(resource, data);
  }

  void SetCameraServerErrorV(std::string_view format,
                             std::format_args args) override {
    ReportErrorV(err::CameraServerError, __FILE__, __LINE__, __FUNCTION__,
                 format, args);
  }

  void SetVisionRunnerErrorV(std::string_view format,
                             std::format_args args) override {
    ReportErrorV(err::Error, __FILE__, __LINE__, __FUNCTION__, format, args);
  }

  void ReportDriverStationErrorV(std::string_view format,
                                 std::format_args args) override {
    ReportErrorV(err::Error, __FILE__, __LINE__, __FUNCTION__, format, args);
  }

  std::pair<std::thread::id, bool> GetRobotMainThreadId() const override {
    return std::pair{RobotBase::GetThreadId(), true};
  }
};

}  // namespace

void CameraServer::Initialize() {
  CameraServer_SetCameraServerShared(new WPILibCameraServerShared{});
}
