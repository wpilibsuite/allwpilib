// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.controller.ArmFeedforward;
import edu.wpi.first.math.proto.Controller.ProtobufArmFeedforward;
import org.junit.jupiter.api.Test;

class ArmFeedforwardProtoTest {
  private static final ArmFeedforward DATA = new ArmFeedforward(0.174, 0.229, 4.4, 4.4);

  @Test
  void testRoundtrip() {
    ProtobufArmFeedforward proto = ArmFeedforward.proto.createMessage();
    ArmFeedforward.proto.pack(proto, DATA);

    ArmFeedforward data = ArmFeedforward.proto.unpack(proto);
    assertEquals(DATA.getKs(), data.getKs());
    assertEquals(DATA.getKg(), data.getKg());
    assertEquals(DATA.getKv(), data.getKv());
    assertEquals(DATA.getKa(), data.getKa());
  }
}
