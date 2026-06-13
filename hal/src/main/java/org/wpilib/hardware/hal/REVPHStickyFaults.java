// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/** Sticky faults for a REV PH. These faults will remain active until they are reset by the user. */
public class REVPHStickyFaults {
  /** An overcurrent event occurred on the compressor output. */
  public final boolean compressorOverCurrent;

  /** The compressor output has an open circuit. */
  public final boolean compressorOpen;

  /** An overcurrent event occurred on a solenoid output. */
  public final boolean solenoidOverCurrent;

  /** The input voltage is below the minimum voltage. */
  public final boolean brownout;

  /** A warning was raised by the device's CAN controller. */
  public final boolean canWarning;

  /** The device's CAN controller experienced a "Bus Off" event. */
  public final boolean canBusOff;

  /** The hardware on the device has malfunctioned. */
  public final boolean hardwareFault;

  /** The firmware on the device has malfunctioned. */
  public final boolean firmwareFault;

  /** The device has rebooted. */
  public final boolean hasReset;

  /**
   * Called from HAL.
   *
   * @param faults sticky fault bit mask
   */
  public REVPHStickyFaults(int faults) {
    compressorOverCurrent = (faults & 0x1) != 0;
    compressorOpen = (faults & 0x2) != 0;
    solenoidOverCurrent = (faults & 0x4) != 0;
    brownout = (faults & 0x8) != 0;
    canWarning = (faults & 0x10) != 0;
    canBusOff = (faults & 0x20) != 0;
    hardwareFault = (faults & 0x40) != 0;
    firmwareFault = (faults & 0x80) != 0;
    hasReset = (faults & 0x100) != 0;
  }
}
