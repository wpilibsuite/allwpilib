// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.ProtoTestBase;
import org.wpilib.math.controller.SimpleMotorFeedforward;
import org.wpilib.math.proto.Controller.ProtobufSimpleMotorFeedforward;

@SuppressWarnings("PMD.TestClassWithoutTestCases")
class SimpleMotorFeedforwardProtoTest
    extends ProtoTestBase<SimpleMotorFeedforward, ProtobufSimpleMotorFeedforward> {
  SimpleMotorFeedforwardProtoTest() {
    super(new SimpleMotorFeedforward(0.4, 4.0, 0.7, 0.025), SimpleMotorFeedforward.proto);
  }

  @Override
  public void checkEquals(SimpleMotorFeedforward testData, SimpleMotorFeedforward data) {
    assertEquals(testData.getKs(), data.getKs());
    assertEquals(testData.getKv(), data.getKv());
    assertEquals(testData.getKa(), data.getKa());
    assertEquals(testData.getDt(), data.getDt());
  }
}
