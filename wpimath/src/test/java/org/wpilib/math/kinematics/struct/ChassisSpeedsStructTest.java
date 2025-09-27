// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.math.kinematics.ChassisSpeeds;

class ChassisSpeedsStructTest {
  private static final ChassisSpeeds DATA = new ChassisSpeeds(2.29, 2.2, 0.3504);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(ChassisSpeeds.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    ChassisSpeeds.struct.pack(buffer, DATA);
    buffer.rewind();

    ChassisSpeeds data = ChassisSpeeds.struct.unpack(buffer);
    assertEquals(DATA.vx, data.vx);
    assertEquals(DATA.vy, data.vy);
    assertEquals(DATA.omega, data.omega);
  }
}
