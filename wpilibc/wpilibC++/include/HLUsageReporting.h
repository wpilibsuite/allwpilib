/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/
#pragma once

class HLUsageReportingInterface {
 public:
  virtual ~HLUsageReportingInterface() = default;
  virtual void ReportScheduler() = 0;
  virtual void ReportSmartDashboard() = 0;
};

class HLUsageReporting {
 private:
  static HLUsageReportingInterface* impl;

 public:
  static void SetImplementation(HLUsageReportingInterface* i);
  static void ReportScheduler();
  static void ReportSmartDashboard();
};
