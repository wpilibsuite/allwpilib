// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertEquals;

import io.avaje.jsonb.Jsonb;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;

class TrajectorySerializationTest {
  // JSON serializes doubles to their shortest decimal form, which can round-trip
  // up to one ULP off, so samples are compared with a small tolerance rather than
  // for exact equality (matching SampleJsonTest).
  private static final double EPSILON = 1e-9;

  <SampleType extends HolonomicSample> void assertTrajectoryEquals(
      Trajectory<SampleType> expected, Trajectory<SampleType> actual) {
    assertEquals(expected.duration, actual.duration, EPSILON);
    assertEquals(expected.samples.size(), actual.samples.size());
    for (int i = 0; i < expected.samples.size(); i++) {
      HolonomicSample e = expected.samples.get(i);
      HolonomicSample a = actual.samples.get(i);
      assertEquals(e.time, a.time, EPSILON);
      assertEquals(e.pose.getX(), a.pose.getX(), EPSILON);
      assertEquals(e.pose.getY(), a.pose.getY(), EPSILON);
      assertEquals(e.pose.getRotation().getRadians(), a.pose.getRotation().getRadians(), EPSILON);
      assertEquals(e.velocity.vx, a.velocity.vx, EPSILON);
      assertEquals(e.velocity.vy, a.velocity.vy, EPSILON);
      assertEquals(e.velocity.omega, a.velocity.omega, EPSILON);
      assertEquals(e.acceleration.ax, a.acceleration.ax, EPSILON);
      assertEquals(e.acceleration.ay, a.acceleration.ay, EPSILON);
      assertEquals(e.acceleration.alpha, a.acceleration.alpha, EPSILON);
      if (e instanceof DifferentialSample de && a instanceof DifferentialSample da) {
        assertEquals(de.leftVelocity, da.leftVelocity, EPSILON);
        assertEquals(de.rightVelocity, da.rightVelocity, EPSILON);
      }
    }
  }

  @Test
  void testHolonomicSerialization(@TempDir Path tempDir) throws IOException {
    Path tempFile = tempDir.resolve("test.json");

    HolonomicTrajectory trajectory =
        new HolonomicTrajectory(
            DrivetrainSplineTrajectoryGeneratorTest.getTrajectory(new ArrayList<>())
                .getSamples()
                .stream()
                .map(s -> new HolonomicSample(s.time, s.pose, s.velocity, s.acceleration))
                .toArray(HolonomicSample[]::new));

    try (var os = Files.newOutputStream(tempFile)) {
      Jsonb.instance().type(HolonomicTrajectory.class).toJson(trajectory, os);
    }
    HolonomicTrajectory deserializedTrajectory =
        HolonomicTrajectory.loadFromFile(tempFile.toFile());

    assertTrajectoryEquals(trajectory, deserializedTrajectory);
  }

  @Test
  void testDifferentialSerialization(@TempDir Path tempDir) throws IOException {
    Path tempFile = tempDir.resolve("test.json");

    DifferentialTrajectory trajectory =
        new DifferentialTrajectory(
            new DifferentialDriveKinematics(12.0),
            DrivetrainSplineTrajectoryGeneratorTest.getTrajectory(new ArrayList<>()).samples);

    try (var os = Files.newOutputStream(tempFile)) {
      Jsonb.instance().type(DifferentialTrajectory.class).toJson(trajectory, os);
    }
    DifferentialTrajectory deserializedTrajectory =
        DifferentialTrajectory.loadFromFile(tempFile.toFile());

    assertTrajectoryEquals(trajectory, deserializedTrajectory);
  }
}
