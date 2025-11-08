// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.controller.DifferentialDriveFeedforward;
import edu.wpi.first.wpilibj.StructTestBase;

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
    assertEquals(testData.m_kVLinear, data.m_kVLinear);
    assertEquals(testData.m_kALinear, data.m_kALinear);
    assertEquals(testData.m_kVAngular, data.m_kVAngular);
    assertEquals(testData.m_kAAngular, data.m_kAAngular);
  }
}
