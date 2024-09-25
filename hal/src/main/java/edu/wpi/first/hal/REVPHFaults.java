// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** Faults for a REV PH. These faults are only active while the condition is active. */
@SuppressWarnings("MemberName")
public class REVPHFaults {
  /** Fault on channel 0. */
  public final boolean Channel0Fault;

  /** Fault on channel 1. */
  public final boolean Channel1Fault;

  /** Fault on channel 2. */
  public final boolean Channel2Fault;

  /** Fault on channel 3. */
  public final boolean Channel3Fault;

  /** Fault on channel 4. */
  public final boolean Channel4Fault;

  /** Fault on channel 5. */
  public final boolean Channel5Fault;

  /** Fault on channel 6. */
  public final boolean Channel6Fault;

  /** Fault on channel 7. */
  public final boolean Channel7Fault;

  /** Fault on channel 8. */
  public final boolean Channel8Fault;

  /** Fault on channel 9. */
  public final boolean Channel9Fault;

  /** Fault on channel 10. */
  public final boolean Channel10Fault;

  /** Fault on channel 11. */
  public final boolean Channel11Fault;

  /** Fault on channel 12. */
  public final boolean Channel12Fault;

  /** Fault on channel 13. */
  public final boolean Channel13Fault;

  /** Fault on channel 14. */
  public final boolean Channel14Fault;

  /** Fault on channel 15. */
  public final boolean Channel15Fault;

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

  /** The hardware on the device has malfunctioned. */
  public final boolean HardwareFault;

  /**
   * Gets whether there is a fault at the specified channel.
   *
   * @param channel Channel to check for faults.
   * @return True if a a fault exists at the channel, otherwise false.
   * @throws IndexOutOfBoundsException if the provided channel is outside of the range supported by
   *     the hardware.
   */
  public final boolean getChannelFault(int channel) {
    return switch (channel) {
      case 0 -> Channel0Fault;
      case 1 -> Channel1Fault;
      case 2 -> Channel2Fault;
      case 3 -> Channel3Fault;
      case 4 -> Channel4Fault;
      case 5 -> Channel5Fault;
      case 6 -> Channel6Fault;
      case 7 -> Channel7Fault;
      case 8 -> Channel8Fault;
      case 9 -> Channel9Fault;
      case 10 -> Channel10Fault;
      case 11 -> Channel11Fault;
      case 12 -> Channel12Fault;
      case 13 -> Channel13Fault;
      case 14 -> Channel14Fault;
      case 15 -> Channel15Fault;
      default -> throw new IndexOutOfBoundsException("Pneumatics fault channel out of bounds!");
    };
  }

  /**
   * Called from HAL to construct.
   *
   * @param faults the fault bitfields
   */
  public REVPHFaults(int faults) {
    Channel0Fault = (faults & (1 << 0)) != 0;
    Channel1Fault = (faults & (1 << 1)) != 0;
    Channel2Fault = (faults & (1 << 2)) != 0;
    Channel3Fault = (faults & (1 << 3)) != 0;
    Channel4Fault = (faults & (1 << 4)) != 0;
    Channel5Fault = (faults & (1 << 5)) != 0;
    Channel6Fault = (faults & (1 << 6)) != 0;
    Channel7Fault = (faults & (1 << 7)) != 0;
    Channel8Fault = (faults & (1 << 8)) != 0;
    Channel9Fault = (faults & (1 << 9)) != 0;
    Channel10Fault = (faults & (1 << 10)) != 0;
    Channel11Fault = (faults & (1 << 11)) != 0;
    Channel12Fault = (faults & (1 << 12)) != 0;
    Channel13Fault = (faults & (1 << 13)) != 0;
    Channel14Fault = (faults & (1 << 14)) != 0;
    Channel15Fault = (faults & (1 << 15)) != 0;
    CompressorOverCurrent = (faults & (1 << 16)) != 0;
    CompressorOpen = (faults & (1 << 17)) != 0;
    SolenoidOverCurrent = (faults & (1 << 18)) != 0;
    Brownout = (faults & (1 << 19)) != 0;
    CanWarning = (faults & (1 << 20)) != 0;
    HardwareFault = (faults & (1 << 21)) != 0;
  }
}
