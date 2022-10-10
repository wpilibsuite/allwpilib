// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/**
 * Flag values for use with topic listeners.
 *
 * <p>The flags are a bitmask and must be OR'ed together to indicate the combination of events
 * desired to be received.
 *
 * <p>The constants kPublish, kUnpublish, and kProperties represent different events that can occur
 * to topics.
 */
public enum TopicListenerFlags {
  ; // no enum values

  /**
   * Initial listener addition.
   *
   * <p>Set this flag to receive immediate notification of topics matching the flag criteria
   * (generally only useful when combined with kPublish).
   */
  public static final int kImmediate = 0x01;

  /**
   * Newly published topic.
   *
   * <p>Set this flag to receive a notification when a topic is initially published.
   */
  public static final int kPublish = 0x02;

  /**
   * Topic has no more publishers.
   *
   * <p>Set this flag to receive a notification when a topic has no more publishers.
   */
  public static final int kUnpublish = 0x04;

  /**
   * Topic's properties changed.
   *
   * <p>Set this flag to receive a notification when an topic's properties change.
   */
  public static final int kProperties = 0x08;
}
