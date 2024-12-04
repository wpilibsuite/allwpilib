// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/RobotBase.h"

#ifdef __FRC_ROBORIO__
#include <dlfcn.h>
#endif

#include <cstdio>
#include <memory>
#include <string>
#include <utility>

#include <cameraserver/CameraServerShared.h>
#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/print.h>
#include <wpi/timestamp.h>
#include <wpimath/MathShared.h>

#include "WPILibVersion.h"
#include "frc/DriverStation.h"
#include "frc/Errors.h"
#include "frc/Notifier.h"
#include "frc/livewindow/LiveWindow.h"
#include "frc/smartdashboard/SmartDashboard.h"

static_assert(frc::RuntimeType::kRoboRIO ==
              static_cast<frc::RuntimeType>(HAL_Runtime_RoboRIO));
static_assert(frc::RuntimeType::kRoboRIO2 ==
              static_cast<frc::RuntimeType>(HAL_Runtime_RoboRIO2));
static_assert(frc::RuntimeType::kSimulation ==
              static_cast<frc::RuntimeType>(HAL_Runtime_Simulation));

using SetCameraServerSharedFP = void (*)(frc::CameraServerShared*);

using namespace frc;

int frc::RunHALInitialization() {
  if (!HAL_Initialize(500, 0)) {
    std::puts("FATAL ERROR: HAL could not be initialized");
    return -1;
  }
  DriverStation::RefreshData();
  HAL_Report(HALUsageReporting::kResourceType_Language,
             HALUsageReporting::kLanguage_CPlusPlus, 0, GetWPILibVersion());

  if (!frc::Notifier::SetHALThreadPriority(true, 40)) {
    FRC_ReportWarning("Setting HAL Notifier RT priority to 40 failed\n");
  }

  std::puts("\n********** Robot program starting **********");
  return 0;
}

std::thread::id RobotBase::m_threadId;

namespace {
class WPILibCameraServerShared : public frc::CameraServerShared {
 public:
  void ReportUsbCamera(int id) override {
    HAL_Report(HALUsageReporting::kResourceType_UsbCamera, id);
  }
  void ReportAxisCamera(int id) override {
    HAL_Report(HALUsageReporting::kResourceType_AxisCamera, id);
  }
  void ReportVideoServer(int id) override {
    HAL_Report(HALUsageReporting::kResourceType_PCVideoServer, id);
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
    frc::ReportErrorV(err::Error, __FILE__, __LINE__, __FUNCTION__, format,
                      args);
  }

  void ReportWarningV(fmt::string_view format, fmt::format_args args) override {
    frc::ReportErrorV(warn::Warning, __FILE__, __LINE__, __FUNCTION__, format,
                      args);
  }

  void ReportUsage(wpi::math::MathUsageId id, int count) override {
    switch (id) {
      case wpi::math::MathUsageId::kKinematics_DifferentialDrive:
        HAL_Report(HALUsageReporting::kResourceType_Kinematics,
                   HALUsageReporting::kKinematics_DifferentialDrive);
        break;
      case wpi::math::MathUsageId::kKinematics_MecanumDrive:
        HAL_Report(HALUsageReporting::kResourceType_Kinematics,
                   HALUsageReporting::kKinematics_MecanumDrive);
        break;
      case wpi::math::MathUsageId::kKinematics_SwerveDrive:
        HAL_Report(HALUsageReporting::kResourceType_Kinematics,
                   HALUsageReporting::kKinematics_SwerveDrive);
        break;
      case wpi::math::MathUsageId::kTrajectory_TrapezoidProfile:
        HAL_Report(HALUsageReporting::kResourceType_TrapezoidProfile, count);
        break;
      case wpi::math::MathUsageId::kFilter_Linear:
        HAL_Report(HALUsageReporting::kResourceType_LinearFilter, count);
        break;
      case wpi::math::MathUsageId::kOdometry_DifferentialDrive:
        HAL_Report(HALUsageReporting::kResourceType_Odometry,
                   HALUsageReporting::kOdometry_DifferentialDrive);
        break;
      case wpi::math::MathUsageId::kOdometry_SwerveDrive:
        HAL_Report(HALUsageReporting::kResourceType_Odometry,
                   HALUsageReporting::kOdometry_SwerveDrive);
        break;
      case wpi::math::MathUsageId::kOdometry_MecanumDrive:
        HAL_Report(HALUsageReporting::kResourceType_Odometry,
                   HALUsageReporting::kOdometry_MecanumDrive);
        break;
      case wpi::math::MathUsageId::kController_PIDController2:
        HAL_Report(HALUsageReporting::kResourceType_PIDController2, count);
        break;
      case wpi::math::MathUsageId::kController_ProfiledPIDController:
        HAL_Report(HALUsageReporting::kResourceType_ProfiledPIDController,
                   count);
        break;
      case wpi::math::MathUsageId::kController_BangBangController:
        HAL_Report(HALUsageReporting::kResourceType_BangBangController, count);
        break;
      case wpi::math::MathUsageId::kTrajectory_PathWeaver:
        HAL_Report(HALUsageReporting::kResourceType_PathWeaverTrajectory,
                   count);
        break;
    }
  }

  units::second_t GetTimestamp() override {
    return units::second_t{wpi::Now() * 1.0e-6};
  }
};
}  // namespace

static void SetupCameraServerShared() {
#ifdef __FRC_ROBORIO__
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

  auto inst = nt::NetworkTableInstance::GetDefault();
  // subscribe to "" to force persistent values to propagate to local
  nt::SubscribeMultiple(inst.GetHandle(), {{std::string_view{}}});
  if constexpr (!IsSimulation()) {
    inst.StartServer("/home/lvuser/networktables.json");
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
      wpi::print(stderr, "timed out while waiting for NT server to start\n");
      break;
    }
  }

  connListenerHandle = inst.AddConnectionListener(false, [&](const nt::Event&
                                                                 event) {
    if (event.Is(nt::EventFlags::kConnected)) {
      if (event.GetConnectionInfo()->remote_id.starts_with("glass")) {
        HAL_Report(HALUsageReporting::kResourceType_Dashboard,
                   HALUsageReporting::kDashboard_Glass);
        m_dashboardDetected = true;
      } else if (event.GetConnectionInfo()->remote_id.starts_with(
                     "SmartDashboard")) {
        HAL_Report(HALUsageReporting::kResourceType_Dashboard,
                   HALUsageReporting::kDashboard_SmartDashboard);
        m_dashboardDetected = true;
      } else if (event.GetConnectionInfo()->remote_id.starts_with(
                     "shuffleboard")) {
        HAL_Report(HALUsageReporting::kResourceType_Dashboard,
                   HALUsageReporting::kDashboard_Shuffleboard);
        m_dashboardDetected = true;
      } else if (event.GetConnectionInfo()->remote_id.starts_with("elastic") ||
                 event.GetConnectionInfo()->remote_id.starts_with("Elastic")) {
        HAL_Report(HALUsageReporting::kResourceType_Dashboard,
                   HALUsageReporting::kDashboard_Elastic);
        m_dashboardDetected = true;
      } else if (event.GetConnectionInfo()->remote_id.starts_with(
                     "Dashboard")) {
        HAL_Report(HALUsageReporting::kResourceType_Dashboard,
                   HALUsageReporting::kDashboard_LabVIEW);
        m_dashboardDetected = true;
      } else if (event.GetConnectionInfo()->remote_id.starts_with(
                     "AdvantageScope")) {
        HAL_Report(HALUsageReporting::kResourceType_Dashboard,
                   HALUsageReporting::kDashboard_AdvantageScope);
        m_dashboardDetected = true;
      } else if (event.GetConnectionInfo()->remote_id.starts_with(
                     "QFRCDashboard")) {
        HAL_Report(HALUsageReporting::kResourceType_Dashboard,
                   HALUsageReporting::kDashboard_QFRCDashboard);
        m_dashboardDetected = true;
      } else if (event.GetConnectionInfo()->remote_id.starts_with(
                     "FRC Web Components")) {
        HAL_Report(HALUsageReporting::kResourceType_Dashboard,
                   HALUsageReporting::kDashboard_FRCWebComponents);
        m_dashboardDetected = true;
      } else {
        if (!m_dashboardDetected) {
          size_t delim = event.GetConnectionInfo()->remote_id.find('@');
          if (delim != std::string::npos) {
            HAL_Report(
                HALUsageReporting::kResourceType_Dashboard,
                HALUsageReporting::kDashboard_Unknown, 0,
                event.GetConnectionInfo()->remote_id.substr(0, delim).c_str());
          } else {
            HAL_Report(HALUsageReporting::kResourceType_Dashboard,
                       HALUsageReporting::kDashboard_Unknown, 0,
                       event.GetConnectionInfo()->remote_id.c_str());
          }
        }
      }
    }
  });

  SmartDashboard::init();

  if constexpr (!IsSimulation()) {
    std::FILE* file = nullptr;
    file = std::fopen("/tmp/frc_versions/FRC_Lib_Version.ini", "w");

    if (file != nullptr) {
      std::fputs("C++ ", file);
      std::fputs(GetWPILibVersion(), file);
      std::fclose(file);
    }
  }

  // Call DriverStation::RefreshData() to kick things off
  DriverStation::RefreshData();

  // First and one-time initialization
  LiveWindow::SetEnabled(false);
}
