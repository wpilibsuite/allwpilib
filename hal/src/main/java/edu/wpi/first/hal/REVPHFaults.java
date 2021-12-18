// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

@SuppressWarnings("AbbreviationAsWordInName")
public class REVPHFaults {
  @SuppressWarnings("MemberName")
  public final boolean Channel0Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel1Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel2Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel3Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel4Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel5Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel6Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel7Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel8Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel9Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel10Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel11Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel12Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel13Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel14Fault;

  @SuppressWarnings("MemberName")
  public final boolean Channel15Fault;

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
  public final boolean HardwareFault;

  /**
   * Called from HAL to construct.
   * 
   * @param faults the fault bitfields
   */
  public REVPHFaults(int faults) {
    Channel0Fault = (faults & 0x1) != 0;
    Channel1Fault = (faults & 0x2) != 0;
    Channel2Fault = (faults & 0x4) != 0;
    Channel3Fault = (faults & 0x8) != 0;
    Channel4Fault = (faults & 0x10) != 0;
    Channel5Fault = (faults & 0x20) != 0;
    Channel6Fault = (faults & 0x40) != 0;
    Channel7Fault = (faults & 0x80) != 0;
    Channel8Fault = (faults & 0x100) != 0;
    Channel9Fault = (faults & 0x200) != 0;
    Channel10Fault = (faults & 0x400) != 0;
    Channel11Fault = (faults & 0x800) != 0;
    Channel12Fault = (faults & 0x1000) != 0;
    Channel13Fault = (faults & 0x2000) != 0;
    Channel14Fault = (faults & 0x4000) != 0;
    Channel15Fault = (faults & 0x8000) != 0;
    CompressorOverCurrent = (faults & 0x8000) != 0;
    CompressorOpen = (faults & 0x10000) != 0;
    SolenoidOverCurrent = (faults & 0x20000) != 0;
    Brownout = (faults & 0x40000) != 0;
    CanWarning = (faults & 0x80000) != 0;
    HardwareFault = (faults & 0x100000) != 0;
  }
}
