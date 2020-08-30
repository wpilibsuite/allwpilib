/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/controller/ProfiledPIDController.h"

#include <hal/FRCUsageReporting.h>

void frc::detail::ReportProfiledPIDController() {
  static int instances = 0;
  ++instances;
  HAL_Report(HALUsageReporting::kResourceType_ProfiledPIDController, instances);
}
