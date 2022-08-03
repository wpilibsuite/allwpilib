// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

@SuppressWarnings("AbbreviationAsWordInName")
public class REVPHStickyFaults {
  @SuppressWarnings("MemberName")
  public final boolean CompressorOverCurrent;

  @SuppressWarnings("MemberName")
  public final boolean CompressorOpen;

  @SuppressWarnings("MemberName")
  public final boolean SolenoidOverCurrent;

  @SuppressWarnings("MemberName")
  public final boolean Brownout;

  @SuppressWarnings("MemberName")
  public final boolean CanWarning;

  @SuppressWarnings("MemberName")
  public final boolean CanBusOff;

  @SuppressWarnings("MemberName")
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
    HasReset = (faults & 0x40) != 0;
  }
}
