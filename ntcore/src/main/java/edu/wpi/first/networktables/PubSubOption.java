// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables publish/subscribe option. */
public class PubSubOption {
  private static final int kPeriodic = 1;
  private static final int kSendAll = 2;
  private static final int kTopicsOnly = 3;
  private static final int kPollStorage = 4;
  private static final int kKeepDuplicates = 5;

  PubSubOption(int type, double value) {
    m_type = type;
    m_value = value;
  }

  /**
   * How frequently changes will be sent over the network. NetworkTables may send more frequently
   * than this (e.g. use a combined minimum period for all values) or apply a restricted range to
   * this value. The default if unspecified (and the immediate flag is false) is 100 ms. This option
   * and the immediate option override each other.
   *
   * @param period time between updates, in seconds
   * @return option
   */
  public static PubSubOption periodic(double period) {
    return new PubSubOption(kPeriodic, period);
  }

  /**
   * If enabled, sends all value changes over the network even if only sent periodically. This
   * option defaults to disabled.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  public static PubSubOption sendAll(boolean enabled) {
    return new PubSubOption(kSendAll, enabled ? 1.0 : 0.0);
  }

  /**
   * If enabled, no value changes are sent over the network. This option defaults to disabled.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  public static PubSubOption topicsOnly(boolean enabled) {
    return new PubSubOption(kTopicsOnly, enabled ? 1.0 : 0.0);
  }

  /**
   * If enabled, preserves duplicate value changes (rather than ignoring them). This option defaults
   * to disabled.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  public static PubSubOption keepDuplicates(boolean enabled) {
    return new PubSubOption(kKeepDuplicates, enabled ? 1.0 : 0.0);
  }

  /**
   * Polling storage for subscription. Specifies the maximum number of updates NetworkTables should
   * store between calls to the subscriber's poll() function. Defaults to 1 if sendAll is false, 20
   * if sendAll is true.
   *
   * @param depth number of entries to save for polling.
   * @return option
   */
  public static PubSubOption pollStorage(int depth) {
    return new PubSubOption(kPollStorage, depth);
  }

  final int m_type;
  final double m_value;
}
