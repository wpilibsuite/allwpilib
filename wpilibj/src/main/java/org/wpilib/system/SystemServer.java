// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import org.wpilib.hardware.hal.SystemServerJNI;
import org.wpilib.networktables.NetworkTableInstance;

/** Class to get system server NT instance. */
public final class SystemServer {
  /**
   * Gets the system server NT Instance.
   *
   * @return NT Instance
   */
  public static NetworkTableInstance getSystemServer() {
    return NetworkTableInstance.fromNativeHandle(SystemServerJNI.getSystemServerHandle());
  }

  /** Utility Class. */
  private SystemServer() {}
}
