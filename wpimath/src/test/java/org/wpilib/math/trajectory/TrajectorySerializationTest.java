// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
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
  <SampleType extends TrajectorySample> void assertTrajectoryEquals(
      Trajectory<SampleType> expected, Trajectory<SampleType> actual) {
    assertEquals(expected.duration, actual.duration);
    assertArrayEquals(expected.samples, actual.samples);
  }

  @Test
  void testHolonomicSerialization(@TempDir Path tempDir) throws IOException {
    Path tempFile = tempDir.resolve("test.json");

    HolonomicTrajectory trajectory =
        new HolonomicTrajectory(
            TrajectoryGeneratorTest.getTrajectory(new ArrayList<>()).getSamples().stream()
                .map(s -> new TrajectorySample(s.timestamp, s.pose, s.velocity, s.acceleration))
                .toArray(TrajectorySample[]::new));

    try (var os = Files.newOutputStream(tempFile)) {
      HolonomicTrajectory.jsonbAdapter().toJson(Jsonb.instance().writer(os), trajectory);
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
      DifferentialTrajectory.jsonbAdapter().toJson(Jsonb.instance().writer(os), trajectory);
    }
    DifferentialTrajectory deserializedTrajectory =
        DifferentialTrajectory.loadFromFile(tempFile.toFile());

    assertTrajectoryEquals(trajectory, deserializedTrajectory);
  }
}
