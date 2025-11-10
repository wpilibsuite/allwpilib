// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/framework/RobotBase.hpp"

#ifdef __FRC_SYSTEMCORE__
#include <dlfcn.h>
#endif

#include <cstdio>
#include <memory>
#include <string>
#include <utility>

#include "wpi/cameraserver/CameraServerShared.hpp"
#include "wpi/driverstation/DriverStation.hpp"
#include "wpi/hal/HALBase.h"
#include "wpi/hal/UsageReporting.h"
#include "wpi/math/util/MathShared.hpp"
#include "wpi/nt/NetworkTable.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/system/Notifier.hpp"
#include "wpi/system/WPILibVersion.hpp"
#include "wpi/util/print.hpp"
#include "wpi/util/timestamp.h"

static_assert(wpi::RuntimeType::kRoboRIO ==
              static_cast<wpi::RuntimeType>(HAL_Runtime_RoboRIO));
static_assert(wpi::RuntimeType::kRoboRIO2 ==
              static_cast<wpi::RuntimeType>(HAL_Runtime_RoboRIO2));
static_assert(wpi::RuntimeType::kSimulation ==
              static_cast<wpi::RuntimeType>(HAL_Runtime_Simulation));
static_assert(wpi::RuntimeType::kSystemcore ==
              static_cast<wpi::RuntimeType>(HAL_Runtime_Systemcore));

using SetCameraServerSharedFP = void (*)(wpi::CameraServerShared*);

using namespace wpi;

int wpi::RunHALInitialization() {
  if (!HAL_Initialize(500, 0)) {
    std::puts("FATAL ERROR: HAL could not be initialized");
    return -1;
  }
  DriverStation::RefreshData();
  HAL_ReportUsage("Language", "C++");
  HAL_ReportUsage("WPILibVersion", GetWPILibVersion());

  if (!wpi::Notifier::SetHALThreadPriority(true, 40)) {
    WPILIB_ReportWarning("Setting HAL Notifier RT priority to 40 failed\n");
  }

  std::puts("\n********** Robot program starting **********");
  return 0;
}

std::thread::id RobotBase::m_threadId;

namespace {
class WPILibCameraServerShared : public wpi::CameraServerShared {
 public:
  void ReportUsage(std::string_view resource, std::string_view data) override {
    HAL_ReportUsage(resource, data);
  }
  void SetCameraServerErrorV(fmt::string_view format,
                             fmt::format_args args) override {
    ReportErrorV(err::CameraServerError, __FILE__, __LINE__, __FUNCTION__,
                 format, args);
  }
  void SetVisionRunnerErrorV(fmt::string_view format,
                             fmt::format_args args) override {
    ReportErrorV(err::Error, __FILE__, __LINE__, __FUNCTION__, format, args);
  }
  void ReportDriverStationErrorV(fmt::string_view format,
                                 fmt::format_args args) override {
    ReportErrorV(err::Error, __FILE__, __LINE__, __FUNCTION__, format, args);
  }
  std::pair<std::thread::id, bool> GetRobotMainThreadId() const override {
    return std::pair{RobotBase::GetThreadId(), true};
  }
};
class WPILibMathShared : public wpi::math::MathShared {
 public:
  void ReportErrorV(fmt::string_view format, fmt::format_args args) override {
    wpi::ReportErrorV(err::Error, __FILE__, __LINE__, __FUNCTION__, format,
                      args);
  }

  void ReportWarningV(fmt::string_view format, fmt::format_args args) override {
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

static void SetupCameraServerShared() {
#ifdef __FRC_SYSTEMCORE__
#ifdef DYNAMIC_CAMERA_SERVER
#ifdef DYNAMIC_CAMERA_SERVER_DEBUG
  auto cameraServerLib = dlopen("libcameraserverd.so", RTLD_NOW);
#else
  auto cameraServerLib = dlopen("libcameraserver.so", RTLD_NOW);
#endif

  if (!cameraServerLib) {
    std::puts("Camera Server Library Not Found");
    std::fflush(stdout);
    return;
  }
  auto symbol = dlsym(cameraServerLib, "CameraServer_SetCameraServerShared");
  if (symbol) {
    auto setCameraServerShared = (SetCameraServerSharedFP)symbol;
    setCameraServerShared(new WPILibCameraServerShared{});
  } else {
    std::puts("Camera Server Shared Symbol Missing");
    std::fflush(stdout);
  }
#else
  CameraServer_SetCameraServerShared(new WPILibCameraServerShared{});
#endif
#else
  std::puts("Not loading CameraServerShared");
  std::fflush(stdout);
#endif
}

static void SetupMathShared() {
  wpi::math::MathSharedStore::SetMathShared(
      std::make_unique<WPILibMathShared>());
}

bool RobotBase::IsEnabled() const {
  return DriverStation::IsEnabled();
}

bool RobotBase::IsDisabled() const {
  return DriverStation::IsDisabled();
}

bool RobotBase::IsAutonomous() const {
  return DriverStation::IsAutonomous();
}

bool RobotBase::IsAutonomousEnabled() const {
  return DriverStation::IsAutonomousEnabled();
}

bool RobotBase::IsTeleop() const {
  return DriverStation::IsTeleop();
}

bool RobotBase::IsTeleopEnabled() const {
  return DriverStation::IsTeleopEnabled();
}

bool RobotBase::IsTest() const {
  return DriverStation::IsTest();
}

bool RobotBase::IsTestEnabled() const {
  return DriverStation::IsTestEnabled();
}

std::thread::id RobotBase::GetThreadId() {
  return m_threadId;
}

RuntimeType RobotBase::GetRuntimeType() {
  return static_cast<RuntimeType>(HAL_GetRuntimeType());
}

RobotBase::RobotBase() {
  m_threadId = std::this_thread::get_id();

  SetupCameraServerShared();
  SetupMathShared();

  auto inst = wpi::nt::NetworkTableInstance::GetDefault();
  // subscribe to "" to force persistent values to propagate to local
  wpi::nt::SubscribeMultiple(inst.GetHandle(), {{std::string_view{}}});
  if constexpr (!IsSimulation()) {
    inst.StartServer("/home/systemcore/networktables.json");
  } else {
    inst.StartServer();
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
        if (event.Is(wpi::nt::EventFlags::kConnected)) {
          auto connInfo = event.GetConnectionInfo();
          HAL_ReportUsage(fmt::format("NT/{}", connInfo->remote_id), "");
        }
      });

  SmartDashboard::init();

  // Call DriverStation::RefreshData() to kick things off
  DriverStation::RefreshData();
}
