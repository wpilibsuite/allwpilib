// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.controller.ElevatorFeedforward;
import edu.wpi.first.math.proto.Controller.ProtobufElevatorFeedforward;
import org.junit.jupiter.api.Test;

class ElevatorFeedforwardProtoTest {
  private static final ElevatorFeedforward DATA = new ElevatorFeedforward(1.91, 1.1, 1.1, 0.229);

  @Test
  void testRoundtrip() {
    ProtobufElevatorFeedforward proto = ElevatorFeedforward.proto.createMessage();
    ElevatorFeedforward.proto.pack(proto, DATA);

    ElevatorFeedforward data = ElevatorFeedforward.proto.unpack(proto);
    assertEquals(DATA.ks, data.ks);
    assertEquals(DATA.kg, data.kg);
    assertEquals(DATA.kv, data.kv);
    assertEquals(DATA.ka, data.ka);
  }
}
