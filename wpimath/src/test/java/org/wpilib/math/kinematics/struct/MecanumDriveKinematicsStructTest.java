// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.kinematics.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.kinematics.MecanumDriveKinematics;

class MecanumDriveKinematicsStructTest {
  private static final MecanumDriveKinematics DATA =
      new MecanumDriveKinematics(
          new Translation2d(19.1, 2.2),
          new Translation2d(35.04, 1.91),
          new Translation2d(1.74, 3.504),
          new Translation2d(3.504, 1.91));

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(MecanumDriveKinematics.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    MecanumDriveKinematics.struct.pack(buffer, DATA);
    buffer.rewind();

    MecanumDriveKinematics data = MecanumDriveKinematics.struct.unpack(buffer);
    assertEquals(DATA.getFrontLeft(), data.getFrontLeft());
    assertEquals(DATA.getFrontRight(), data.getFrontRight());
    assertEquals(DATA.getRearLeft(), data.getRearLeft());
    assertEquals(DATA.getRearRight(), data.getRearRight());
  }
}
