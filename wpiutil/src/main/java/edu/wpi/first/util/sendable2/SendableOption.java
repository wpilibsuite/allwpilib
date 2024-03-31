// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable2;

/** Sendable publish/subscribe option. */
public class SendableOption {
  enum Kind {
    periodic,
    typeString,
    sendAll,
    pollStorage,
    keepDuplicates,
    disableRemote,
    disableLocal
  }

  SendableOption(Kind kind, boolean value) {
    m_kind = kind;
    m_bValue = value;
    m_iValue = 0;
    m_dValue = 0;
    m_sValue = null;
  }

  SendableOption(Kind kind, int value) {
    m_kind = kind;
    m_bValue = false;
    m_iValue = value;
    m_dValue = 0;
    m_sValue = null;
  }

  SendableOption(Kind kind, double value) {
    m_kind = kind;
    m_bValue = false;
    m_iValue = 0;
    m_dValue = value;
    m_sValue = null;
  }

  SendableOption(Kind kind, String value) {
    m_kind = kind;
    m_bValue = false;
    m_iValue = 0;
    m_dValue = 0;
    m_sValue = value;
  }

  /**
   * Type string.  Default is used for the data type if this is empty.
   * Not used for raw values (pass the type string directly to the functions).
   */
  public static SendableOption typeString(String typeString) {
    return new SendableOption(Kind.typeString, typeString);
  }

  /**
   * How frequently changes will be sent over the network. NetworkTables may send more frequently
   * than this (e.g. use a combined minimum period for all values) or apply a restricted range to
   * this value. The default if unspecified is 100 ms.
   *
   * @param period time between updates, in seconds
   * @return option
   */
  public static SendableOption periodic(double period) {
    return new SendableOption(Kind.periodic, period);
  }

  /**
   * If enabled, sends all value changes over the network. This option defaults to disabled.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  public static SendableOption sendAll(boolean enabled) {
    return new SendableOption(Kind.sendAll, enabled);
  }

  /**
   * If enabled, preserves duplicate value changes (rather than ignoring them). This option defaults
   * to disabled.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  public static SendableOption keepDuplicates(boolean enabled) {
    return new SendableOption(Kind.keepDuplicates, enabled);
  }

  /**
   * Polling storage for subscription. Specifies the maximum number of updates NetworkTables should
   * store between calls to the subscriber's readQueue() function. Defaults to 1 if sendAll is
   * false, 20 if sendAll is true.
   *
   * @param depth number of entries to save for polling.
   * @return option
   */
  public static SendableOption pollStorage(int depth) {
    return new SendableOption(Kind.pollStorage, depth);
  }

  /**
   * For subscriptions, specify whether remote value updates should not be queued for readQueue().
   * See also disableLocal(). Defaults to false (remote value updates are queued).
   *
   * @param disabled True to disable, false to enable
   * @return option
   */
  public static SendableOption disableRemote(boolean disabled) {
    return new SendableOption(Kind.disableRemote, disabled);
  }

  /**
   * For subscriptions, specify whether local value updates should not be queued for readQueue().
   * See also disableRemote(). Defaults to false (local value updates are queued).
   *
   * @param disabled True to disable, false to enable
   * @return option
   */
  public static SendableOption disableLocal(boolean disabled) {
    return new SendableOption(Kind.disableLocal, disabled);
  }

  final Kind m_kind;
  final boolean m_bValue;
  final int m_iValue;
  final double m_dValue;
  final String m_sValue;
}
