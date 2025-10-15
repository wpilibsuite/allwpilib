// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class DifferentialDriveKinematicsStructTest {
  private static final DifferentialDriveKinematics DATA = new DifferentialDriveKinematics(1.74);

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(DifferentialDriveKinematics.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    DifferentialDriveKinematics.struct.pack(buffer, DATA);
    buffer.rewind();

    DifferentialDriveKinematics data = DifferentialDriveKinematics.struct.unpack(buffer);
    assertEquals(DATA.trackwidth, data.trackwidth);
  }
}
