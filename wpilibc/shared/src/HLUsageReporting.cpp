/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HLUsageReporting.h"

frc::HLUsageReportingInterface* frc::HLUsageReporting::impl = nullptr;

void frc::HLUsageReporting::SetImplementation(HLUsageReportingInterface* i) {
  impl = i;
}

void frc::HLUsageReporting::ReportScheduler() {
  if (impl != nullptr) {
    impl->ReportScheduler();
  }
}

void frc::HLUsageReporting::ReportSmartDashboard() {
  if (impl != nullptr) {
    impl->ReportSmartDashboard();
  }
}
