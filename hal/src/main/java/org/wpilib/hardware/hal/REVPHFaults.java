// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/** Faults for a REV PH. These faults are only active while the condition is active. */
public class REVPHFaults {
  /** Fault on channel 0. */
  public final boolean channel0Fault;

  /** Fault on channel 1. */
  public final boolean channel1Fault;

  /** Fault on channel 2. */
  public final boolean channel2Fault;

  /** Fault on channel 3. */
  public final boolean channel3Fault;

  /** Fault on channel 4. */
  public final boolean channel4Fault;

  /** Fault on channel 5. */
  public final boolean channel5Fault;

  /** Fault on channel 6. */
  public final boolean channel6Fault;

  /** Fault on channel 7. */
  public final boolean channel7Fault;

  /** Fault on channel 8. */
  public final boolean channel8Fault;

  /** Fault on channel 9. */
  public final boolean channel9Fault;

  /** Fault on channel 10. */
  public final boolean channel10Fault;

  /** Fault on channel 11. */
  public final boolean channel11Fault;

  /** Fault on channel 12. */
  public final boolean channel12Fault;

  /** Fault on channel 13. */
  public final boolean channel13Fault;

  /** Fault on channel 14. */
  public final boolean channel14Fault;

  /** Fault on channel 15. */
  public final boolean channel15Fault;

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

  /** The hardware on the device has malfunctioned. */
  public final boolean hardwareFault;

  /**
   * Gets whether there is a fault at the specified channel.
   *
   * @param channel Channel to check for faults.
   * @return True if a a fault exists at the channel, otherwise false.
   * @throws IndexOutOfBoundsException if the provided channel is outside of the range supported by
   *     the hardware.
   */
  public final boolean getchannelFault(int channel) {
    return switch (channel) {
      case 0 -> channel0Fault;
      case 1 -> channel1Fault;
      case 2 -> channel2Fault;
      case 3 -> channel3Fault;
      case 4 -> channel4Fault;
      case 5 -> channel5Fault;
      case 6 -> channel6Fault;
      case 7 -> channel7Fault;
      case 8 -> channel8Fault;
      case 9 -> channel9Fault;
      case 10 -> channel10Fault;
      case 11 -> channel11Fault;
      case 12 -> channel12Fault;
      case 13 -> channel13Fault;
      case 14 -> channel14Fault;
      case 15 -> channel15Fault;
      default -> throw new IndexOutOfBoundsException("Pneumatics fault channel out of bounds!");
    };
  }

  /**
   * Called from HAL to construct.
   *
   * @param faults the fault bitfields
   */
  public REVPHFaults(int faults) {
    channel0Fault = (faults & (1 << 0)) != 0;
    channel1Fault = (faults & (1 << 1)) != 0;
    channel2Fault = (faults & (1 << 2)) != 0;
    channel3Fault = (faults & (1 << 3)) != 0;
    channel4Fault = (faults & (1 << 4)) != 0;
    channel5Fault = (faults & (1 << 5)) != 0;
    channel6Fault = (faults & (1 << 6)) != 0;
    channel7Fault = (faults & (1 << 7)) != 0;
    channel8Fault = (faults & (1 << 8)) != 0;
    channel9Fault = (faults & (1 << 9)) != 0;
    channel10Fault = (faults & (1 << 10)) != 0;
    channel11Fault = (faults & (1 << 11)) != 0;
    channel12Fault = (faults & (1 << 12)) != 0;
    channel13Fault = (faults & (1 << 13)) != 0;
    channel14Fault = (faults & (1 << 14)) != 0;
    channel15Fault = (faults & (1 << 15)) != 0;
    compressorOverCurrent = (faults & (1 << 16)) != 0;
    compressorOpen = (faults & (1 << 17)) != 0;
    solenoidOverCurrent = (faults & (1 << 18)) != 0;
    brownout = (faults & (1 << 19)) != 0;
    canWarning = (faults & (1 << 20)) != 0;
    hardwareFault = (faults & (1 << 21)) != 0;
  }
}
