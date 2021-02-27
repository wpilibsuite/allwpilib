// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.communication;

public class NIRioStatus {
  public static final int kRioStatusOffset = -63000;

  public static final int kRioStatusSuccess = 0;
  public static final int kRIOStatusBufferInvalidSize = kRioStatusOffset - 80;
  public static final int kRIOStatusOperationTimedOut = -52007;
  public static final int kRIOStatusFeatureNotSupported = kRioStatusOffset - 193;
  public static final int kRIOStatusResourceNotInitialized = -52010;
}
