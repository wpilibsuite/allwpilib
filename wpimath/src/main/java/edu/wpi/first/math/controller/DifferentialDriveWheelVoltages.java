// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

/** Motor voltages for a differential drive. */
public class DifferentialDriveWheelVoltages {
  public double left;
  public double right;

  public DifferentialDriveWheelVoltages() {}

  public DifferentialDriveWheelVoltages(double left, double right) {
    this.left = left;
    this.right = right;
  }
}
