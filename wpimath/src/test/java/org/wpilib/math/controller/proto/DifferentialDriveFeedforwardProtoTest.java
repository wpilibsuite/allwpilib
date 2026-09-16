// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.ProtoTestBase;
import org.wpilib.math.controller.DifferentialDriveFeedforward;
import org.wpilib.math.controller.proto.detail.ProtobufDifferentialDriveFeedforward;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
class DifferentialDriveFeedforwardProtoTest
    extends ProtoTestBase<DifferentialDriveFeedforward, ProtobufDifferentialDriveFeedforward> {
  DifferentialDriveFeedforwardProtoTest() {
    super(
        new DifferentialDriveFeedforward(0.174, 0.229, 4.4, 4.5),
        DifferentialDriveFeedforward.proto);
  }

  @Override
  public void checkEquals(
      DifferentialDriveFeedforward testData, DifferentialDriveFeedforward data) {
    assertEquals(testData.kvLinear, data.kvLinear);
    assertEquals(testData.kaLinear, data.kaLinear);
    assertEquals(testData.kvAngular, data.kvAngular);
    assertEquals(testData.kaAngular, data.kaAngular);
  }
}
