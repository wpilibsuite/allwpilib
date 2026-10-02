// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.trajectory.DifferentialSample;

class DifferentialSampleProtoTest {
  private static final DifferentialSample DATA =
      new DifferentialSample(
          1.5,
          new Pose2d(1.1, 2.2, Rotation2d.fromDegrees(30)),
          new ChassisVelocities(1.0, 2.0, 3.0),
          new ChassisAccelerations(4.0, 5.0, 6.0),
          1.23,
          4.56);

  @Test
  void testRoundtrip() {
    var proto = DifferentialSample.proto.createMessage();
    DifferentialSample.proto.pack(proto, DATA);

    DifferentialSample data = DifferentialSample.proto.unpack(proto);
    assertEquals(DATA, data);
  }
}
