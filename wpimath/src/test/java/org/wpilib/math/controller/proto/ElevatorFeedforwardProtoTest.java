// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.controller.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.controller.ElevatorFeedforward;
import org.wpilib.math.proto.Controller.ProtobufElevatorFeedforward;

class ElevatorFeedforwardProtoTest {
  private static final ElevatorFeedforward DATA = new ElevatorFeedforward(1.91, 1.1, 1.1, 0.229);

  @Test
  void testRoundtrip() {
    ProtobufElevatorFeedforward proto = ElevatorFeedforward.proto.createMessage();
    ElevatorFeedforward.proto.pack(proto, DATA);

    ElevatorFeedforward data = ElevatorFeedforward.proto.unpack(proto);
    assertEquals(DATA.getKs(), data.getKs());
    assertEquals(DATA.getKg(), data.getKg());
    assertEquals(DATA.getKv(), data.getKv());
    assertEquals(DATA.getKa(), data.getKa());
  }
}
