// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.HALUtil;
import edu.wpi.first.hal.PowerJNI;
import edu.wpi.first.hal.can.CANJNI;
import edu.wpi.first.hal.can.CANStatus;

/** Contains functions for roboRIO functionality. */
public final class RobotController {
  private RobotController() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Return the FPGA Version number. For now, expect this to be the current year.
   *
   * @return FPGA Version number.
   */
  public static int getFPGAVersion() {
    return HALUtil.getFPGAVersion();
  }

  /**
   * Return the FPGA Revision number. The format of the revision is 3 numbers. The 12 most
   * significant bits are the Major Revision. the next 8 bits are the Minor Revision. The 12 least
   * significant bits are the Build Number.
   *
   * @return FPGA Revision number.
   */
  public static long getFPGARevision() {
    return (long) HALUtil.getFPGARevision();
  }

  /**
   * Read the microsecond timer from the FPGA.
   *
   * @return The current time in microseconds according to the FPGA.
   */
  public static long getFPGATime() {
    return HALUtil.getFPGATime();
  }

  /**
   * Get the state of the "USER" button on the roboRIO.
   *
   * @return true if the button is currently pressed down
   */
  public static boolean getUserButton() {
    return HALUtil.getFPGAButton();
  }

  /**
   * Read the battery voltage.
   *
   * @return The battery voltage in Volts.
   */
  public static double getBatteryVoltage() {
    return PowerJNI.getVinVoltage();
  }

  /**
   * Gets a value indicating whether the FPGA outputs are enabled. The outputs may be disabled if
   * the robot is disabled or e-stopped, the watchdog has expired, or if the roboRIO browns out.
   *
   * @return True if the FPGA outputs are enabled.
   */
  public static boolean isSysActive() {
    return HAL.getSystemActive();
  }

  /**
   * Check if the system is browned out.
   *
   * @return True if the system is browned out
   */
  public static boolean isBrownedOut() {
    return HAL.getBrownedOut();
  }

  /**
   * Get the input voltage to the robot controller.
   *
   * @return The controller input voltage value in Volts
   */
  public static double getInputVoltage() {
    return PowerJNI.getVinVoltage();
  }

  /**
   * Get the input current to the robot controller.
   *
   * @return The controller input current value in Amps
   */
  public static double getInputCurrent() {
    return PowerJNI.getVinCurrent();
  }

  /**
   * Get the voltage of the 3.3V rail.
   *
   * @return The controller 3.3V rail voltage value in Volts
   */
  public static double getVoltage3V3() {
    return PowerJNI.getUserVoltage3V3();
  }

  /**
   * Get the current output of the 3.3V rail.
   *
   * @return The controller 3.3V rail output current value in Amps
   */
  public static double getCurrent3V3() {
    return PowerJNI.getUserCurrent3V3();
  }

  /**
   * Get the enabled state of the 3.3V rail. The rail may be disabled due to a controller brownout,
   * a short circuit on the rail, or controller over-voltage.
   *
   * @return The controller 3.3V rail enabled value
   */
  public static boolean getEnabled3V3() {
    return PowerJNI.getUserActive3V3();
  }

  /**
   * Get the count of the total current faults on the 3.3V rail since the controller has booted.
   *
   * @return The number of faults
   */
  public static int getFaultCount3V3() {
    return PowerJNI.getUserCurrentFaults3V3();
  }

  /**
   * Get the voltage of the 5V rail.
   *
   * @return The controller 5V rail voltage value in Volts
   */
  public static double getVoltage5V() {
    return PowerJNI.getUserVoltage5V();
  }

  /**
   * Get the current output of the 5V rail.
   *
   * @return The controller 5V rail output current value in Amps
   */
  public static double getCurrent5V() {
    return PowerJNI.getUserCurrent5V();
  }

  /**
   * Get the enabled state of the 5V rail. The rail may be disabled due to a controller brownout, a
   * short circuit on the rail, or controller over-voltage.
   *
   * @return The controller 5V rail enabled value
   */
  public static boolean getEnabled5V() {
    return PowerJNI.getUserActive5V();
  }

  /**
   * Get the count of the total current faults on the 5V rail since the controller has booted.
   *
   * @return The number of faults
   */
  public static int getFaultCount5V() {
    return PowerJNI.getUserCurrentFaults5V();
  }

  /**
   * Get the voltage of the 6V rail.
   *
   * @return The controller 6V rail voltage value in Volts
   */
  public static double getVoltage6V() {
    return PowerJNI.getUserVoltage6V();
  }

  /**
   * Get the current output of the 6V rail.
   *
   * @return The controller 6V rail output current value in Amps
   */
  public static double getCurrent6V() {
    return PowerJNI.getUserCurrent6V();
  }

  /**
   * Get the enabled state of the 6V rail. The rail may be disabled due to a controller brownout, a
   * short circuit on the rail, or controller over-voltage.
   *
   * @return The controller 6V rail enabled value
   */
  public static boolean getEnabled6V() {
    return PowerJNI.getUserActive6V();
  }

  /**
   * Get the count of the total current faults on the 6V rail since the controller has booted.
   *
   * @return The number of faults
   */
  public static int getFaultCount6V() {
    return PowerJNI.getUserCurrentFaults6V();
  }

  /**
   * Get the current brownout voltage setting.
   *
   * @return The brownout voltage
   */
  public static double getBrownoutVoltage() {
    return PowerJNI.getBrownoutVoltage();
  }

  /**
   * Set the voltage the roboRIO will brownout and disable all outputs.
   *
   * <p>Note that this only does anything on the roboRIO 2. On the roboRIO it is a no-op.
   *
   * @param brownoutVoltage The brownout voltage
   */
  public static void setBrownoutVoltage(double brownoutVoltage) {
    PowerJNI.setBrownoutVoltage(brownoutVoltage);
  }

  /**
   * Get the current status of the CAN bus.
   *
   * @return The status of the CAN bus
   */
  public static CANStatus getCANStatus() {
    CANStatus status = new CANStatus();
    CANJNI.getCANStatus(status);
    return status;
  }
}
