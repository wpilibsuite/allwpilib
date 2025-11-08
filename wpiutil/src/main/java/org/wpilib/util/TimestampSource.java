// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

/**
 * Options for where the timestamp an {@link RawFrame} was captured at can be measured relative to.
 */
public enum TimestampSource {
  /** unknown. */
  kUnknown(0),
  /**
   * wpi::Now when the new frame was dequeued by CSCore. Does not account for camera exposure time
   * or V4L latency.
   */
  kFrameDequeue(1),
  /** End of Frame. Same as V4L2_BUF_FLAG_TSTAMP_SRC_EOF, translated into wpi::Now's timebase. */
  kV4LEOF(2),
  /**
   * Start of Exposure. Same as V4L2_BUF_FLAG_TSTAMP_SRC_SOE, translated into wpi::Now's timebase.
   */
  kV4LSOE(3);

  private final int value;

  TimestampSource(int value) {
    this.value = value;
  }

  /**
   * Gets the integer value of the pixel format.
   *
   * @return Integer value
   */
  public int getValue() {
    return value;
  }

  private static final TimestampSource[] s_values = values();

  /**
   * Gets a TimestampSource enum value from its integer value.
   *
   * @param timestampSource integer value
   * @return Enum value
   */
  public static TimestampSource getFromInt(int timestampSource) {
    return s_values[timestampSource];
  }
}
