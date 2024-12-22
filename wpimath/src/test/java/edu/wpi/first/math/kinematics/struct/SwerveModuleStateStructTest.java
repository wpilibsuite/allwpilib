// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.kinematics.SwerveModuleState;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class SwerveModuleStateStructTest {
  private static final SwerveModuleState DATA = new SwerveModuleState(22.9, new Rotation2d(3.3));

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(SwerveModuleState.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    SwerveModuleState.struct.pack(buffer, DATA);
    buffer.rewind();

    SwerveModuleState data = SwerveModuleState.struct.unpack(buffer);
    assertEquals(DATA.speed, data.speed);
    assertEquals(DATA.angle, data.angle);
  }
}
