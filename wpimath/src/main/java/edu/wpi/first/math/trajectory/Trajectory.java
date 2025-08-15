package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Transform2d;
import edu.wpi.first.math.interpolation.InterpolatingTreeMap;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.units.measure.Time;
import java.util.ArrayList;
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
  public Trajectory<SampleType> transformBy(Transform2d transform) {
    var newFirstSample = samples.getFirst().transform(transform);

    return new Trajectory<>(
        samples.stream()
            .map(
                sample ->
                    sample.fromSample(
                        new TrajectorySample.Base(
                            sample.timestamp,
                            newFirstSample.pose.plus(sample.pose.minus(samples.getFirst().pose)),
                            sample.vel,
                            sample.accel)))
            .toList());
  }

  /**
   * Concatenates this trajectory with another trajectory. If the other trajectory is empty, this
   * trajectory is returned. To work correctly, the other trajectory should start at the end of this
   * trajectory.
   *
   * @param other the other trajectory to concatenate with this one.
   * @return a new trajectory that is the concatenation of this trajectory and the other trajectory.
   */
  public Trajectory<SampleType> concatenate(Trajectory<SampleType> other) {
    if (other.samples.isEmpty()) {
      return this;
    }

    var combinedSamples = new ArrayList<>(this.samples);
    combinedSamples.addAll(other.samples.stream().map(
      s ->
        s.fromSample(
            new TrajectorySample.Base(
                s.timestamp.plus(this.duration), s.pose, s.vel, s.accel))
    ).toList());

    return new Trajectory<>(combinedSamples);
  }

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
  public Trajectory<SampleType> relativeTo(Pose2d other) {
    return new Trajectory<>(
        samples.stream()
            .map(
                s ->
                    s.fromSample(
                        new TrajectorySample.Base(
                            s.timestamp, s.pose.relativeTo(other), s.vel, s.accel)))
            .toList());
  }

  /**
   * Converts this trajectory to a differential trajectory, allowing for easier following by
   * differential drives.
   *
   * @param kinematics DifferentialDriveKinematics object representing the drivetrain.
   */
  public Trajectory<DifferentialSample> toDifferentialTrajectory(
      DifferentialDriveKinematics kinematics) {
    return new Trajectory<>(
        samples.stream().map(s -> new DifferentialSample(s, kinematics)).toList());
  }
}
