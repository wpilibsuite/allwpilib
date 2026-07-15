// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/RobotBase.hpp"

#include <stdint.h>

#include <cstdio>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "wpi/driverstation/RobotState.hpp"
#include "wpi/driverstation/internal/DriverStationBackend.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/hal/UsageReporting.hpp"
#include "wpi/math/util/MathShared.hpp"
#include "wpi/nt/NetworkTable.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/system/WPILibVersion.hpp"
#include "wpi/util/print.hpp"
#include "wpi/util/timestamp.hpp"

static_assert(wpi::RuntimeType::SYSTEMCORE ==
              static_cast<wpi::RuntimeType>(HAL_RUNTIME_SYSTEMCORE));
static_assert(wpi::RuntimeType::SIMULATION ==
              static_cast<wpi::RuntimeType>(HAL_RUNTIME_SIMULATION));

using namespace wpi;

int wpi::RunHALInitialization() {
  if (!HAL_Initialize(500, 0)) {
    std::puts("FATAL ERROR: HAL could not be initialized");
    return -1;
  }
  wpi::internal::DriverStationBackend::RefreshData();
  HAL_ReportUsage("Language", "C++");
  HAL_ReportUsage("WPILibVersion", GetWPILibVersion());
  HAL_PublishWpilibVersion(std::format("{} (C++)", GetWPILibVersion()));

  std::puts("\n********** Robot program starting **********");
  return 0;
}

std::thread::id RobotBase::m_threadId;

namespace {
class WPILibMathShared : public wpi::math::MathShared {
 public:
  void ReportErrorV(std::string_view format, std::format_args args) override {
    wpi::ReportErrorV(err::Error, __FILE__, __LINE__, __FUNCTION__, format,
                      args);
  }

  void ReportWarningV(std::string_view format, std::format_args args) override {
    wpi::ReportErrorV(warn::Warning, __FILE__, __LINE__, __FUNCTION__, format,
                      args);
  }

  void ReportUsage(std::string_view resource, std::string_view data) override {
    HAL_ReportUsage(resource, data);
  }

  wpi::units::second_t GetTimestamp() override {
    return wpi::units::second_t{wpi::util::Now() * 1.0e-6};
  }
};
}  // namespace

static void SetupMathShared() {
  wpi::math::MathSharedStore::SetMathShared(
      std::make_unique<WPILibMathShared>());
}

bool RobotBase::IsEnabled() {
  return RobotState::IsEnabled();
}

bool RobotBase::IsDisabled() {
  return RobotState::IsDisabled();
}

bool RobotBase::IsAutonomous() {
  return RobotState::IsAutonomous();
}

bool RobotBase::IsAutonomousEnabled() {
  return RobotState::IsAutonomousEnabled();
}

bool RobotBase::IsTeleop() {
  return RobotState::IsTeleop();
}

bool RobotBase::IsTeleopEnabled() {
  return RobotState::IsTeleopEnabled();
}

bool RobotBase::IsUtility() {
  return RobotState::IsUtility();
}

bool RobotBase::IsUtilityEnabled() {
  return RobotState::IsUtilityEnabled();
}

int64_t RobotBase::GetOpModeId() {
  return RobotState::GetOpModeId();
}

std::string RobotBase::GetOpMode() {
  return RobotState::GetOpMode();
}

std::thread::id RobotBase::GetThreadId() {
  return m_threadId;
}

RuntimeType RobotBase::GetRuntimeType() {
  return static_cast<RuntimeType>(HAL_GetRuntimeType());
}

RobotBase::RobotBase() {
  m_threadId = std::this_thread::get_id();

  SetupMathShared();

  auto inst = wpi::nt::NetworkTableInstance::GetDefault();
  // subscribe to "" to force persistent values to propagate to local
  wpi::nt::SubscribeMultiple(inst.GetHandle(), {{std::string_view{}}},
                             {.disableSignal = true});
  if constexpr (!IsSimulation()) {
    inst.StartServer("/home/systemcore/networktables.json", "", "robot");
  } else {
    inst.StartServer("networktables.json", "", "robot");
  }

  // wait for the NT server to actually start
  int count = 0;
  while ((inst.GetNetworkMode() & NT_NET_MODE_STARTING) != 0) {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(10ms);
    ++count;
    if (count > 100) {
      wpi::util::print(stderr,
                       "timed out while waiting for NT server to start\n");
      break;
    }
  }

  connListenerHandle =
      inst.AddConnectionListener(false, [&](const wpi::nt::Event& event) {
        if (event.Is(wpi::nt::EventFlags::CONNECTED)) {
          auto connInfo = event.GetConnectionInfo();
          HAL_ReportUsage(std::format("NT/{}", connInfo->remote_id), "");
        }
      });

  SmartDashboard::init();

  // Call wpi::internal::DriverStationBackend::RefreshData() to kick things off
  wpi::internal::DriverStationBackend::RefreshData();
}
