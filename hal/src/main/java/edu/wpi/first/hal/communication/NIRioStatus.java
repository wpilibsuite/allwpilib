/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.communication;

public class NIRioStatus {
  public static final int kRioStatusOffset = -63000;

  public static final int kRioStatusSuccess = 0;
  public static final int kRIOStatusBufferInvalidSize = kRioStatusOffset - 80;
  public static final int kRIOStatusOperationTimedOut = -52007;
  public static final int kRIOStatusFeatureNotSupported = kRioStatusOffset - 193;
  public static final int kRIOStatusResourceNotInitialized = -52010;
}
