// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.kinematics.MecanumDriveMotorVoltages;
import edu.wpi.first.wpilibj.StructTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
class MecanumDriveMotorVoltagesStructTest extends StructTestBase<MecanumDriveMotorVoltages> {
  MecanumDriveMotorVoltagesStructTest() {
    super(new MecanumDriveMotorVoltages(1.2, 3.1, 2.5, -0.1), MecanumDriveMotorVoltages.struct);
  }

  @Override
  public void checkEquals(MecanumDriveMotorVoltages testData, MecanumDriveMotorVoltages data) {
    assertEquals(testData.frontLeft, data.frontLeft);
    assertEquals(testData.frontRight, data.frontRight);
    assertEquals(testData.rearLeft, data.rearLeft);
    assertEquals(testData.rearRight, data.rearRight);
  }
}
