/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RobotBase.h"

#include <cstdio>

#include <HAL/HAL.h>
#include <networktables/NetworkTableInstance.h>

#include "CameraServerShared.h"
#include "DriverStation.h"
#include "LiveWindow/LiveWindow.h"
#include "RobotState.h"
#include "SmartDashboard/SmartDashboard.h"
#include "Utility.h"
#include "WPIErrors.h"
#include "WPILibVersion.h"

using namespace frc;

std::thread::id RobotBase::m_threadId;

namespace {
class WPILibCameraServerShared : public frc::CameraServerShared {
 public:
  void ReportUsbCamera(int id) override {
    HAL_Report(HALUsageReporting::kResourceType_PCVideoServer, id);
  }
  void ReportAxisCamera(int id) override {
    HAL_Report(HALUsageReporting::kResourceType_AxisCamera, id);
  }
  void ReportVideoServer(int id) override {
    HAL_Report(HALUsageReporting::kResourceType_PCVideoServer, id);
  }
  void SetCameraServerError(wpi::StringRef error) override {
    wpi_setGlobalWPIErrorWithContext(CameraServerError, error);
  }
  void SetVisionRunnerError(wpi::StringRef error) override {
    wpi_setGlobalErrorWithContext(-1, error);
  }
  void ReportDriverStationError(wpi::StringRef error) override {
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

  std::FILE* file = nullptr;
  file = std::fopen("/tmp/frc_versions/FRC_Lib_Version.ini", "w");

  if (file != nullptr) {
    std::fputs("C++ ", file);
    std::fputs(GetWPILibVersion(), file);
    std::fclose(file);
  }

  // First and one-time initialization
  inst.GetTable("LiveWindow")
      ->GetSubTable(".status")
      ->GetEntry("LW Enabled")
      .SetBoolean(false);

  LiveWindow::GetInstance()->SetEnabled(false);
}
