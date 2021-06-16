// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable;

/** The base interface for objects that can be sent over the network. */
public interface Sendable {
  /**
   * Initializes this {@link Sendable} object.
   *
   * @param builder sendable builder
   */
  void initSendable(SendableBuilder builder);
}
