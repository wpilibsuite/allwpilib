// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/** Data decoded from an A301 periodic status 1 frame. */
public class A301PeriodicStatus1 {
  /** Packed active fault bits. */
  public final int faults;

  /** Packed active warning bits. */
  public final int warnings;

  /** Packed sticky fault bits. */
  public final int stickyFaults;

  /** Packed sticky warning bits. */
  public final int stickyWarnings;

  /** Whether an uncategorized fault is active. */
  public final boolean otherFault;

  /** Whether a motor-type fault is active. */
  public final boolean motorTypeFault;

  /** Whether a sensor fault is active. */
  public final boolean sensorFault;

  /** Whether a CAN communication fault is active. */
  public final boolean canFault;

  /** Whether a temperature fault is active. */
  public final boolean temperatureFault;

  /** Whether a gate-driver fault is active. */
  public final boolean drvFault;

  /** Whether an ESC EEPROM fault is active. */
  public final boolean escEepromFault;

  /** Whether a firmware fault is active. */
  public final boolean firmwareFault;

  /** Whether a motor-startup fault is active. */
  public final boolean motorStartupFault;

  /** Whether a brownout warning is active. */
  public final boolean brownoutWarning;

  /** Whether an overcurrent warning is active. */
  public final boolean overcurrentWarning;

  /** Whether an ESC EEPROM warning is active. */
  public final boolean escEepromWarning;

  /** Whether an external EEPROM warning is active. */
  public final boolean extEepromWarning;

  /** Whether a sensor warning is active. */
  public final boolean sensorWarning;

  /** Whether a stall warning is active. */
  public final boolean stallWarning;

  /** Whether a controller-reset warning is active. */
  public final boolean hasResetWarning;

  /** Whether an uncategorized warning is active. */
  public final boolean otherWarning;

  /** Whether an overvoltage warning is active. */
  public final boolean overvoltageWarning;

  /** Whether a motor-loop-speed warning is active. */
  public final boolean motorLoopSpeedWarning;

  /** Whether an uncategorized fault has occurred since faults were last cleared. */
  public final boolean otherStickyFault;

  /** Whether a motor-type fault has occurred since faults were last cleared. */
  public final boolean motorTypeStickyFault;

  /** Whether a sensor fault has occurred since faults were last cleared. */
  public final boolean sensorStickyFault;

  /** Whether a CAN communication fault has occurred since faults were last cleared. */
  public final boolean canStickyFault;

  /** Whether a temperature fault has occurred since faults were last cleared. */
  public final boolean temperatureStickyFault;

  /** Whether a gate-driver fault has occurred since faults were last cleared. */
  public final boolean drvStickyFault;

  /** Whether an ESC EEPROM fault has occurred since faults were last cleared. */
  public final boolean escEepromStickyFault;

  /** Whether a firmware fault has occurred since faults were last cleared. */
  public final boolean firmwareStickyFault;

  /** Whether a motor-startup fault has occurred since faults were last cleared. */
  public final boolean motorStartupStickyFault;

  /** Whether a brownout warning has occurred since warnings were last cleared. */
  public final boolean brownoutStickyWarning;

  /** Whether an overcurrent warning has occurred since warnings were last cleared. */
  public final boolean overcurrentStickyWarning;

  /** Whether an ESC EEPROM warning has occurred since warnings were last cleared. */
  public final boolean escEepromStickyWarning;

  /** Whether an external EEPROM warning has occurred since warnings were last cleared. */
  public final boolean extEepromStickyWarning;

  /** Whether a sensor warning has occurred since warnings were last cleared. */
  public final boolean sensorStickyWarning;

  /** Whether a stall warning has occurred since warnings were last cleared. */
  public final boolean stallStickyWarning;

  /** Whether a controller reset has occurred since warnings were last cleared. */
  public final boolean hasResetStickyWarning;

  /** Whether an uncategorized warning has occurred since warnings were last cleared. */
  public final boolean otherStickyWarning;

  /** Whether an overvoltage warning has occurred since warnings were last cleared. */
  public final boolean overvoltageStickyWarning;

  /** Whether a motor-loop-speed warning has occurred since warnings were last cleared. */
  public final boolean motorLoopSpeedStickyWarning;

  /** Whether the controller is following another controller. */
  public final boolean isFollower;

  /** HAL status associated with the read. */
  public final int status;

  /** CAN frame timestamp in microseconds. */
  public final long timestamp;

  /**
   * Constructs A301 periodic status 1 data (called from the HAL).
   *
   * @param faults packed active fault bits
   * @param warnings packed active warning bits
   * @param stickyFaults packed sticky fault bits
   * @param stickyWarnings packed sticky warning bits
   * @param isFollower whether the controller is following another controller
   * @param status HAL status associated with the read
   * @param timestamp CAN frame timestamp in microseconds
   */
  public A301PeriodicStatus1(
      int faults,
      int warnings,
      int stickyFaults,
      int stickyWarnings,
      boolean isFollower,
      int status,
      long timestamp) {
    this.faults = faults;
    this.warnings = warnings;
    this.stickyFaults = stickyFaults;
    this.stickyWarnings = stickyWarnings;
    otherFault = getBit(faults, 0);
    motorTypeFault = getBit(faults, 1);
    sensorFault = getBit(faults, 2);
    canFault = getBit(faults, 3);
    temperatureFault = getBit(faults, 4);
    drvFault = getBit(faults, 5);
    escEepromFault = getBit(faults, 6);
    firmwareFault = getBit(faults, 7);
    motorStartupFault = getBit(faults, 8);
    brownoutWarning = getBit(warnings, 0);
    overcurrentWarning = getBit(warnings, 1);
    escEepromWarning = getBit(warnings, 2);
    extEepromWarning = getBit(warnings, 3);
    sensorWarning = getBit(warnings, 4);
    stallWarning = getBit(warnings, 5);
    hasResetWarning = getBit(warnings, 6);
    otherWarning = getBit(warnings, 7);
    overvoltageWarning = getBit(warnings, 8);
    motorLoopSpeedWarning = getBit(warnings, 9);
    otherStickyFault = getBit(stickyFaults, 0);
    motorTypeStickyFault = getBit(stickyFaults, 1);
    sensorStickyFault = getBit(stickyFaults, 2);
    canStickyFault = getBit(stickyFaults, 3);
    temperatureStickyFault = getBit(stickyFaults, 4);
    drvStickyFault = getBit(stickyFaults, 5);
    escEepromStickyFault = getBit(stickyFaults, 6);
    firmwareStickyFault = getBit(stickyFaults, 7);
    motorStartupStickyFault = getBit(stickyFaults, 8);
    brownoutStickyWarning = getBit(stickyWarnings, 0);
    overcurrentStickyWarning = getBit(stickyWarnings, 1);
    escEepromStickyWarning = getBit(stickyWarnings, 2);
    extEepromStickyWarning = getBit(stickyWarnings, 3);
    sensorStickyWarning = getBit(stickyWarnings, 4);
    stallStickyWarning = getBit(stickyWarnings, 5);
    hasResetStickyWarning = getBit(stickyWarnings, 6);
    otherStickyWarning = getBit(stickyWarnings, 7);
    overvoltageStickyWarning = getBit(stickyWarnings, 8);
    motorLoopSpeedStickyWarning = getBit(stickyWarnings, 9);
    this.isFollower = isFollower;
    this.status = status;
    this.timestamp = timestamp;
  }

  private static boolean getBit(int value, int bit) {
    return (value & (1 << bit)) != 0;
  }
}
