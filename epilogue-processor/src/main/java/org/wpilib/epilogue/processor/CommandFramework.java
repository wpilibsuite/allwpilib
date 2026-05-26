// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.processor;

/**
 * Represents the different WPILib command frameworks. Epilogue will detect the presence of these
 * frameworks at compile time to add special handling, such as automatic logging of the scheduler.
 */
public enum CommandFramework {
  /** The Commands V2 framework. */
  V2,

  /** The Commands V3 framework. */
  V3,
}
