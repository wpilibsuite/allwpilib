// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/** Data decoded from an A301 periodic status 1 frame. */
public class A301PeriodicStatus1 {
  public final int faults;
  public final int warnings;
  public final int stickyFaults;
  public final int stickyWarnings;

  public final boolean otherFault;
  public final boolean motorTypeFault;
  public final boolean sensorFault;
  public final boolean canFault;
  public final boolean temperatureFault;
  public final boolean drvFault;
  public final boolean escEepromFault;
  public final boolean firmwareFault;
  public final boolean motorStartupFault;

  public final boolean brownoutWarning;
  public final boolean overcurrentWarning;
  public final boolean escEepromWarning;
  public final boolean extEepromWarning;
  public final boolean sensorWarning;
  public final boolean stallWarning;
  public final boolean hasResetWarning;
  public final boolean otherWarning;
  public final boolean overvoltageWarning;
  public final boolean motorLoopSpeedWarning;

  public final boolean otherStickyFault;
  public final boolean motorTypeStickyFault;
  public final boolean sensorStickyFault;
  public final boolean canStickyFault;
  public final boolean temperatureStickyFault;
  public final boolean drvStickyFault;
  public final boolean escEepromStickyFault;
  public final boolean firmwareStickyFault;
  public final boolean motorStartupStickyFault;

  public final boolean brownoutStickyWarning;
  public final boolean overcurrentStickyWarning;
  public final boolean escEepromStickyWarning;
  public final boolean extEepromStickyWarning;
  public final boolean sensorStickyWarning;
  public final boolean stallStickyWarning;
  public final boolean hasResetStickyWarning;
  public final boolean otherStickyWarning;
  public final boolean overvoltageStickyWarning;
  public final boolean motorLoopSpeedStickyWarning;

  public final boolean isFollower;
  public final int status;
  public final long timestamp;

  /** Constructs A301 periodic status 1 data (called from the HAL). */
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
