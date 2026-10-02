// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.net;

import org.wpilib.util.runtime.RuntimeLoader;

public final class DevMain {
  /** Main entry point. */
  public static void main(String[] args) {
    System.out.println("Hello World!");
    System.out.println(RuntimeLoader.getPlatformName());
  }

  private DevMain() {}
}
