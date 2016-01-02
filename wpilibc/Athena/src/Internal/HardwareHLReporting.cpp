/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Internal/HardwareHLReporting.h"
#include "HAL/HAL.hpp"

void HardwareHLReporting::ReportScheduler() {
  HALReport(HALUsageReporting::kResourceType_Command,
            HALUsageReporting::kCommand_Scheduler);
}

void HardwareHLReporting::ReportSmartDashboard() {
  HALReport(HALUsageReporting::kResourceType_SmartDashboard, 0);
}
