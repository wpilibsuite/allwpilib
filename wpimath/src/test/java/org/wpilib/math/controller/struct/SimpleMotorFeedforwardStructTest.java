// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.controller.SimpleMotorFeedforward;
import edu.wpi.first.wpilibj.StructTestBase;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
class SimpleMotorFeedforwardStructTest extends StructTestBase<SimpleMotorFeedforward> {
  SimpleMotorFeedforwardStructTest() {
    super(new SimpleMotorFeedforward(0.4, 4.0, 0.7, 0.025), SimpleMotorFeedforward.struct);
  }

  @Override
  public void checkEquals(SimpleMotorFeedforward testData, SimpleMotorFeedforward data) {
    assertEquals(testData.getKs(), data.getKs());
    assertEquals(testData.getKv(), data.getKv());
    assertEquals(testData.getKa(), data.getKa());
    assertEquals(testData.getDt(), data.getDt());
  }
}
