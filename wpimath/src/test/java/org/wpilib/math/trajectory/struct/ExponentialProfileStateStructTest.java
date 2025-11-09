// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.math.trajectory.ExponentialProfile;

class ExponentialProfileStateStructTest {
  private static final ExponentialProfile.State STATE = new ExponentialProfile.State(4.0, 5.0);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(ExponentialProfile.State.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    ExponentialProfile.State.struct.pack(buffer, STATE);
    buffer.rewind();
    assertEquals(STATE, ExponentialProfile.State.struct.unpack(buffer));
  }
}
