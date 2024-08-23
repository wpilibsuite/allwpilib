// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** Sticky faults for a REV PH. These faults will remain active until they are reset by the user. */
@SuppressWarnings("MemberName")
public class REVPHStickyFaults {
  /** An overcurrent event occurred on the compressor output. */
  public final boolean CompressorOverCurrent;

  /** The compressor output has an open circuit. */
  public final boolean CompressorOpen;

  /** An overcurrent event occurred on a solenoid output. */
  public final boolean SolenoidOverCurrent;

  /** The input voltage is below the minimum voltage. */
  public final boolean Brownout;

  /** A warning was raised by the device's CAN controller. */
  public final boolean CanWarning;

  /** The device's CAN controller experienced a "Bus Off" event. */
  public final boolean CanBusOff;

  /** The hardware on the device has malfunctioned. */
  public final boolean HardwareFault;

  /** The firmware on the device has malfunctioned. */
  public final boolean FirmwareFault;

  /** The device has rebooted. */
  public final boolean HasReset;

  /**
   * Called from HAL.
   *
   * @param faults sticky fault bit mask
   */
  public REVPHStickyFaults(int faults) {
    CompressorOverCurrent = (faults & 0x1) != 0;
    CompressorOpen = (faults & 0x2) != 0;
    SolenoidOverCurrent = (faults & 0x4) != 0;
    Brownout = (faults & 0x8) != 0;
    CanWarning = (faults & 0x10) != 0;
    CanBusOff = (faults & 0x20) != 0;
    HardwareFault = (faults & 0x40) != 0;
    FirmwareFault = (faults & 0x80) != 0;
    HasReset = (faults & 0x100) != 0;
  }
}
