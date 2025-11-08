// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.controller.ArmFeedforward;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class ArmFeedforwardStructTest {
  private static final ArmFeedforward DATA = new ArmFeedforward(0.174, 0.229, 4.4, 4.4);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(ArmFeedforward.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    ArmFeedforward.struct.pack(buffer, DATA);
    buffer.rewind();

    ArmFeedforward data = ArmFeedforward.struct.unpack(buffer);
    assertEquals(DATA.getKs(), data.getKs());
    assertEquals(DATA.getKg(), data.getKg());
    assertEquals(DATA.getKv(), data.getKv());
    assertEquals(DATA.getKa(), data.getKa());
  }
}
