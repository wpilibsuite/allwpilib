// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import static org.wpilib.units.Units.Amps;
import static org.wpilib.units.Units.Celsius;
import static org.wpilib.units.Units.Microseconds;
import static org.wpilib.units.Units.Volts;

import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.HALUtil;
import org.wpilib.hardware.hal.PowerJNI;
import org.wpilib.hardware.hal.can.CANJNI;
import org.wpilib.hardware.hal.can.CANStatus;
import org.wpilib.units.measure.Current;
import org.wpilib.units.measure.Temperature;
import org.wpilib.units.measure.Time;
import org.wpilib.units.measure.Voltage;
import java.util.function.LongSupplier;

/** Contains functions for roboRIO functionality. */
public final class RobotController {
  private static LongSupplier m_timeSource = RobotController::getFPGATime;

  private RobotController() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Return the serial number of the roboRIO.
   *
   * @return The serial number of the roboRIO.
   */
  public static String getSerialNumber() {
    return HALUtil.getSerialNumber();
  }

  /**
   * Return the comments from the roboRIO web interface.
   *
   * <p>The comments string is cached after the first call to this function on the RoboRIO - restart
   * the robot code to reload the comments string after changing it in the web interface.
   *
   * @return the comments from the roboRIO web interface.
   */
  public static String getComments() {
    return HALUtil.getComments();
  }

  /**
   * Returns the team number configured for the robot controller.
   *
   * @return team number, or 0 if not found.
   */
  public static int getTeamNumber() {
    return HALUtil.getTeamNumber();
  }

  /**
   * Sets a new source to provide the clock time in microseconds. Changing this affects the return
   * value of {@code getTime} in Java.
   *
   * @param supplier Function to return the time in microseconds.
   */
  public static void setTimeSource(LongSupplier supplier) {
    m_timeSource = supplier;
  }

  /**
   * Read the microsecond timestamp. By default, the time is based on the FPGA hardware clock in
   * microseconds since the FPGA started. However, the return value of this method may be modified
   * to use any time base, including non-monotonic and non-continuous time bases.
   *
   * @return The current time in microseconds.
   */
  public static long getTime() {
    return m_timeSource.getAsLong();
  }

  /**
   * Read the microsecond timestamp. By default, the time is based on the FPGA hardware clock in
   * microseconds since the FPGA started. However, the return value of this method may be modified
   * to use any time base, including non-monotonic and non-continuous time bases.
   *
   * @return The current time in a measure.
   */
  public static Time getMeasureTime() {
    return Microseconds.of(m_timeSource.getAsLong());
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
   * Read the microsecond timer in a measure from the FPGA.
   *
   * @return The current time according to the FPGA in a measure.
   */
  public static Time getMeasureFPGATime() {
    return Microseconds.of(HALUtil.getFPGATime());
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
   * Read the battery voltage in a measure.
   *
   * @return The battery voltage in a measure.
   */
  public static Voltage getMeasureBatteryVoltage() {
    return Volts.of(PowerJNI.getVinVoltage());
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
   * Gets the number of times the system has been disabled due to communication errors with the
   * Driver Station.
   *
   * @return number of disables due to communication errors.
   */
  public static int getCommsDisableCount() {
    return HAL.getCommsDisableCount();
  }

  /**
   * Gets the current state of the Robot Signal Light (RSL).
   *
   * @return The current state of the RSL- true if on, false if off
   */
  public static boolean getRSLState() {
    return HAL.getRSLState();
  }

  /**
   * Gets if the system time is valid.
   *
   * @return True if the system time is valid, false otherwise
   */
  public static boolean isSystemTimeValid() {
    return HAL.getSystemTimeValid();
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
   * Get the input voltage to the robot controller in a measure.
   *
   * @return The controller input voltage value in a measure.
   */
  public static Voltage getMeasureInputVoltage() {
    return Volts.of(PowerJNI.getVinVoltage());
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
   * Get the voltage in a measure of the 3.3V rail.
   *
   * @return The controller 3.3V rail voltage value in a measure.
   */
  public static Voltage getMeasureVoltage3V3() {
    return Volts.of(PowerJNI.getUserVoltage3V3());
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
   * Get the current output in a measure of the 3.3V rail.
   *
   * @return The controller 3.3V rail output current value in a measure.
   */
  public static Current getMeasureCurrent3V3() {
    return Amps.of(PowerJNI.getUserCurrent3V3());
  }

  /**
   * Enables or disables the 3.3V rail.
   *
   * @param enabled whether to enable the 3.3V rail.
   */
  public static void setEnabled3V3(boolean enabled) {
    PowerJNI.setUserEnabled3V3(enabled);
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
   * Get the count of the total current faults on the 3.3V rail since the code started.
   *
   * @return The number of faults
   */
  public static int getFaultCount3V3() {
    return PowerJNI.getUserCurrentFaults3V3();
  }

  /** Reset the overcurrent fault counters for all user rails to 0. */
  public static void resetRailFaultCounts() {
    PowerJNI.resetUserCurrentFaults();
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
   * Get the current brownout voltage setting in a measure.
   *
   * @return The brownout voltage in a measure.
   */
  public static Voltage getMeasureBrownoutVoltage() {
    return Volts.of(PowerJNI.getBrownoutVoltage());
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
   * Set the voltage in a measure the roboRIO will brownout and disable all outputs.
   *
   * <p>Note that this only does anything on the roboRIO 2. On the roboRIO it is a no-op.
   *
   * @param brownoutVoltage The brownout voltage in a measure
   */
  public static void setBrownoutVoltage(Voltage brownoutVoltage) {
    PowerJNI.setBrownoutVoltage(brownoutVoltage.baseUnitMagnitude());
  }

  /**
   * Get the current CPU temperature in degrees Celsius.
   *
   * @return current CPU temperature in degrees Celsius
   */
  public static double getCPUTemp() {
    return PowerJNI.getCPUTemp();
  }

  /**
   * Get the current CPU temperature in a measure.
   *
   * @return current CPU temperature in a measure.
   */
  public static Temperature getMeasureCPUTemp() {
    return Celsius.of(PowerJNI.getCPUTemp());
  }

  /**
   * Get the current status of the CAN bus.
   *
   * @param busId The bus ID
   * @return The status of the CAN bus
   */
  public static CANStatus getCANStatus(int busId) {
    CANStatus status = new CANStatus();
    CANJNI.getCANStatus(busId, status);
    return status;
  }
}
