// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.controller;

public final class ControllerUtil {
  /**
   * Returns modulus of error where error is the difference between the reference and a measurement.
   *
   * @param reference Reference input of a controller.
   * @param measurement The current measurement.
   * @param minimumInput The minimum value expected from the input.
   * @param maximumInput The maximum value expected from the input.
   */
  public static double getModulusError(
      double reference, double measurement, double minimumInput, double maximumInput) {
    double error = reference - measurement;
    double modulus = maximumInput - minimumInput;

    // Wrap error above maximum input
    int numMax = (int) ((error + maximumInput) / modulus);
    error -= numMax * modulus;

    // Wrap error below minimum input
    int numMin = (int) ((error + minimumInput) / modulus);
    error -= numMin * modulus;

    return error;
  }

  private ControllerUtil() {}
}
