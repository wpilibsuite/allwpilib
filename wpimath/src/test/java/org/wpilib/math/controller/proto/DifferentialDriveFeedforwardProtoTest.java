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
    assertEquals(testData.V_LINEAR, data.V_LINEAR);
    assertEquals(testData.A_LINEAR, data.A_LINEAR);
    assertEquals(testData.V_ANGULAR, data.V_ANGULAR);
    assertEquals(testData.A_ANGULAR, data.A_ANGULAR);
  }
}
