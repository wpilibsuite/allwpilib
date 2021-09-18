// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.commands;

import edu.wpi.first.hal.HALUtil;
import edu.wpi.first.networktables.NetworkTablesJNI;
import edu.wpi.first.util.RuntimeDetector;

public final class DevMain {
  /** Main entry point. */
  public static void main(String[] args) {
    System.out.println("Hello World!");
    System.out.println(RuntimeDetector.getPlatformPath());
    System.out.println(NetworkTablesJNI.now());
    System.out.println(HALUtil.getHALRuntimeType());
  }

  private DevMain() {}
}
