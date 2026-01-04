// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;

class TrajectorySerializationTest {
  private final ObjectMapper mapper = new ObjectMapper();
  private final ObjectWriter writer = mapper.writerWithDefaultPrettyPrinter();

  <SampleType extends TrajectorySample> void assertTrajectoryEquals(
      Trajectory<SampleType> expected, Trajectory<SampleType> actual) {
    assertEquals(expected.duration, actual.duration);
    assertArrayEquals(expected.samples, actual.samples);
  }

  @Test
  void testJsonSerialization(@TempDir Path tempDir) throws IOException {
    Path tempFile = tempDir.resolve("test.json");

    Trajectory<SplineSample> generatedTrajectory =
        TrajectoryGeneratorTest.getTrajectory(new ArrayList<>());
    HolonomicTrajectory trajectory = new HolonomicTrajectory(generatedTrajectory.getSamples());

    writer.writeValue(Files.newOutputStream(tempFile), trajectory);
    HolonomicTrajectory deserializedTrajectory =
        mapper.readValue(tempFile.toFile(), HolonomicTrajectory.class);

    assertTrajectoryEquals(trajectory, deserializedTrajectory);
  }

  @Test
  void testDifferentialSerialization(@TempDir Path tempDir) throws IOException {
    Path tempFile = tempDir.resolve("test.json");

    DifferentialTrajectory trajectory =
        new DifferentialTrajectory(
            new DifferentialDriveKinematics(12.0),
            TrajectoryGeneratorTest.getTrajectory(new ArrayList<>()).samples);

    writer.writeValue(Files.newOutputStream(tempFile), trajectory);
    DifferentialTrajectory deserializedTrajectory =
        mapper.readValue(tempFile.toFile(), DifferentialTrajectory.class);

    assertTrajectoryEquals(trajectory, deserializedTrajectory);
  }
}
