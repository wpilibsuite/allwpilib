// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.StructTestBase;
import org.wpilib.math.controller.DifferentialDriveFeedforward;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
class DifferentialDriveFeedforwardStructTest extends StructTestBase<DifferentialDriveFeedforward> {
  DifferentialDriveFeedforwardStructTest() {
    super(
        new DifferentialDriveFeedforward(0.174, 0.229, 4.4, 4.5),
        DifferentialDriveFeedforward.struct);
  }

  @Override
  public void checkEquals(
      DifferentialDriveFeedforward testData, DifferentialDriveFeedforward data) {
    assertEquals(testData.kVLinear, data.kVLinear);
    assertEquals(testData.kALinear, data.kALinear);
    assertEquals(testData.kVAngular, data.kVAngular);
    assertEquals(testData.kAAngular, data.kAAngular);
  }
}
