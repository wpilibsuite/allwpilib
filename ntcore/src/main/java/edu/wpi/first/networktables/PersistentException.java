// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import java.io.IOException;

/**
 * An exception thrown when persistent load/save fails in a {@link NetworkTable}.
 */
public final class PersistentException extends IOException {
  public static final long serialVersionUID = 0;

  public PersistentException(String message) {
    super(message);
  }
}
