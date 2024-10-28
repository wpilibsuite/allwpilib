// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.controller.ElevatorFeedforward;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class ElevatorFeedforwardStructTest {
  private static final ElevatorFeedforward DATA = new ElevatorFeedforward(1.91, 1.1, 1.1, 0.229);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(ElevatorFeedforward.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    ElevatorFeedforward.struct.pack(buffer, DATA);
    buffer.rewind();

    ElevatorFeedforward data = ElevatorFeedforward.struct.unpack(buffer);
    assertEquals(DATA.getKs(), data.getKs());
    assertEquals(DATA.getKg(), data.getKg());
    assertEquals(DATA.getKv(), data.getKv());
    assertEquals(DATA.getKa(), data.getKa());
  }
}
