
#include "Internal/HardwareHLReporting.h"
#include "HAL/HAL.hpp"

void HardwareHLReporting::ReportScheduler() {
  HALReport(HALUsageReporting::kResourceType_Command,
            HALUsageReporting::kCommand_Scheduler);
}

void HardwareHLReporting::ReportSmartDashboard() {
  HALReport(HALUsageReporting::kResourceType_SmartDashboard, 0);
}
