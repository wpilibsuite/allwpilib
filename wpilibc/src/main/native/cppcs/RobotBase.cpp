/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/RobotBase.h"

#ifdef __FRC_ROBORIO__
#include <dlfcn.h>
#endif

#include <cstdio>

#include <cameraserver/CameraServerShared.h>
#include <hal/FRCUsageReporting.h>
#include <hal/HALBase.h>
#include <networktables/NetworkTableInstance.h>
#include <wpimath/MathShared.h>

#include "WPILibVersion.h"
#include "frc/DriverStation.h"
#include "frc/RobotState.h"
#include "frc/Utility.h"
#include "frc/WPIErrors.h"
#include "frc/livewindow/LiveWindow.h"
#include "frc/smartdashboard/SmartDashboard.h"

typedef void (*SetCameraServerSharedFP)(frc::CameraServerShared* shared);

using namespace frc;

int frc::RunHALInitialization() {
  if (!HAL_Initialize(500, 0)) {
    wpi::errs() << "FATAL ERROR: HAL could not be initialized\n";
    return -1;
  }
  HAL_Report(HALUsageReporting::kResourceType_Language,
             HALUsageReporting::kLanguage_CPlusPlus, 0, GetWPILibVersion());
  wpi::outs() << "\n********** Robot program starting **********\n";
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
  void SetCameraServerError(const wpi::Twine& error) override {
    wpi_setGlobalWPIErrorWithContext(CameraServerError, error);
  }
  void SetVisionRunnerError(const wpi::Twine& error) override {
    wpi_setGlobalErrorWithContext(-1, error);
  }
  void ReportDriverStationError(const wpi::Twine& error) override {
    DriverStation::ReportError(error);
  }
  std::pair<std::thread::id, bool> GetRobotMainThreadId() const override {
    return std::make_pair(RobotBase::GetThreadId(), true);
  }
};
class WPILibMathShared : public wpi::math::MathShared {
 public:
  void ReportError(const wpi::Twine& error) override {
    DriverStation::ReportError(error);
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
    }
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
    wpi::outs() << "Camera Server Library Not Found\n";
    wpi::outs().flush();
    return;
  }
  auto symbol = dlsym(cameraServerLib, "CameraServer_SetCameraServerShared");
  if (symbol) {
    auto setCameraServerShared = (SetCameraServerSharedFP)symbol;
    setCameraServerShared(new WPILibCameraServerShared{});
  } else {
    wpi::outs() << "Camera Server Shared Symbol Missing\n";
    wpi::outs().flush();
  }
#else
  CameraServer_SetCameraServerShared(new WPILibCameraServerShared{});
#endif
#else
  wpi::outs() << "Not loading CameraServerShared\n";
  wpi::outs().flush();
#endif
}

static void SetupMathShared() {
  wpi::math::MathSharedStore::SetMathShared(
      std::make_unique<WPILibMathShared>());
}

bool RobotBase::IsEnabled() const { return m_ds.IsEnabled(); }

bool RobotBase::IsDisabled() const { return m_ds.IsDisabled(); }

bool RobotBase::IsAutonomous() const { return m_ds.IsAutonomous(); }

bool RobotBase::IsAutonomousEnabled() const {
  return m_ds.IsAutonomousEnabled();
}

bool RobotBase::IsOperatorControl() const { return m_ds.IsOperatorControl(); }

bool RobotBase::IsOperatorControlEnabled() const {
  return m_ds.IsOperatorControlEnabled();
}

bool RobotBase::IsTest() const { return m_ds.IsTest(); }

bool RobotBase::IsNewDataAvailable() const { return m_ds.IsNewControlData(); }

std::thread::id RobotBase::GetThreadId() { return m_threadId; }

RobotBase::RobotBase() : m_ds(DriverStation::GetInstance()) {
  m_threadId = std::this_thread::get_id();

  SetupCameraServerShared();
  SetupMathShared();

  auto inst = nt::NetworkTableInstance::GetDefault();
  inst.SetNetworkIdentity("Robot");
#ifdef __FRC_ROBORIO__
  inst.StartServer("/home/lvuser/networktables.ini");
#else
  inst.StartServer();
#endif

  SmartDashboard::init();

  if (IsReal()) {
    std::FILE* file = nullptr;
    file = std::fopen("/tmp/frc_versions/FRC_Lib_Version.ini", "w");

    if (file != nullptr) {
      std::fputs("C++ ", file);
      std::fputs(GetWPILibVersion(), file);
      std::fclose(file);
    }
  }

  // First and one-time initialization
  inst.GetTable("LiveWindow")
      ->GetSubTable(".status")
      ->GetEntry("LW Enabled")
      .SetBoolean(false);

  LiveWindow::GetInstance()->SetEnabled(false);
}

RobotBase::RobotBase(RobotBase&&) noexcept
    : m_ds(DriverStation::GetInstance()) {}

RobotBase::~RobotBase() {}

RobotBase& RobotBase::operator=(RobotBase&&) noexcept { return *this; }
