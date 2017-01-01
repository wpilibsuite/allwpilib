/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Internal/HardwareHLReporting.h"

#include "HAL/HAL.h"

using namespace frc;

void HardwareHLReporting::ReportScheduler() {
  HAL_Report(HALUsageReporting::kResourceType_Command,
             HALUsageReporting::kCommand_Scheduler);
}

void HardwareHLReporting::ReportSmartDashboard() {
  HAL_Report(HALUsageReporting::kResourceType_SmartDashboard, 0);
}
