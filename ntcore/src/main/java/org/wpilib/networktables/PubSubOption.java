// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables publish/subscribe option. */
public class PubSubOption {
  enum Kind {
    periodic,
    sendAll,
    topicsOnly,
    pollStorage,
    keepDuplicates,
    disableRemote,
    disableLocal,
    excludePublisher,
    excludeSelf,
    hidden
  }

  PubSubOption(Kind kind, boolean value) {
    m_kind = kind;
    m_bValue = value;
    m_iValue = 0;
    m_dValue = 0;
  }

  PubSubOption(Kind kind, int value) {
    m_kind = kind;
    m_bValue = false;
    m_iValue = value;
    m_dValue = 0;
  }

  PubSubOption(Kind kind, double value) {
    m_kind = kind;
    m_bValue = false;
    m_iValue = 0;
    m_dValue = value;
  }

  /**
   * How frequently changes will be sent over the network. NetworkTables may send more frequently
   * than this (e.g. use a combined minimum period for all values) or apply a restricted range to
   * this value. The default if unspecified is 100 ms.
   *
   * @param period time between updates, in seconds
   * @return option
   */
  public static PubSubOption periodic(double period) {
    return new PubSubOption(Kind.periodic, period);
  }

  /**
   * If enabled, sends all value changes over the network. This option defaults to disabled.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  public static PubSubOption sendAll(boolean enabled) {
    return new PubSubOption(Kind.sendAll, enabled);
  }

  /**
   * If enabled on a subscription, does not request value changes. This option defaults to disabled.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  public static PubSubOption topicsOnly(boolean enabled) {
    return new PubSubOption(Kind.topicsOnly, enabled);
  }

  /**
   * If enabled, preserves duplicate value changes (rather than ignoring them). This option defaults
   * to disabled.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  public static PubSubOption keepDuplicates(boolean enabled) {
    return new PubSubOption(Kind.keepDuplicates, enabled);
  }

  /**
   * Polling storage for subscription. Specifies the maximum number of updates NetworkTables should
   * store between calls to the subscriber's readQueue() function. Defaults to 1 if sendAll is
   * false, 20 if sendAll is true.
   *
   * @param depth number of entries to save for polling.
   * @return option
   */
  public static PubSubOption pollStorage(int depth) {
    return new PubSubOption(Kind.pollStorage, depth);
  }

  /**
   * For subscriptions, specify whether remote value updates should not be queued for readQueue().
   * See also disableLocal(). Defaults to false (remote value updates are queued).
   *
   * @param disabled True to disable, false to enable
   * @return option
   */
  public static PubSubOption disableRemote(boolean disabled) {
    return new PubSubOption(Kind.disableRemote, disabled);
  }

  /**
   * For subscriptions, specify whether local value updates should not be queued for readQueue().
   * See also disableRemote(). Defaults to false (local value updates are queued).
   *
   * @param disabled True to disable, false to enable
   * @return option
   */
  public static PubSubOption disableLocal(boolean disabled) {
    return new PubSubOption(Kind.disableLocal, disabled);
  }

  /**
   * Don't queue value updates for the given publisher. Only has an effect on subscriptions. Only
   * one exclusion may be set.
   *
   * @param publisher publisher handle to exclude
   * @return option
   */
  public static PubSubOption excludePublisher(int publisher) {
    return new PubSubOption(Kind.excludePublisher, publisher);
  }

  /**
   * Don't queue value updates for the given publisher. Only has an effect on subscriptions. Only
   * one exclusion may be set.
   *
   * @param publisher publisher to exclude
   * @return option
   */
  public static PubSubOption excludePublisher(Publisher publisher) {
    return excludePublisher(publisher != null ? publisher.getHandle() : 0);
  }

  /**
   * Don't queue value updates for the internal publisher for an entry. Only has an effect on
   * entries.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  public static PubSubOption excludeSelf(boolean enabled) {
    return new PubSubOption(Kind.excludeSelf, enabled);
  }

  /**
   * For subscriptions, don't share the existence of the subscription with the network. Note this
   * means updates will not be received from the network unless another subscription overlaps with
   * this one, and the subscription will not appear in metatopics.
   *
   * @param enabled True to enable, false to disable
   * @return option
   */
  public static PubSubOption hidden(boolean enabled) {
    return new PubSubOption(Kind.hidden, enabled);
  }

  final Kind m_kind;
  final boolean m_bValue;
  final int m_iValue;
  final double m_dValue;
}
