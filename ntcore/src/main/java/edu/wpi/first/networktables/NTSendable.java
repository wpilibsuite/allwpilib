// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;

/** Interface for NetworkTable Sendable objects. */
public interface NTSendable extends Sendable {
  /**
   * Initializes this {@link Sendable} object.
   *
   * @param builder sendable builder
   */
  void initSendable(NTSendableBuilder builder);

  @Override
  default void initSendable(SendableBuilder builder) {
    if (builder.getBackendKind() == SendableBuilder.BackendKind.kNetworkTables) {
      initSendable((NTSendableBuilder) builder);
    }
  }
}
