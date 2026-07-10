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
  private static final double kEpsilon = 1e-9;

  <SampleType extends HolonomicSample> void assertTrajectoryEquals(
      Trajectory<SampleType> expected, Trajectory<SampleType> actual) {
    assertEquals(expected.duration, actual.duration, kEpsilon);
    assertEquals(expected.samples.size(), actual.samples.size());
    for (int i = 0; i < expected.samples.size(); i++) {
      HolonomicSample e = expected.samples.get(i);
      HolonomicSample a = actual.samples.get(i);
      assertEquals(e.time, a.time, kEpsilon);
      assertEquals(e.pose.getX(), a.pose.getX(), kEpsilon);
      assertEquals(e.pose.getY(), a.pose.getY(), kEpsilon);
      assertEquals(e.pose.getRotation().getRadians(), a.pose.getRotation().getRadians(), kEpsilon);
      assertEquals(e.velocity.vx, a.velocity.vx, kEpsilon);
      assertEquals(e.velocity.vy, a.velocity.vy, kEpsilon);
      assertEquals(e.velocity.omega, a.velocity.omega, kEpsilon);
      assertEquals(e.acceleration.ax, a.acceleration.ax, kEpsilon);
      assertEquals(e.acceleration.ay, a.acceleration.ay, kEpsilon);
      assertEquals(e.acceleration.alpha, a.acceleration.alpha, kEpsilon);
      if (e instanceof DifferentialSample de && a instanceof DifferentialSample da) {
        assertEquals(de.leftVelocity, da.leftVelocity, kEpsilon);
        assertEquals(de.rightVelocity, da.rightVelocity, kEpsilon);
      }
    }
  }

  @Test
  void testHolonomicSerialization(@TempDir Path tempDir) throws IOException {
    Path tempFile = tempDir.resolve("test.json");

    HolonomicTrajectory trajectory =
        new HolonomicTrajectory(
            TrajectoryGeneratorTest.getTrajectory(new ArrayList<>()).getSamples().stream()
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
            TrajectoryGeneratorTest.getTrajectory(new ArrayList<>()).samples);

    try (var os = Files.newOutputStream(tempFile)) {
      Jsonb.instance().type(DifferentialTrajectory.class).toJson(trajectory, os);
    }
    DifferentialTrajectory deserializedTrajectory =
        DifferentialTrajectory.loadFromFile(tempFile.toFile());

    assertTrajectoryEquals(trajectory, deserializedTrajectory);
  }
}
