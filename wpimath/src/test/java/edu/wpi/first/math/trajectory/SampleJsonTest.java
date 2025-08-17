package edu.wpi.first.math.trajectory;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import edu.wpi.first.math.kinematics.ChassisAccelerations;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.Test;

public class SampleJsonTest {
  @Test
  public void testBaseSample() {
    ObjectMapper mapper = new ObjectMapper();
    Trajectory<SplineSample> trajectory = TrajectoryGeneratorTest.getTrajectory(new ArrayList<>());
    trajectory.samples.stream()
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
        trajectory.samples.stream().map(TrajectorySample.Base::new).toList();

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
    trajectory.samples.forEach(
        sample -> {
          try {
            String json = mapper.writeValueAsString(sample);
            DifferentialSample deserializedSample =
                mapper.readValue(json, DifferentialSample.class);
            assertAll(
                () -> assertEquals(sample.timestamp, deserializedSample.timestamp),
                () -> assertEquals(sample.pose, deserializedSample.pose),
                () -> assertEquals(sample.vel, deserializedSample.vel),
                () -> assertEquals(sample.accel, deserializedSample.accel),
                () -> assertEquals(sample.leftSpeed, deserializedSample.leftSpeed),
                () -> assertEquals(sample.rightSpeed, deserializedSample.rightSpeed));
          } catch (JsonProcessingException e) {
            throw new RuntimeException(e);
          }
        });
  }
}
