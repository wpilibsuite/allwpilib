// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertAll;
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

class SampleJsonTest {
  private final ObjectMapper mapper = new ObjectMapper();
  private final ObjectWriter writer = mapper.writerWithDefaultPrettyPrinter();

  @Test
  void testBaseSample(@TempDir Path tempDir) throws IOException {
    TrajectoryBase trajectory = new TrajectoryBase(TrajectoryGeneratorTest.getTrajectory(new ArrayList<>()).samples);

    int index = 0;
    for (TrajectorySample sample : trajectory.samples) {
      Path tempFile = tempDir.resolve("base_sample_" + index + ".json");

      writer.writeValue(Files.newOutputStream(tempFile), sample);
      TrajectorySample deserializedSample =
          mapper.readValue(tempFile.toFile(), TrajectorySample.class);

      assertEquals(sample.timestamp, deserializedSample.timestamp);
      assertEquals(sample.pose, deserializedSample.pose);
      assertEquals(sample.velocity, deserializedSample.velocity);
      assertEquals(sample.acceleration, deserializedSample.acceleration);

      index++;
    }
  }

  @Test
  void testFromJson(@TempDir Path tempDir) throws IOException {
    TrajectoryBase trajectory = new TrajectoryBase(TrajectoryGeneratorTest.getTrajectory(new ArrayList<>()).samples);

    int index = 0;
    for (TrajectorySample sample : trajectory.samples) {
      Path tempFile = tempDir.resolve("from_json_" + index + ".json");

      writer.writeValue(Files.newOutputStream(tempFile), sample);
      TrajectorySample deserializedSample =
          mapper.readValue(tempFile.toFile(), TrajectorySample.class);

      assertEquals(sample.timestamp, deserializedSample.timestamp);
      assertEquals(sample.pose, deserializedSample.pose);
      assertEquals(sample.velocity, deserializedSample.velocity);
      assertEquals(sample.acceleration, deserializedSample.acceleration);

      index++;
    }
  }

  @Test
  void testDifferentialSamples(@TempDir Path tempDir) throws IOException {
    Trajectory<DifferentialSample> trajectory =
        new DifferentialTrajectory(
            new DifferentialDriveKinematics(0.5),
            TrajectoryGeneratorTest.getTrajectory(new ArrayList<>()).samples);

    int index = 0;
    for (DifferentialSample sample : trajectory.samples) {
      Path tempFile = tempDir.resolve("differential_sample_" + index + ".json");

      writer.writeValue(Files.newOutputStream(tempFile), sample);
      DifferentialSample deserializedSample =
          mapper.readValue(tempFile.toFile(), DifferentialSample.class);

      assertAll(
          () -> assertEquals(sample.timestamp, deserializedSample.timestamp),
          () -> assertEquals(sample.pose, deserializedSample.pose),
          () -> assertEquals(sample.velocity, deserializedSample.velocity),
          () -> assertEquals(sample.acceleration, deserializedSample.acceleration),
          () -> assertEquals(sample.leftSpeed, deserializedSample.leftSpeed),
          () -> assertEquals(sample.rightSpeed, deserializedSample.rightSpeed));

      index++;
    }
  }
}
