/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.communication;

public class NIRioStatus {
  // TODO: Should this file be auto-generated?
  public static final int kRioStatusOffset = -63000;

  public static final int kRioStatusSuccess = 0;
  public static final int kRIOStatusBufferInvalidSize = kRioStatusOffset - 80;
  public static final int kRIOStatusOperationTimedOut = -52007;
  public static final int kRIOStatusFeatureNotSupported = kRioStatusOffset - 193;
  public static final int kRIOStatusResourceNotInitialized = -52010;
}
