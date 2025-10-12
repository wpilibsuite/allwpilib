package edu.wpi.first.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import edu.wpi.first.math.geometry.Translation2d;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import org.junit.jupiter.api.Test;

class SampleJsonTest {
  private final Translation2d m_fl = new Translation2d(12, 12);
  private final Translation2d m_fr = new Translation2d(12, -12);
  private final Translation2d m_bl = new Translation2d(-12, 12);
  private final Translation2d m_br = new Translation2d(-12, -12);

  @Test
  public void testBaseSample() {
    ObjectMapper mapper = new ObjectMapper();
    Trajectory<SplineSample> trajectory = TrajectoryGeneratorTest.getTrajectory(new ArrayList<>());
    Arrays.stream(trajectory.samples)
        .map(TrajectorySample.Base::new)
        .forEach(
            sample -> {
              try {
                String json = mapper.writeValueAsString(sample);
                System.out.println(json);
              } catch (JsonProcessingException e) {
                throw new RuntimeException(e);
              }
            });
  }

  @Test
  public void testFromJson() {
    ObjectMapper mapper = new ObjectMapper();

    Trajectory<SplineSample> trajectory = TrajectoryGeneratorTest.getTrajectory(new ArrayList<>());
    List<TrajectorySample.Base> samples =
        Arrays.stream(trajectory.samples).map(TrajectorySample.Base::new).toList();

    for (TrajectorySample.Base sample : samples) {
      try {
        String json = mapper.writeValueAsString(sample);
        TrajectorySample.Base deserializedSample =
            mapper.readValue(json, TrajectorySample.Base.class);
        assertEquals(sample.timestamp, deserializedSample.timestamp);
      } catch (JsonProcessingException e) {
        throw new RuntimeException(e);
      }
    }
  }

  @Test
  public void testDifferentialSamples() {
    ObjectMapper mapper = new ObjectMapper();
    Trajectory<DifferentialSample> trajectory =
        TrajectoryGeneratorTest.getTrajectory(new ArrayList<>())
            .toDifferentialTrajectory(new DifferentialDriveKinematics(0.5));
    Arrays.stream(trajectory.samples).forEach(
        sample -> {
          try {
            String json = mapper.writeValueAsString(sample);
            DifferentialSample deserializedSample =
                mapper.readValue(json, DifferentialSample.class);
            assertAll(
                () -> assertEquals(sample.timestamp, deserializedSample.timestamp),
                () -> assertEquals(sample.pose, deserializedSample.pose),
                () -> assertEquals(sample.velocity, deserializedSample.velocity),
                () -> assertEquals(sample.acceleration, deserializedSample.acceleration),
                () -> assertEquals(sample.leftSpeed, deserializedSample.leftSpeed),
                () -> assertEquals(sample.rightSpeed, deserializedSample.rightSpeed));
          } catch (JsonProcessingException e) {
            throw new RuntimeException(e);
          }
        });
  }

  @Test
  public void testMecanumSample() {
    ObjectMapper mapper = new ObjectMapper();
    Trajectory<MecanumSample> trajectory =
        TrajectoryGeneratorTest.getTrajectory(new ArrayList<>())
            .toMecanumTrajectory(new MecanumDriveKinematics(m_fl, m_fr, m_bl, m_br));
    Arrays.stream(trajectory.samples).forEach(
        sample -> {
          try {
            String json = mapper.writeValueAsString(sample);
            MecanumSample deserializedSample = mapper.readValue(json, MecanumSample.class);
            assertAll(
                () -> assertEquals(sample.timestamp, deserializedSample.timestamp),
                () -> assertEquals(sample.pose, deserializedSample.pose),
                () -> assertEquals(sample.velocity, deserializedSample.velocity),
                () -> assertEquals(sample.acceleration, deserializedSample.acceleration),
                () -> assertEquals(sample.speeds.frontLeft, deserializedSample.speeds.frontLeft),
                () -> assertEquals(sample.speeds.frontRight, deserializedSample.speeds.frontRight),
                () -> assertEquals(sample.speeds.rearLeft, deserializedSample.speeds.rearLeft),
                () -> assertEquals(sample.speeds.rearRight, deserializedSample.speeds.rearRight));
          } catch (JsonProcessingException e) {
            throw new RuntimeException(e);
          }
        });
  }
}
