/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.RobotController;

public final class BatterySim {
  private BatterySim() {
    // Utility class
  }

  /**
   * Calculate the loaded battery voltage. Use this with
   * {@link RoboRioSim#setVInVoltage(double)} to set the simulated battery
   * voltage, which can then be retrieved with the {@link RobotController#getBatteryVoltage()}
   * method.
   *
   * @param nominalVoltage The nominal battery voltage. Usually 12v.
   * @param resistanceOhms The forward resistance of the battery. Most batteries are at or
   *                       below 20 milliohms.
   * @param currents       The currents drawn from the battery.
   * @return The battery's voltage under load.
   */
  public static double calculateLoadedBatteryVoltage(double nominalVoltage,
                                                     double resistanceOhms,
                                                     double... currents) {
    double retval = nominalVoltage;
    for (var current : currents) {
      retval -= current * resistanceOhms;
    }
    return retval;
  }

  /**
   * Calculate the loaded battery voltage. Use this with
   * {@link RoboRioSim#setVInVoltage(double)} to set the simulated battery
   * voltage, which can then be retrieved with the {@link RobotController#getBatteryVoltage()}
   * method. This function assumes a nominal voltage of 12v and a resistance of 20 milliohms
   * (0.020 ohms)
   *
   * @param currents The currents drawn from the battery.
   * @return The battery's voltage under load.
   */
  public static double calculateDefaultBatteryLoadedVoltage(double... currents) {
    return calculateLoadedBatteryVoltage(12, 0.02, currents);
  }
}
