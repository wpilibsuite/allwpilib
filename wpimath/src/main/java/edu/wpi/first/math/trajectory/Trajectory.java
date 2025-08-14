package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.interpolation.InterpolatingTreeMap;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import edu.wpi.first.units.measure.Time;
import java.util.Comparator;
import java.util.List;

/**
 * Represents a trajectory consisting of a list of {@link TrajectorySample}s, kinematically
 * interpolating between them.
 */
public class Trajectory<SampleType extends TrajectorySample<SampleType>> {

  /** The samples this Trajectory is composed of. */
  public final List<SampleType> samples;

  private final InterpolatingTreeMap<Time, SampleType> sampleMap;

  /** The total duration of the trajectory. */
  public final Time duration;

  /**
   * Constructs a Trajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  public Trajectory(List<SampleType> samples) {
    this.samples =
        samples.stream().sorted(Comparator.comparingDouble(s -> s.timestamp.in(Seconds))).toList();

    this.sampleMap =
        new InterpolatingTreeMap<>(
            (start, end, q) ->
                MathUtil.inverseInterpolate(start.in(Seconds), end.in(Seconds), q.in(Seconds)),
            SampleType::interpolate);

    this.samples.forEach(
        sample -> {
          this.sampleMap.put(sample.timestamp, sample);
        });

    this.duration = this.samples.getLast().timestamp;
  }

  /**
   * Gets the first sample in the trajectory.
   */
  public SampleType start() {
    return sampleAt(0.0);
  }

  /**
   * Gets the last sample in the trajectory.
   */
  public SampleType end() {
    return sampleAt(duration);
  }

  /**
   * Gets the sample at the given timestamp.
   */
  public SampleType sampleAt(Time timestamp) {
    return sampleMap.get(timestamp);
  }

  /**
   * Gets the sample at the given timestamp.
   * @param timestamp timestamp in seconds
   */
  public SampleType sampleAt(double timestamp) {
    return sampleAt(Seconds.of(timestamp));
  }

  /**
   * Converts this trajectory to a differential trajectory, allowing for easier following by differential drives.
   * @param kinematics DifferentialDriveKinematics object representing the drivetrain.
   */
  public Trajectory<DifferentialSample> toDifferentialTrajectory(DifferentialDriveKinematics kinematics) {
    return new Trajectory<>(samples.stream().map(s -> new DifferentialSample(s, kinematics)).toList());
  }

  /**
   * Converts this trajectory to a mecanum trajectory, allowing for easier following by mecanum drives.
   * @param kinematics MecanumDriveKinematics object representing the drivetrain.
   */
  public Trajectory<MecanumSample> toMecanumTrajectory(MecanumDriveKinematics kinematics) {
    return new Trajectory<>(samples.stream().map(s -> new MecanumSample(s, kinematics)).toList());
  }

  /**
   * Converts this trajectory to a swerve trajectory, allowing for easier following by swerve drives.
   * @param kinematics SwerveDriveKinematics object representing the drivetrain.
   */
  public Trajectory<SwerveSample> toSwerveTrajectory(SwerveDriveKinematics kinematics) {
    return new Trajectory<>(samples.stream().map(s -> new SwerveSample(s, kinematics)).toList());
  }

}
