// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

/** Exception used to reject collection telemetry logging without an explicit element type. */
public class CollectionElementTypeRequiredException extends Exception {
  private static final long serialVersionUID = 1L;

  /** Constructs a new exception. */
  public CollectionElementTypeRequiredException() {
    super("collection element type must be specified");
  }
}
