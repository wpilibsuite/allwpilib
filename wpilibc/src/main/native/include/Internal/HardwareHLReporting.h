/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "HLUsageReporting.h"

namespace frc {

class HardwareHLReporting : public HLUsageReportingInterface {
 public:
  virtual void ReportScheduler();
  virtual void ReportSmartDashboard();
};

}  // namespace frc
