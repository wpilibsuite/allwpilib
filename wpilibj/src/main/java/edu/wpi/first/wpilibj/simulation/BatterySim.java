// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

/** A utility class to simulate the robot battery. */
public final class BatterySim {
  private BatterySim() {
    // Utility class
  }

  /**
   * Calculate the loaded battery voltage. Use this with {@link RoboRioSim#setVInVoltage(double)} to
   * set the simulated battery voltage, which can then be retrieved with the {@link
   * edu.wpi.first.wpilibj.RobotController#getBatteryVoltage()} method.
   *
   * @param nominalVoltage The nominal battery voltage. Usually 12v.
   * @param resistanceOhms The forward resistance of the battery. Most batteries are at or below 20
   *     milliohms.
   * @param currents The currents drawn from the battery.
   * @return The battery's voltage under load.
   */
  public static double calculateLoadedBatteryVoltage(
      double nominalVoltage, double resistanceOhms, double... currents) {
    double retval = nominalVoltage;
    for (var current : currents) {
      retval -= current * resistanceOhms;
    }
    return retval;
  }

  /**
   * Calculate the loaded battery voltage. Use this with {@link RoboRioSim#setVInVoltage(double)} to
   * set the simulated battery voltage, which can then be retrieved with the {@link
   * edu.wpi.first.wpilibj.RobotController#getBatteryVoltage()} method. This function assumes a
   * nominal voltage of 12v and a resistance of 20 milliohms (0.020 ohms)
   *
   * @param currents The currents drawn from the battery.
   * @return The battery's voltage under load.
   */
  public static double calculateDefaultBatteryLoadedVoltage(double... currents) {
    return calculateLoadedBatteryVoltage(12, 0.02, currents);
  }
}
