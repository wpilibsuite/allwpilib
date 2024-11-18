// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.math.kinematics.MecanumDriveMotorVoltages;
import org.wpilib.math.proto.Kinematics.ProtobufMecanumDriveMotorVoltages;
import org.wpilib.wpilibj.ProtoTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
class MecanumDriveMotorVoltagesProtoTest
    extends ProtoTestBase<MecanumDriveMotorVoltages, ProtobufMecanumDriveMotorVoltages> {
  MecanumDriveMotorVoltagesProtoTest() {
    super(new MecanumDriveMotorVoltages(1.2, 3.1, 2.5, -0.1), MecanumDriveMotorVoltages.proto);
  }

  @Override
  public void checkEquals(MecanumDriveMotorVoltages testData, MecanumDriveMotorVoltages data) {
    assertEquals(testData.frontLeftVoltage, data.frontLeftVoltage);
    assertEquals(testData.frontRightVoltage, data.frontRightVoltage);
    assertEquals(testData.rearLeftVoltage, data.rearLeftVoltage);
    assertEquals(testData.rearRightVoltage, data.rearRightVoltage);
  }
}
