// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory.proto;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.kinematics.ChassisAccelerations;
import org.wpilib.math.kinematics.ChassisVelocities;
import org.wpilib.math.trajectory.DifferentialSample;
import org.wpilib.math.trajectory.DifferentialTrajectory;

class DifferentialTrajectoryProtoTest {
  private static final DifferentialTrajectory DATA =
      new DifferentialTrajectory(
          List.of(
              new DifferentialSample(
                  0.0,
                  Pose2d.kZero,
                  new ChassisVelocities(1.0, 0.0, 0.0),
                  new ChassisAccelerations(0.5, 0.0, 0.0),
                  0.9,
                  1.1),
              new DifferentialSample(
                  1.0,
                  new Pose2d(1.1, 2.2, Rotation2d.fromDegrees(30)),
                  new ChassisVelocities(1.5, 0.0, 0.3),
                  new ChassisAccelerations(0.1, 0.0, 0.4),
                  1.4,
                  1.6)));

  @Test
  void testRoundtrip() {
    var proto = DifferentialTrajectory.proto.createMessage();
    DifferentialTrajectory.proto.pack(proto, DATA);

    DifferentialTrajectory data = DifferentialTrajectory.proto.unpack(proto);
    assertEquals(DATA.getSamples(), data.getSamples());
  }
}
