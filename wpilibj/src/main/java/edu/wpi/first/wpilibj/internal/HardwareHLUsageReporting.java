/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.internal;

import edu.wpi.first.wpilibj.HLUsageReporting;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tInstances;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;

public class HardwareHLUsageReporting implements HLUsageReporting.Interface {
  @Override
  public void reportScheduler() {
    HAL.report(tResourceType.kResourceType_Command, tInstances.kCommand_Scheduler);
  }

  @Override
  public void reportPIDController(int num) {
    HAL.report(tResourceType.kResourceType_PIDController, num);
  }

  @Override
  public void reportSmartDashboard() {
    HAL.report(tResourceType.kResourceType_SmartDashboard, 0);
  }
}
