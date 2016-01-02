/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.internal;

import edu.wpi.first.wpilibj.HLUsageReporting;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tInstances;
import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tResourceType;
import edu.wpi.first.wpilibj.communication.UsageReporting;

public class HardwareHLUsageReporting implements HLUsageReporting.Interface {
  @Override
  public void reportScheduler() {
    UsageReporting.report(tResourceType.kResourceType_Command, tInstances.kCommand_Scheduler);
  }

  @Override
  public void reportPIDController(int num) {
    UsageReporting.report(tResourceType.kResourceType_PIDController, num);
  }

  @Override
  public void reportSmartDashboard() {
    UsageReporting.report(tResourceType.kResourceType_SmartDashboard, 0);
  }
}
