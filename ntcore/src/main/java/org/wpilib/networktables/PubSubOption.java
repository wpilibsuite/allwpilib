// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

/** NetworkTables publish/subscribe option. */
public sealed interface PubSubOption {
  /**
   * How frequently changes will be sent over the network. NetworkTables may send more frequently
   * than this (e.g. use a combined minimum period for all values) or apply a restricted range to
   * this value. The default if unspecified is 100 ms.
   *
   * @param period time between updates, in seconds
   */
  record Periodic(double period) implements PubSubOption {}

  /**
   * If enabled, sends all value changes over the network. This option defaults to disabled.
   *
   * @param enabled True to enable, false to disable
   */
  record SendAll(boolean enabled) implements PubSubOption {}

  /**
   * If enabled on a subscription, does not request value changes. This option defaults to disabled.
   *
   * @param enabled True to enable, false to disable
   */
  record TopicsOnly(boolean enabled) implements PubSubOption {}

  /**
   * If enabled, preserves duplicate value changes (rather than ignoring them). This option defaults
   * to disabled.
   *
   * @param enabled True to enable, false to disable
   */
  record KeepDuplicates(boolean enabled) implements PubSubOption {}

  /**
   * Polling storage for subscription. Specifies the maximum number of updates NetworkTables should
   * store between calls to the subscriber's readQueue() function. Defaults to 1 if sendAll is
   * false, 20 if sendAll is true.
   *
   * @param depth number of entries to save for polling.
   */
  record PollStorage(int depth) implements PubSubOption {}

  /**
   * For subscriptions, specify whether remote value updates should not be queued for readQueue().
   * See also disableLocal(). Defaults to false (remote value updates are queued).
   *
   * @param disabled True to disable, false to enable
   */
  record DisableRemote(boolean disabled) implements PubSubOption {}

  /**
   * For subscriptions, specify whether local value updates should not be queued for readQueue().
   * See also disableRemote(). Defaults to false (local value updates are queued).
   *
   * @param disabled True to disable, false to enable
   */
  record DisableLocal(boolean disabled) implements PubSubOption {}

  /**
   * Don't queue value updates for the given publisher. Only has an effect on subscriptions. Only
   * one exclusion may be set.
   *
   * @param publisher publisher to exclude
   */
  record ExcludePublisher(Publisher publisher) implements PubSubOption {}

  /**
   * Don't queue value updates for the given publisher. Only has an effect on subscriptions. Only
   * one exclusion may be set.
   *
   * @param publisher publisher handle to exclude
   */
  record ExcludePublisherHandle(int publisher) implements PubSubOption {}

  /**
   * Don't queue value updates for the internal publisher for an entry. Only has an effect on
   * entries.
   *
   * @param enabled True to enable, false to disable
   */
  record ExcludeSelf(boolean enabled) implements PubSubOption {}

  /**
   * For subscriptions, don't share the existence of the subscription with the network. Note this
   * means updates will not be received from the network unless another subscription overlaps with
   * this one, and the subscription will not appear in metatopics.
   *
   * @param enabled True to enable, false to disable
   */
  record Hidden(boolean enabled) implements PubSubOption {}

  /** Indicates only value changes will be sent over the network (default). */
  PubSubOption SEND_CHANGES = new SendAll(false);

  /** Indicates all value changes will be sent over the network. */
  PubSubOption SEND_ALL = new SendAll(true);

  /** Indicates both topics and values will be sent over the network (default). */
  PubSubOption TOPICS_AND_VALUES = new TopicsOnly(false);

  /** Indicates only topics will be sent over the network. */
  PubSubOption TOPICS_ONLY = new TopicsOnly(true);

  /** Indicates duplicate value changes will be ignored (default). */
  PubSubOption IGNORE_DUPLICATES = new KeepDuplicates(false);

  /** Indicates duplicate value changes will be preserved. */
  PubSubOption KEEP_DUPLICATES = new KeepDuplicates(true);

  /** For subscriptions, indicates remote value updates will be queued for readQueue() (default). */
  PubSubOption ENABLE_REMOTE = new DisableRemote(false);

  /** For subscriptions, indicates remote value updates will not be queued for readQueue() . */
  PubSubOption DISABLE_REMOTE = new DisableRemote(true);

  /** For subscriptions, indicates local value updates will be queued for readQueue() (default). */
  PubSubOption ENABLE_LOCAL = new DisableLocal(false);

  /** For subscriptions, indicates local value updates will not be queued for readQueue() . */
  PubSubOption DISABLE_LOCAL = new DisableLocal(true);

  /**
   * For entries, indicates value updates from the internal publisher will be included (default).
   */
  PubSubOption INCLUDE_SELF = new ExcludeSelf(false);

  /** For entries, indicates value updates from the internal publisher will be excluded. */
  PubSubOption EXCLUDE_SELF = new ExcludeSelf(true);

  /** For subscriptions, indicates the subscription is visible to the network (default). */
  PubSubOption VISIBLE = new Hidden(false);

  /** For subscriptions, indicates the subscription is hidden from the network. */
  PubSubOption HIDDEN = new Hidden(true);

  /**
   * How frequently changes will be sent over the network. NetworkTables may send more frequently
   * than this (e.g. use a combined minimum period for all values) or apply a restricted range to
   * this value. The default if unspecified is 100 ms.
   *
   * @param period time between updates, in seconds
   * @return option
   */
  static PubSubOption periodic(double period) {
    return new Periodic(period);
  }

  /**
   * Polling storage for subscription. Specifies the maximum number of updates NetworkTables should
   * store between calls to the subscriber's readQueue() function. Defaults to 1 if sendAll is
   * false, 20 if sendAll is true.
   *
   * @param depth number of entries to save for polling.
   * @return option
   */
  static PubSubOption pollStorage(int depth) {
    return new PollStorage(depth);
  }

  /**
   * Don't queue value updates for the given publisher. Only has an effect on subscriptions. Only
   * one exclusion may be set.
   *
   * @param publisher publisher handle to exclude
   * @return option
   */
  static PubSubOption excludePublisher(int publisher) {
    return new ExcludePublisherHandle(publisher);
  }

  /**
   * Don't queue value updates for the given publisher. Only has an effect on subscriptions. Only
   * one exclusion may be set.
   *
   * @param publisher publisher to exclude
   * @return option
   */
  static PubSubOption excludePublisher(Publisher publisher) {
    return new ExcludePublisher(publisher);
  }
}
