// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import io.avaje.jsonb.Jsonb;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;

class SampleJsonTest {
  private final Jsonb jsonb = Jsonb.instance();

  @Test
  void testBaseSample(@TempDir Path tempDir) throws IOException {
    HolonomicTrajectory trajectory =
        new HolonomicTrajectory(
            TrajectoryGeneratorTest.getTrajectory(new ArrayList<>()).getSamples().stream()
                .map(s -> new TrajectorySample(s.timestamp, s.pose, s.velocity, s.acceleration))
                .toArray(TrajectorySample[]::new));

    int index = 0;
    for (TrajectorySample sample : trajectory.samples) {
      Path tempFile = tempDir.resolve("base_sample_" + index + ".json");

      jsonb.toJson(sample, Files.newOutputStream(tempFile));
      TrajectorySample deserializedSample =
          jsonb.type(TrajectorySample.class).fromJson(Files.newInputStream(tempFile));

      assertEquals(sample.timestamp, deserializedSample.timestamp, 1e-9);
      assertEquals(sample.pose, deserializedSample.pose);
      assertEquals(sample.velocity.vx, deserializedSample.velocity.vx, 1e-9);
      assertEquals(sample.velocity.vy, deserializedSample.velocity.vy, 1e-9);
      assertEquals(sample.velocity.omega, deserializedSample.velocity.omega, 1e-9);
      assertEquals(sample.acceleration.ax, deserializedSample.acceleration.ax, 1e-9);
      assertEquals(sample.acceleration.ay, deserializedSample.acceleration.ay, 1e-9);
      assertEquals(sample.acceleration.alpha, deserializedSample.acceleration.alpha, 1e-9);

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

      jsonb.toJson(sample, Files.newOutputStream(tempFile));
      DifferentialSample deserializedSample =
          jsonb.type(DifferentialSample.class).fromJson(Files.newInputStream(tempFile));

      assertAll(
          () -> assertEquals(sample.timestamp, deserializedSample.timestamp, 1e-9),
          () -> assertEquals(sample.pose, deserializedSample.pose),
          () -> assertEquals(sample.velocity.vx, deserializedSample.velocity.vx, 1e-9),
          () -> assertEquals(sample.velocity.vy, deserializedSample.velocity.vy, 1e-9),
          () -> assertEquals(sample.velocity.omega, deserializedSample.velocity.omega, 1e-9),
          () -> assertEquals(sample.acceleration.ax, deserializedSample.acceleration.ax, 1e-9),
          () -> assertEquals(sample.acceleration.ay, deserializedSample.acceleration.ay, 1e-9),
          () ->
              assertEquals(sample.acceleration.alpha, deserializedSample.acceleration.alpha, 1e-9),
          () -> assertEquals(sample.leftSpeed, deserializedSample.leftSpeed, 1e-9),
          () -> assertEquals(sample.rightSpeed, deserializedSample.rightSpeed, 1e-9));

      index++;
    }
  }
}
