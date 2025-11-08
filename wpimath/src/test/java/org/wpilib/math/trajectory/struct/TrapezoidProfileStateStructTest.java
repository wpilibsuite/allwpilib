// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.math.trajectory.TrapezoidProfile;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class TrapezoidProfileStateStructTest {
  private static final TrapezoidProfile.State STATE = new TrapezoidProfile.State(4.0, 5.0);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(TrapezoidProfile.State.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    TrapezoidProfile.State.struct.pack(buffer, STATE);
    buffer.rewind();
    assertEquals(STATE, TrapezoidProfile.State.struct.unpack(buffer));
  }
}
