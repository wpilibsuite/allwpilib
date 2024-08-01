// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable2;

/** Sendable publish/subscribe option. */
public class SendableOption {
  /** The option kind. */
  public enum Kind {
    kPeriodic,
    kTypeString,
    kSendAll,
    kPollStorage,
    kKeepDuplicates,
    kDisableRemote,
    kDisableLocal
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
   * Type string. Default is used for the data type if this is empty. Not used for raw values (pass
   * the type string directly to the functions).
   */
  public static SendableOption typeString(String typeString) {
    return new SendableOption(Kind.kTypeString, typeString);
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
    return new SendableOption(Kind.kPeriodic, period);
  }

  /**
   * If enabled, sends all value changes over the network. This option defaults to disabled.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  public static SendableOption sendAll(boolean enabled) {
    return new SendableOption(Kind.kSendAll, enabled);
  }

  /**
   * If enabled, preserves duplicate value changes (rather than ignoring them). This option defaults
   * to disabled.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  public static SendableOption keepDuplicates(boolean enabled) {
    return new SendableOption(Kind.kKeepDuplicates, enabled);
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
    return new SendableOption(Kind.kPollStorage, depth);
  }

  /**
   * For subscriptions, specify whether remote value updates should not be queued for readQueue().
   * See also disableLocal(). Defaults to false (remote value updates are queued).
   *
   * @param disabled True to disable, false to enable
   * @return option
   */
  public static SendableOption disableRemote(boolean disabled) {
    return new SendableOption(Kind.kDisableRemote, disabled);
  }

  /**
   * For subscriptions, specify whether local value updates should not be queued for readQueue().
   * See also disableRemote(). Defaults to false (local value updates are queued).
   *
   * @param disabled True to disable, false to enable
   * @return option
   */
  public static SendableOption disableLocal(boolean disabled) {
    return new SendableOption(Kind.kDisableLocal, disabled);
  }

  /**
   * Gets the kind of option.
   *
   * @return option kind
   */
  public Kind getKind() {
    return m_kind;
  }

  /**
   * Gets the stored boolean value. Value is unspecified for non-boolean kinds.
   *
   * @return boolean value
   */
  public boolean getBooleanValue() {
    return m_bValue;
  }

  /**
   * Gets the stored integer value. Value is unspecified for non-integer kinds.
   *
   * @return integer value
   */
  public int getIntValue() {
    return m_iValue;
  }

  /**
   * Gets the stored double value. Value is unspecified for non-double kinds.
   *
   * @return double value
   */
  public double getDoubleValue() {
    return m_dValue;
  }

  /**
   * Gets the stored string value. Value is unspecified for non-string kinds.
   *
   * @return string value
   */
  public String getStringValue() {
    return m_sValue;
  }

  final Kind m_kind;
  final boolean m_bValue;
  final int m_iValue;
  final double m_dValue;
  final String m_sValue;
}
