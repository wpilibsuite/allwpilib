// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.ntcore;

import org.wpilib.networktables.NetworkTablesJNI;
import org.wpilib.util.runtime.CombinedRuntimeLoader;

public final class DevMain {
  /** Main method. */
  public static void main(String[] args) {
    System.out.println("Hello World!");
    System.out.println(CombinedRuntimeLoader.getPlatformPath());
    NetworkTablesJNI.flush(NetworkTablesJNI.getDefaultInstance());
  }

  private DevMain() {}
}
