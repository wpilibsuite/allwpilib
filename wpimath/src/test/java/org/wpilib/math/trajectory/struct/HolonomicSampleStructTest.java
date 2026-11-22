// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.struct;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.trajectory.HolonomicSample;

class HolonomicSampleStructTest {
  private static final HolonomicSample DATA =
      new HolonomicSample(
          1.23,
          new Pose2d(1.1, 2.2, Rotation2d.fromDegrees(33.3)),
          new ChassisVelocities(3.3, 4.4, 5.5),
          new ChassisAccelerations(6.6, 7.7, 8.8));

  @Test
  void testRoundtrip() {
    ByteBuffer buffer = ByteBuffer.allocate(HolonomicSample.struct.getSize());
    buffer.order(ByteOrder.LITTLE_ENDIAN);
    HolonomicSample.struct.pack(buffer, DATA);
    buffer.rewind();

    assertEquals(DATA, HolonomicSample.struct.unpack(buffer));
  }
}
