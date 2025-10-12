package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.interpolation.InterpolatingTreeMap;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.kinematics.MecanumDriveKinematics;
import edu.wpi.first.math.kinematics.SwerveDriveKinematics;
import edu.wpi.first.units.measure.Time;
import java.util.Arrays;
import java.util.Comparator;

/**
 * Represents a trajectory consisting of a list of {@link TrajectorySample}s, kinematically
 * interpolating between them.
 */
public abstract class Trajectory<SampleType extends TrajectorySample<SampleType>> {

  /** The samples this Trajectory is composed of. */
  public final SampleType[] samples;

  private final InterpolatingTreeMap<Time, SampleType> sampleMap;

  /** The total duration of the trajectory. */
  public final Time duration;

  /**
   * Constructs a Trajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  @SuppressWarnings({"this-escape", "unchecked"})
  public Trajectory(SampleType[] samples) {
    this.samples =
        (SampleType[])
            Arrays.stream(samples)
                .sorted(Comparator.comparingDouble(s -> s.timestamp.in(Seconds)))
                .toArray(TrajectorySample[]::new);

    this.sampleMap =
        new InterpolatingTreeMap<>(
            (start, end, q) ->
                MathUtil.inverseLerp(start.in(Seconds), end.in(Seconds), q.in(Seconds)),
            this::interpolate);

    for (var sample : this.samples) {
      sampleMap.put(sample.timestamp, sample);
    }

    this.duration =
        this.samples.length > 0 ? this.samples[this.samples.length - 1].timestamp : Seconds.of(0.0);
  }

  /**
   * Interpolates between two samples. This method must be implemented by subclasses to provide
   * drivetrain-specific interpolation logic.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  public abstract SampleType interpolate(SampleType start, SampleType end, double t);

  /** Gets the first sample in the trajectory. */
  public SampleType start() {
    return sampleAt(0.0);
  }

  /** Gets the last sample in the trajectory. */
  public SampleType end() {
    return sampleAt(duration);
  }

  /** Gets the sample at the given timestamp. */
  public SampleType sampleAt(Time timestamp) {
    return sampleMap.get(timestamp);
  }

  /**
   * Gets the sample at the given timestamp.
   *
   * @param timestamp timestamp in seconds
   */
  public SampleType sampleAt(double timestamp) {
    return sampleAt(Seconds.of(timestamp));
  }

  /**
   * Transforms all poses in the trajectory by the given transform. This is useful for converting a
   * robot-relative trajectory into a field-relative trajectory. This works with respect to the
   * first pose in the trajectory.
   *
   * @param transform The transform to transform the trajectory by.
   * @return The transformed trajectory.
   */
  public abstract Trajectory<SampleType> transformBy(Transform2d transform);

  /**
   * Concatenates this trajectory with another trajectory. If the other trajectory is empty, this
   * trajectory is returned. To work correctly, the other trajectory should start at the end of this
   * trajectory.
   *
   * @param other the other trajectory to concatenate with this one.
   * @return a new trajectory that is the concatenation of this trajectory and the other trajectory.
   */
  public abstract Trajectory<SampleType> concatenate(Trajectory<SampleType> other);

  /**
   * Returns a new trajectory that is relative to the given pose. This is useful for converting a
   * field-relative trajectory into a robot-relative trajectory. The returned trajectory will have
   * the same timestamps, velocities, and accelerations as the original trajectory, but the poses
   * will be relative to the given pose.
   *
   * @param other the pose to which the trajectory should be relative. This is typically the robot's
   *     starting pose.
   * @return a new trajectory that is relative to the given pose.
   */
  public abstract Trajectory<SampleType> relativeTo(Pose2d other);

  /**
   * Converts this trajectory to a differential trajectory, allowing for easier following by
   * differential drives.
   *
   * @param kinematics DifferentialDriveKinematics object representing the drivetrain.
   * @return the trajectory with differential samples.
   */
  public DifferentialTrajectory toDifferentialTrajectory(DifferentialDriveKinematics kinematics) {
    return new DifferentialTrajectory(kinematics, samples);
  }

  /**
   * Converts this trajectory to a mecanum trajectory, allowing for easier following by mecanum
   * drives.
   *
   * @param kinematics MecanumDriveKinematics object representing the drivetrain.
   * @return the trajectory with mecanum samples.
   */
  public MecanumTrajectory toMecanumTrajectory(MecanumDriveKinematics kinematics) {
    return new MecanumTrajectory(kinematics, samples);
  }

  /**
   * Converts this trajectory to a swerve trajectory, allowing for easier following by swerve
   * drives.
   *
   * @param kinematics SwerveDriveKinematics object representing the drivetrain.
   * @return the trajectory with swerve samples.
   */
  public SwerveTrajectory toSwerveTrajectory(SwerveDriveKinematics kinematics) {
    return new SwerveTrajectory(kinematics, samples);
  }
}
