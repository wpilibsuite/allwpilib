// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.apriltag;

public final class DevMain {
  /** Main entry point. */
  public static void main(String[] args) {
    System.out.println("Hello World!");
    AprilTagDetector detector = new AprilTagDetector();
    detector.addFamily("tag16h5");
    AprilTagDetector.Config config = new AprilTagDetector.Config();
    config.refineEdges = false;
    detector.setConfig(config);
    detector.close();
  }

  private DevMain() {}
}
