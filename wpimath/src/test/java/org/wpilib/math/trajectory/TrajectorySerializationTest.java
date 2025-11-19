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
import java.util.Arrays;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.kinematics.DifferentialDriveKinematics;
import org.wpilib.math.kinematics.MecanumDriveKinematics;
import org.wpilib.math.kinematics.SwerveDriveKinematics;

class TrajectorySerializationTest {
  private final Translation2d m_fl = new Translation2d(12, 12);
  private final Translation2d m_fr = new Translation2d(12, -12);
  private final Translation2d m_bl = new Translation2d(-12, 12);
  private final Translation2d m_br = new Translation2d(-12, -12);

  private final ObjectMapper mapper = new ObjectMapper();
  private final ObjectWriter writer = mapper.writerWithDefaultPrettyPrinter();

  <SampleType extends TrajectorySample<SampleType>> void assertTrajectoryEquals(
      Trajectory<SampleType> expected, Trajectory<SampleType> actual) {
    assertEquals(expected.duration, actual.duration);
    assertArrayEquals(expected.samples, actual.samples);
  }

  @Test
  void testJsonSerialization(@TempDir Path tempDir) throws IOException {
    Path tempFile = tempDir.resolve("test.json");

    Trajectory<SplineSample> generatedTrajectory =
        TrajectoryGeneratorTest.getTrajectory(new ArrayList<>());
    TrajectorySample.Base[] samples =
        Arrays.stream(generatedTrajectory.samples)
            .map(TrajectorySample.Base::new)
            .toArray(TrajectorySample.Base[]::new);
    TrajectoryBase trajectory = new TrajectoryBase(samples);

    writer.writeValue(Files.newOutputStream(tempFile), trajectory);
    TrajectoryBase deserializedTrajectory =
        mapper.readValue(tempFile.toFile(), TrajectoryBase.class);

    assertTrajectoryEquals(trajectory, deserializedTrajectory);
  }

  @Test
  void testDifferentialSerialization(@TempDir Path tempDir) throws IOException {
    Path tempFile = tempDir.resolve("test.json");

    DifferentialTrajectory trajectory =
        TrajectoryGeneratorTest.getTrajectory(new ArrayList<>())
            .toDifferentialTrajectory(new DifferentialDriveKinematics(12.0));

    writer.writeValue(Files.newOutputStream(tempFile), trajectory);
    DifferentialTrajectory deserializedTrajectory =
        mapper.readValue(tempFile.toFile(), DifferentialTrajectory.class);

    assertTrajectoryEquals(trajectory, deserializedTrajectory);
  }

  @Test
  void testMecanumSerialization(@TempDir Path tempDir) throws IOException {
    Path tempFile = tempDir.resolve("test.json");

    MecanumTrajectory trajectory =
        TrajectoryGeneratorTest.getTrajectory(new ArrayList<>())
            .toMecanumTrajectory(new MecanumDriveKinematics(m_fl, m_fr, m_bl, m_br));

    writer.writeValue(Files.newOutputStream(tempFile), trajectory);
    MecanumTrajectory deserializedTrajectory =
        mapper.readValue(tempFile.toFile(), MecanumTrajectory.class);

    assertTrajectoryEquals(trajectory, deserializedTrajectory);
  }

  @Test
  void testSwerveSerialization(@TempDir Path tempDir) throws IOException {
    Path tempFile = tempDir.resolve("test.json");

    SwerveTrajectory trajectory =
        TrajectoryGeneratorTest.getTrajectory(new ArrayList<>())
            .toSwerveTrajectory(new SwerveDriveKinematics(m_fl, m_fr, m_bl, m_br));

    writer.writeValue(Files.newOutputStream(tempFile), trajectory);
    SwerveTrajectory deserializedTrajectory =
        mapper.readValue(tempFile.toFile(), SwerveTrajectory.class);

    assertTrajectoryEquals(trajectory, deserializedTrajectory);
  }
}
