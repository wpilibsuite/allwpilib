// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.trajectory.ExponentialProfile;
import edu.wpi.first.math.trajectory.TrapezoidProfile;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class ProfileStateStructsTest {
  private static final TrapezoidProfile.State TRAP_STATE = new TrapezoidProfile.State(4.0, 5.0);
  private static final ExponentialProfile.State EXP_STATE = new ExponentialProfile.State(4.0, 5.0);

  @Test
  void testRoundtripTrapezoidal() {
    ByteBuffer buffer = ByteBuffer.allocate(TrapezoidProfile.State.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    TrapezoidProfile.State.struct.pack(buffer, TRAP_STATE);
    buffer.rewind();
    assertEquals(TRAP_STATE, TrapezoidProfile.State.struct.unpack(buffer));
  }

  @Test
  void testRoundtripExponential() {
    ByteBuffer buffer = ByteBuffer.allocate(ExponentialProfile.State.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    ExponentialProfile.State.struct.pack(buffer, EXP_STATE);
    buffer.rewind();
    assertEquals(EXP_STATE, ExponentialProfile.State.struct.unpack(buffer));
  }
}
