/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.PowerJNI;

public class ControllerPower {
  /**
   * Get the input voltage to the robot controller
   *$
   * @return The controller input voltage value in Volts
   */
  public static double getInputVoltage() {
    return PowerJNI.getVinVoltage();
  }

  /**
   * Get the input current to the robot controller
   *$
   * @return The controller input current value in Amps
   */
  public static double getInputCurrent() {
    return PowerJNI.getVinCurrent();
  }

  /**
   * Get the voltage of the 3.3V rail
   *$
   * @return The controller 3.3V rail voltage value in Volts
   */
  public static double getVoltage3V3() {
    return PowerJNI.getUserVoltage3V3();
  }

  /**
   * Get the current output of the 3.3V rail
   *$
   * @return The controller 3.3V rail output current value in Volts
   */
  public static double getCurrent3V3() {
    return PowerJNI.getUserCurrent3V3();
  }

  /**
   * Get the enabled state of the 3.3V rail. The rail may be disabled due to a
   * controller brownout, a short circuit on the rail, or controller
   * over-voltage
   *$
   * @return The controller 3.3V rail enabled value
   */
  public static boolean getEnabled3V3() {
    return PowerJNI.getUserActive3V3();
  }

  /**
   * Get the count of the total current faults on the 3.3V rail since the
   * controller has booted
   *$
   * @return The number of faults
   */
  public static int getFaultCount3V3() {
    return PowerJNI.getUserCurrentFaults3V3();
  }

  /**
   * Get the voltage of the 5V rail
   *$
   * @return The controller 5V rail voltage value in Volts
   */
  public static double getVoltage5V() {
    return PowerJNI.getUserVoltage5V();
  }

  /**
   * Get the current output of the 5V rail
   *$
   * @return The controller 5V rail output current value in Amps
   */
  public static double getCurrent5V() {
    return PowerJNI.getUserCurrent5V();
  }

  /**
   * Get the enabled state of the 5V rail. The rail may be disabled due to a
   * controller brownout, a short circuit on the rail, or controller
   * over-voltage
   *$
   * @return The controller 5V rail enabled value
   */
  public static boolean getEnabled5V() {
    return PowerJNI.getUserActive5V();
  }

  /**
   * Get the count of the total current faults on the 5V rail since the
   * controller has booted
   *$
   * @return The number of faults
   */
  public static int getFaultCount5V() {
    return PowerJNI.getUserCurrentFaults5V();
  }

  /**
   * Get the voltage of the 6V rail
   *$
   * @return The controller 6V rail voltage value in Volts
   */
  public static double getVoltage6V() {
    return PowerJNI.getUserVoltage6V();
  }

  /**
   * Get the current output of the 6V rail
   *$
   * @return The controller 6V rail output current value in Amps
   */
  public static double getCurrent6V() {
    return PowerJNI.getUserCurrent6V();
  }

  /**
   * Get the enabled state of the 6V rail. The rail may be disabled due to a
   * controller brownout, a short circuit on the rail, or controller
   * over-voltage
   *$
   * @return The controller 6V rail enabled value
   */
  public static boolean getEnabled6V() {
    return PowerJNI.getUserActive6V();
  }

  /**
   * Get the count of the total current faults on the 6V rail since the
   * controller has booted
   *$
   * @return The number of faults
   */
  public static int getFaultCount6V() {
    return PowerJNI.getUserCurrentFaults6V();
  }
}
