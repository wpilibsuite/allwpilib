// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.kinematics.SwerveModulePosition;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class SwerveModulePositionStructTest {
  private static final SwerveModulePosition DATA =
      new SwerveModulePosition(3.504, new Rotation2d(17.4));

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(SwerveModulePosition.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    SwerveModulePosition.struct.pack(buffer, DATA);
    buffer.rewind();

    SwerveModulePosition data = SwerveModulePosition.struct.unpack(buffer);
    assertEquals(DATA.distance, data.distance);
    assertEquals(DATA.angle, data.angle);
  }
}
