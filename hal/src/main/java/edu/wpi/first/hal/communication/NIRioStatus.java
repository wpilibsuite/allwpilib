// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.communication;

/** NI RIO status. */
public class NIRioStatus {
  /** RIO status offset. */
  public static final int kRioStatusOffset = -63000;

  /** Success. */
  public static final int kRioStatusSuccess = 0;

  /** Buffer invalid size. */
  public static final int kRIOStatusBufferInvalidSize = kRioStatusOffset - 80;

  /** Operation timed out. */
  public static final int kRIOStatusOperationTimedOut = -52007;

  /** Feature not supported. */
  public static final int kRIOStatusFeatureNotSupported = kRioStatusOffset - 193;

  /** Resource not initialized. */
  public static final int kRIOStatusResourceNotInitialized = -52010;

  /** Default constructor. */
  public NIRioStatus() {}
}
