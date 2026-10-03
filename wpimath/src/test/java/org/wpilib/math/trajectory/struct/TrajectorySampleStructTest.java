// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.math.trajectory.TrajectorySample;

class TrajectorySampleStructTest {
  private static final TrajectorySample DATA = new TrajectorySample(1.23);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(TrajectorySample.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    TrajectorySample.struct.pack(buffer, DATA);
    buffer.rewind();

    assertEquals(DATA, TrajectorySample.struct.unpack(buffer));
  }
}
