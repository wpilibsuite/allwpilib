// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.ObjectWriter;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

class SampleJsonTest {
  private final Translation2d m_fl = new Translation2d(12, 12);
  private final Translation2d m_fr = new Translation2d(12, -12);
  private final Translation2d m_bl = new Translation2d(-12, 12);
  private final Translation2d m_br = new Translation2d(-12, -12);

  private final ObjectMapper mapper = new ObjectMapper();
  private final ObjectWriter writer = mapper.writerWithDefaultPrettyPrinter();

  @Test
  void testBaseSample(@TempDir Path tempDir) throws IOException {
    Trajectory<SplineSample> trajectory = TrajectoryGeneratorTest.getTrajectory(new ArrayList<>());

    int index = 0;
    for (SplineSample splineSample : trajectory.samples) {
      TrajectorySample.Base sample = new TrajectorySample.Base(splineSample);
      Path tempFile = tempDir.resolve("base_sample_" + index + ".json");

      writer.writeValue(Files.newOutputStream(tempFile), sample);
      TrajectorySample.Base deserializedSample =
          mapper.readValue(tempFile.toFile(), TrajectorySample.Base.class);

      assertEquals(sample.timestamp, deserializedSample.timestamp);
      assertEquals(sample.pose, deserializedSample.pose);
      assertEquals(sample.velocity, deserializedSample.velocity);
      assertEquals(sample.acceleration, deserializedSample.acceleration);

      index++;
    }
  }

  @Test
  void testFromJson(@TempDir Path tempDir) throws IOException {
    Trajectory<SplineSample> trajectory = TrajectoryGeneratorTest.getTrajectory(new ArrayList<>());
    List<TrajectorySample.Base> samples =
        Arrays.stream(trajectory.samples).map(TrajectorySample.Base::new).toList();

    int index = 0;
    for (TrajectorySample.Base sample : samples) {
      Path tempFile = tempDir.resolve("from_json_" + index + ".json");

      writer.writeValue(Files.newOutputStream(tempFile), sample);
      TrajectorySample.Base deserializedSample =
          mapper.readValue(tempFile.toFile(), TrajectorySample.Base.class);

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
        TrajectoryGeneratorTest.getTrajectory(new ArrayList<>())
            .toDifferentialTrajectory(new DifferentialDriveKinematics(0.5));

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

  @Test
  void testMecanumSample(@TempDir Path tempDir) throws IOException {
    Trajectory<MecanumSample> trajectory =
        TrajectoryGeneratorTest.getTrajectory(new ArrayList<>())
            .toMecanumTrajectory(new MecanumDriveKinematics(m_fl, m_fr, m_bl, m_br));

    int index = 0;
    for (MecanumSample sample : trajectory.samples) {
      Path tempFile = tempDir.resolve("mecanum_sample_" + index + ".json");

      writer.writeValue(Files.newOutputStream(tempFile), sample);
      MecanumSample deserializedSample = mapper.readValue(tempFile.toFile(), MecanumSample.class);

      assertAll(
          () -> assertEquals(sample.timestamp, deserializedSample.timestamp),
          () -> assertEquals(sample.pose, deserializedSample.pose),
          () -> assertEquals(sample.velocity, deserializedSample.velocity),
          () -> assertEquals(sample.acceleration, deserializedSample.acceleration),
          () -> assertEquals(sample.speeds.frontLeft, deserializedSample.speeds.frontLeft),
          () -> assertEquals(sample.speeds.frontRight, deserializedSample.speeds.frontRight),
          () -> assertEquals(sample.speeds.rearLeft, deserializedSample.speeds.rearLeft),
          () -> assertEquals(sample.speeds.rearRight, deserializedSample.speeds.rearRight));

      index++;
    }
  }

  @Test
  void testSwerveSample(@TempDir Path tempDir) throws IOException {
    Trajectory<SwerveSample> trajectory =
        TrajectoryGeneratorTest.getTrajectory(new ArrayList<>())
            .toSwerveTrajectory(new SwerveDriveKinematics(m_fl, m_fr, m_bl, m_br));

    int index = 0;
    for (SwerveSample sample : trajectory.samples) {
      Path tempFile = tempDir.resolve("swerve_sample_" + index + ".json");

      writer.writeValue(Files.newOutputStream(tempFile), sample);
      SwerveSample deserializedSample = mapper.readValue(tempFile.toFile(), SwerveSample.class);

      assertAll(
          () -> assertEquals(sample.timestamp, deserializedSample.timestamp),
          () -> assertEquals(sample.pose, deserializedSample.pose),
          () -> assertEquals(sample.velocity, deserializedSample.velocity),
          () -> assertEquals(sample.acceleration, deserializedSample.acceleration),
          () -> assertArrayEquals(sample.states, deserializedSample.states));

      index++;
    }
  }
}
