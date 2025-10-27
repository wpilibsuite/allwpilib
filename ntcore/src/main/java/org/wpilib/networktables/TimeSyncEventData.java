// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables time sync event data. */
@SuppressWarnings("MemberName")
public final class TimeSyncEventData {
  /**
   * Offset between local time and server time, in microseconds. Add this value to local time to get
   * the estimated equivalent server time.
   */
  public final long serverTimeOffset;

  /** Measured round trip time divided by 2, in microseconds. */
  public final long rtt2;

  /**
   * If serverTimeOffset and RTT are valid. An event with this set to false is sent when the client
   * disconnects.
   */
  public final boolean valid;

  /**
   * Constructor. This should generally only be used internally to NetworkTables.
   *
   * @param serverTimeOffset Server time offset
   * @param rtt2 Round trip time divided by 2
   * @param valid If other parameters are valid
   */
  public TimeSyncEventData(long serverTimeOffset, long rtt2, boolean valid) {
    this.serverTimeOffset = serverTimeOffset;
    this.rtt2 = rtt2;
    this.valid = valid;
  }
}
