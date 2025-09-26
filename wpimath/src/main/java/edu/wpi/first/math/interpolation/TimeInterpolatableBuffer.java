// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

import edu.wpi.first.math.MathUtil;
import java.util.NavigableMap;
import java.util.Optional;
import java.util.TreeMap;

/**
 * The TimeInterpolatableBuffer provides an easy way to estimate past measurements. One application
 * might be in conjunction with the DifferentialDrivePoseEstimator, where knowledge of the robot
 * pose at the time when vision or other global measurement were recorded is necessary, or for
 * recording the past angles of mechanisms as measured by encoders.
 *
 * @param <T> The type stored in this buffer.
 */
public final class TimeInterpolatableBuffer<T> {
  private final double m_historySize;
  private final Interpolator<T> m_interpolatingFunc;
  private final NavigableMap<Double, T> m_pastSnapshots = new TreeMap<>();

  /**
   * Constructs a TimeInterpolatableBuffer.
   *
   * @param interpolateFunction Interpolation function.
   * @param historySize The history size of the buffer in seconds.
   */
  private TimeInterpolatableBuffer(Interpolator<T> interpolateFunction, double historySize) {
    this.m_historySize = historySize;
    this.m_interpolatingFunc = interpolateFunction;
  }

  /**
   * Create a new TimeInterpolatableBuffer.
   *
   * @param interpolateFunction The function used to interpolate between values.
   * @param historySize The history size of the buffer in seconds.
   * @param <T> The type of data to store in the buffer.
   * @return The new TimeInterpolatableBuffer.
   */
  public static <T> TimeInterpolatableBuffer<T> createBuffer(
      Interpolator<T> interpolateFunction, double historySize) {
    return new TimeInterpolatableBuffer<>(interpolateFunction, historySize);
  }

  /**
   * Create a new TimeInterpolatableBuffer that stores a given subclass of {@link Interpolatable}.
   *
   * @param historySize The history size of the buffer in seconds.
   * @param <T> The type of {@link Interpolatable} to store in the buffer.
   * @return The new TimeInterpolatableBuffer.
   */
  public static <T extends Interpolatable<T>> TimeInterpolatableBuffer<T> createBuffer(
      double historySize) {
    return new TimeInterpolatableBuffer<>(Interpolatable::interpolate, historySize);
  }

  /**
   * Create a new TimeInterpolatableBuffer to store Double values.
   *
   * @param historySize The history size of the buffer in seconds.
   * @return The new TimeInterpolatableBuffer.
   */
  public static TimeInterpolatableBuffer<Double> createDoubleBuffer(double historySize) {
    return new TimeInterpolatableBuffer<>(MathUtil::lerp, historySize);
  }

  /**
   * Add a sample to the buffer.
   *
   * @param time The timestamp of the sample in seconds.
   * @param sample The sample object.
   */
  public void addSample(double time, T sample) {
    cleanUp(time);
    m_pastSnapshots.put(time, sample);
  }

  /**
   * Removes samples older than our current history size.
   *
   * @param time The current timestamp in seconds.
   */
  private void cleanUp(double time) {
    while (!m_pastSnapshots.isEmpty()) {
      var entry = m_pastSnapshots.firstEntry();
      if (time - entry.getKey() >= m_historySize) {
        m_pastSnapshots.remove(entry.getKey());
      } else {
        return;
      }
    }
  }

  /** Clear all old samples. */
  public void clear() {
    m_pastSnapshots.clear();
  }

  /**
   * Sample the buffer at the given time. If the buffer is empty, an empty Optional is returned.
   *
   * @param time The time at which to sample in seconds.
   * @return The interpolated value at that timestamp or an empty Optional.
   */
  public Optional<T> getSample(double time) {
    if (m_pastSnapshots.isEmpty()) {
      return Optional.empty();
    }

    // Special case for when the requested time is the same as a sample
    var nowEntry = m_pastSnapshots.get(time);
    if (nowEntry != null) {
      return Optional.of(nowEntry);
    }

    var topBound = m_pastSnapshots.ceilingEntry(time);
    var bottomBound = m_pastSnapshots.floorEntry(time);

    // Return null if neither sample exists, and the opposite bound if the other is null
    if (topBound == null && bottomBound == null) {
      return Optional.empty();
    } else if (topBound == null) {
      return Optional.of(bottomBound.getValue());
    } else if (bottomBound == null) {
      return Optional.of(topBound.getValue());
    } else {
      // Otherwise, interpolate. Because T is between [0, 1], we want the ratio of (the difference
      // between the current time and bottom bound) and (the difference between top and bottom
      // bounds).
      return Optional.of(
          m_interpolatingFunc.interpolate(
              bottomBound.getValue(),
              topBound.getValue(),
              (time - bottomBound.getKey()) / (topBound.getKey() - bottomBound.getKey())));
    }
  }

  /**
   * Grant access to the internal sample buffer. Used in Pose Estimation to replay odometry inputs
   * stored within this buffer.
   *
   * @return The internal sample buffer.
   */
  public NavigableMap<Double, T> getInternalBuffer() {
    return m_pastSnapshots;
  }
}
