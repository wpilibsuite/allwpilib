/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil;

public final class DevMain {
  /**
   * Main entry point.
   */
  public static void main(String[] args) {
    System.out.println("Hello World!");
    System.out.println(RuntimeDetector.getPlatformPath());
  }

  private DevMain() {
  }
}
