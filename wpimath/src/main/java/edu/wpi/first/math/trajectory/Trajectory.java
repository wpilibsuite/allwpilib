package edu.wpi.first.math.trajectory;

import static edu.wpi.first.units.Units.Milliseconds;
import static edu.wpi.first.units.Units.Seconds;

import edu.wpi.first.math.MathUtil;
import edu.wpi.first.math.interpolation.InterpolatingTreeMap;
import edu.wpi.first.math.interpolation.InverseInterpolator;
import edu.wpi.first.units.measure.Time;
import java.util.Comparator;
import java.util.List;

/**
 * Represents a trajectory consisting of a list of {@link TrajectorySample}s, kinematically
 * interpolating between them.
 */
public class Trajectory {
  private final InterpolatingTreeMap<Time, TrajectorySample> samples;
  private final Time duration;

  private static final InverseInterpolator<Time> timeInverseInterpolator =
      (start, end, q) ->
          MathUtil.inverseInterpolate(start.in(Seconds), end.in(Seconds), q.in(Seconds));

  /**
   * Constructs a Trajectory.
   *
   * @param samples the samples of the trajectory. Order does not matter as they will be ordered
   *     internally.
   */
  public Trajectory(List<TrajectorySample> samples) {
    this.samples =
        new InterpolatingTreeMap<>(timeInverseInterpolator, TrajectorySample::interpolate);

    samples.forEach(
        sample -> {
          this.samples.put(sample.timestamp, sample);
        });

    this.duration =
        samples.stream()
            .sorted(Comparator.comparingDouble(s -> s.timestamp.in(Milliseconds)))
            .toList()
            .get(samples.size() - 1)
            .timestamp;
  }

  public Time duration() {
    return duration;
  }

  public TrajectorySample sampleAt(Time timestamp) {
    return samples.get(timestamp);
  }

  public TrajectorySample sampleAt(double timestamp) {
    return sampleAt(Seconds.of(timestamp));
  }
}
