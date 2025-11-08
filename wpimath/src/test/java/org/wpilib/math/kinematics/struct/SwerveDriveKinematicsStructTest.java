// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;

import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.kinematics.SwerveDriveKinematics;
import org.wpilib.StructTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
class SwerveDriveKinematicsStructTest extends StructTestBase<SwerveDriveKinematics> {
  SwerveDriveKinematicsStructTest() {
    super(
        new SwerveDriveKinematics(
            new Translation2d(1.0, 2.1),
            new Translation2d(1.5, -0.9),
            new Translation2d(-1.8, 1.2),
            new Translation2d(-1.7, -1.3)),
        SwerveDriveKinematics.getStruct(4));
  }

  @Override
  public void checkEquals(SwerveDriveKinematics testData, SwerveDriveKinematics data) {
    assertArrayEquals(testData.getModules(), data.getModules());
  }
}
