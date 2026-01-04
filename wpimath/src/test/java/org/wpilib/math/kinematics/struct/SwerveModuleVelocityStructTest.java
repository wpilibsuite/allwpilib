// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.SwerveModuleVelocity;

class SwerveModuleVelocityStructTest {
  private static final SwerveModuleVelocity DATA =
      new SwerveModuleVelocity(22.9, new Rotation2d(3.3));

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(SwerveModuleVelocity.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    SwerveModuleVelocity.struct.pack(buffer, DATA);
    buffer.rewind();

    SwerveModuleVelocity data = SwerveModuleVelocity.struct.unpack(buffer);
    assertEquals(DATA.velocity, data.velocity);
    assertEquals(DATA.angle, data.angle);
  }
}
