// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import org.wpilib.hardware.hal.HALUtil;
import org.wpilib.networktables.NetworkTablesJNI;
import org.wpilib.util.runtime.CombinedRuntimeLoader;

/** Dev main. */
public final class DevMain {
  /** Main entry point. */
  public static void main(String[] args) {
    System.out.println("Commands V3 DevMain");

    System.out.println(CombinedRuntimeLoader.getPlatformPath());
    System.out.println(NetworkTablesJNI.now());
    System.out.println(HALUtil.getHALRuntimeType());
  }

  private DevMain() {}
}
