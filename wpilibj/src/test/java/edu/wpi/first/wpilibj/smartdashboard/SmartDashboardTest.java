/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.smartdashboard;

import org.junit.jupiter.api.extension.ExtendWith;

import edu.wpi.first.wpilibj.MockHardwareExtension;
import edu.wpi.first.wpilibj.UtilityClassTest;

@ExtendWith(MockHardwareExtension.class)
class SmartDashboardTest extends UtilityClassTest {
  SmartDashboardTest() {
    super(SmartDashboard.class);
  }
}
