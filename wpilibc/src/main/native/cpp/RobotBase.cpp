/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/RobotBase.h"

#include <cstdio>

#include <cameraserver/CameraServerShared.h>
#include <cscore.h>
#include <hal/HAL.h>
#include <networktables/NetworkTableInstance.h>

#include "WPILibVersion.h"
#include "frc/DriverStation.h"
#include "frc/RobotState.h"
#include "frc/Utility.h"
#include "frc/WPIErrors.h"
#include "frc/livewindow/LiveWindow.h"
#include "frc/smartdashboard/SmartDashboard.h"

using namespace frc;

int frc::RunHALInitialization() {
  if (!HAL_Initialize(500, 0)) {
    wpi::errs() << "FATAL ERROR: HAL could not be initialized\n";
    return -1;
  }
  HAL_Report(HALUsageReporting::kResourceType_Language,
             HALUsageReporting::kLanguage_CPlusPlus);
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
}  // namespace

static void SetupCameraServerShared() {
  SetCameraServerShared(std::make_unique<WPILibCameraServerShared>());
}

bool RobotBase::IsEnabled() const { return m_ds.IsEnabled(); }

bool RobotBase::IsDisabled() const { return m_ds.IsDisabled(); }

bool RobotBase::IsAutonomous() const { return m_ds.IsAutonomous(); }

bool RobotBase::IsOperatorControl() const { return m_ds.IsOperatorControl(); }

bool RobotBase::IsTest() const { return m_ds.IsTest(); }

bool RobotBase::IsNewDataAvailable() const { return m_ds.IsNewControlData(); }

std::thread::id RobotBase::GetThreadId() { return m_threadId; }

RobotBase::RobotBase() : m_ds(DriverStation::GetInstance()) {
  if (!HAL_Initialize(500, 0)) {
    wpi::errs() << "FATAL ERROR: HAL could not be initialized\n";
    wpi::errs().flush();
    std::terminate();
  }
  m_threadId = std::this_thread::get_id();

  SetupCameraServerShared();

  auto inst = nt::NetworkTableInstance::GetDefault();
  inst.SetNetworkIdentity("Robot");
  inst.StartServer("/home/lvuser/networktables.ini");

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

RobotBase::RobotBase(RobotBase&&) : m_ds(DriverStation::GetInstance()) {}

RobotBase::~RobotBase() { cs::Shutdown(); }

RobotBase& RobotBase::operator=(RobotBase&&) { return *this; }
