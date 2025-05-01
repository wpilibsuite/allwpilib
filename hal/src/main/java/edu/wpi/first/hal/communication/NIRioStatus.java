// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.communication;

/** NI RIO status. */
public class NIRioStatus {
  /** RIO status offset. */
  public static final int RIO_STATUS_OFFSET = -63000;

  /** Success. */
  public static final int RIO_STATUS_SUCCESS = 0;

  /** Buffer invalid size. */
  public static final int RIO_STATUS_BUFFER_INVALID_SIZE = RIO_STATUS_OFFSET - 80;

  /** Operation timed out. */
  public static final int RIO_STATUS_OPERATION_TIMED_OUT = -52007;

  /** Feature not supported. */
  public static final int RIO_STATUS_FEATURE_NOT_SUPPORTED = RIO_STATUS_OFFSET - 193;

  /** Resource not initialized. */
  public static final int RIO_STATUS_RESOURCE_NOT_INITIALIZED = -52010;

  /** Default constructor. */
  public NIRioStatus() {}
}
