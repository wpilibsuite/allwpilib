/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

public final class ControllerUtil {
  /**
   * Returns modulus of error where error is the difference between the reference
   * and a measurement.
   *
   * @param reference Reference input of a controller.
   * @param measurement The current measurement.
   * @param minimumInput The minimum value expected from the input.
   * @param maximumInput The maximum value expected from the input.
   */
  public static double getModulusError(double reference, double measurement, double minimumInput,
      double maximumInput) {
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

  private ControllerUtil() {
  }
}
