// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.quickvision;

import org.wpilib.opmode.TimedRobot;
import org.wpilib.vision.stream.CameraServer;

/**
 * Uses the CameraServer class to automatically capture video from a USB webcam and send it to the
 * FRC dashboard without doing any vision processing. This is the easiest way to get camera images
 * to the dashboard. Just add this to the robot class constructor.
 */
public class Robot extends TimedRobot {
  public Robot() {
    CameraServer.startAutomaticCapture();
  }
}
