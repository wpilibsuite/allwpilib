// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import org.junit.jupiter.api.Test;

class TimeInterpolatableBufferTest {
  @Test
  void testInterpolation() {
    TimeInterpolatableBuffer<Rotation2d> buffer = TimeInterpolatableBuffer.createBuffer(10);

    buffer.addSample(0, new Rotation2d());
    assertEquals(0, buffer.getSample(0).get().getRadians(), 0.001);
    buffer.addSample(1, new Rotation2d(1));
    assertEquals(0.5, buffer.getSample(0.5).get().getRadians(), 0.001);
    assertEquals(1.0, buffer.getSample(1.0).get().getRadians(), 0.001);
    buffer.addSample(3, new Rotation2d(2));
    assertEquals(1.5, buffer.getSample(2).get().getRadians(), 0.001);

    buffer.addSample(10.5, new Rotation2d(2));
    assertEquals(new Rotation2d(1), buffer.getSample(0).get());
  }

  @Test
  void testPose2d() {
    TimeInterpolatableBuffer<Pose2d> buffer = TimeInterpolatableBuffer.createBuffer(10);

    // We expect to be at (1 - 1/Math.sqrt(2), 1/Math.sqrt(2), 45deg) at t=0.5
    buffer.addSample(0, new Pose2d(0, 0, Rotation2d.fromDegrees(90)));
    buffer.addSample(1, new Pose2d(1, 1, Rotation2d.fromDegrees(0)));
    Pose2d sample = buffer.getSample(0.5).get();

    assertEquals(1 - 1 / Math.sqrt(2), sample.getTranslation().getX(), 0.01);
    assertEquals(1 / Math.sqrt(2), sample.getTranslation().getY(), 0.01);
    assertEquals(45, sample.getRotation().getDegrees(), 0.01);
  }
}
