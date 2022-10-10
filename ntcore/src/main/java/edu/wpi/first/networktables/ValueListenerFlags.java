// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/**
 * Flag values for use with value listeners.
 *
 * <p>The flags are a bitmask and must be OR'ed together to indicate the combination of events
 * desired to be received.
 *
 * <p>By default, notifications are only generated for remote changes occurring after the listener
 * is created. The constants kImmediate and kLocal are modifiers that cause notifications to be
 * generated at other times.
 */
public enum ValueListenerFlags {
  ; // no enum values

  /**
   * Initial listener addition.
   *
   * <p>Set this flag to receive immediate notification of the current value.
   */
  public static final int kImmediate = 0x01;

  /**
   * Changed locally.
   *
   * <p>Set this flag to receive notification of both local changes and changes coming from remote
   * nodes. By default, notifications are only generated for remote changes.
   */
  public static final int kLocal = 0x02;
}
