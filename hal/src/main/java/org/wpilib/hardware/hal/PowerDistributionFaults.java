// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * Faults for a PowerDistribution device. These faults are only active while the condition is
 * active.
 */
public class PowerDistributionFaults {
  /** Breaker fault on channel 0. */
  public final boolean channel0BreakerFault;

  /** Breaker fault on channel 1. */
  public final boolean channel1BreakerFault;

  /** Breaker fault on channel 2. */
  public final boolean channel2BreakerFault;

  /** Breaker fault on channel 3. */
  public final boolean channel3BreakerFault;

  /** Breaker fault on channel 4. */
  public final boolean channel4BreakerFault;

  /** Breaker fault on channel 5. */
  public final boolean channel5BreakerFault;

  /** Breaker fault on channel 6. */
  public final boolean channel6BreakerFault;

  /** Breaker fault on channel 7. */
  public final boolean channel7BreakerFault;

  /** Breaker fault on channel 8. */
  public final boolean channel8BreakerFault;

  /** Breaker fault on channel 9. */
  public final boolean channel9BreakerFault;

  /** Breaker fault on channel 10. */
  public final boolean channel10BreakerFault;

  /** Breaker fault on channel 11. */
  public final boolean channel11BreakerFault;

  /** Breaker fault on channel 12. */
  public final boolean channel12BreakerFault;

  /** Breaker fault on channel 13. */
  public final boolean channel13BreakerFault;

  /** Breaker fault on channel 14. */
  public final boolean channel14BreakerFault;

  /** Breaker fault on channel 15. */
  public final boolean channel15BreakerFault;

  /** Breaker fault on channel 16. */
  public final boolean channel16BreakerFault;

  /** Breaker fault on channel 17. */
  public final boolean channel17BreakerFault;

  /** Breaker fault on channel 18. */
  public final boolean channel18BreakerFault;

  /** Breaker fault on channel 19. */
  public final boolean channel19BreakerFault;

  /** Breaker fault on channel 20. */
  public final boolean channel20BreakerFault;

  /** Breaker fault on channel 21. */
  public final boolean channel21BreakerFault;

  /** Breaker fault on channel 22. */
  public final boolean channel22BreakerFault;

  /** Breaker fault on channel 23. */
  public final boolean channel23BreakerFault;

  /** The input voltage is below the minimum voltage. */
  public final boolean brownout;

  /** A warning was raised by the device's CAN controller. */
  public final boolean canWarning;

  /** The hardware on the device has malfunctioned. */
  public final boolean hardwareFault;

  /**
   * Gets whether there is a breaker fault at the specified channel.
   *
   * @param channel Channel to check for faults.
   * @return True if there is a breaker fault at the channel, otherwise false.
   * @throws IndexOutOfBoundsException if the provided channel is outside of the range supported by
   *     the hardware.
   */
  public final boolean getBreakerFault(int channel) {
    return switch (channel) {
      case 0 -> channel0BreakerFault;
      case 1 -> channel1BreakerFault;
      case 2 -> channel2BreakerFault;
      case 3 -> channel3BreakerFault;
      case 4 -> channel4BreakerFault;
      case 5 -> channel5BreakerFault;
      case 6 -> channel6BreakerFault;
      case 7 -> channel7BreakerFault;
      case 8 -> channel8BreakerFault;
      case 9 -> channel9BreakerFault;
      case 10 -> channel10BreakerFault;
      case 11 -> channel11BreakerFault;
      case 12 -> channel12BreakerFault;
      case 13 -> channel13BreakerFault;
      case 14 -> channel14BreakerFault;
      case 15 -> channel15BreakerFault;
      case 16 -> channel16BreakerFault;
      case 17 -> channel17BreakerFault;
      case 18 -> channel18BreakerFault;
      case 19 -> channel19BreakerFault;
      case 20 -> channel20BreakerFault;
      case 21 -> channel21BreakerFault;
      case 22 -> channel22BreakerFault;
      case 23 -> channel23BreakerFault;
      default ->
          throw new IndexOutOfBoundsException("Power distribution fault channel out of bounds!");
    };
  }

  /**
   * Constructs from a bitfield.
   *
   * @param faults faults
   */
  public PowerDistributionFaults(int faults) {
    channel0BreakerFault = (faults & 0x1) != 0;
    channel1BreakerFault = (faults & 0x2) != 0;
    channel2BreakerFault = (faults & 0x4) != 0;
    channel3BreakerFault = (faults & 0x8) != 0;
    channel4BreakerFault = (faults & 0x10) != 0;
    channel5BreakerFault = (faults & 0x20) != 0;
    channel6BreakerFault = (faults & 0x40) != 0;
    channel7BreakerFault = (faults & 0x80) != 0;
    channel8BreakerFault = (faults & 0x100) != 0;
    channel9BreakerFault = (faults & 0x200) != 0;
    channel10BreakerFault = (faults & 0x400) != 0;
    channel11BreakerFault = (faults & 0x800) != 0;
    channel12BreakerFault = (faults & 0x1000) != 0;
    channel13BreakerFault = (faults & 0x2000) != 0;
    channel14BreakerFault = (faults & 0x4000) != 0;
    channel15BreakerFault = (faults & 0x8000) != 0;
    channel16BreakerFault = (faults & 0x10000) != 0;
    channel17BreakerFault = (faults & 0x20000) != 0;
    channel18BreakerFault = (faults & 0x40000) != 0;
    channel19BreakerFault = (faults & 0x80000) != 0;
    channel20BreakerFault = (faults & 0x100000) != 0;
    channel21BreakerFault = (faults & 0x200000) != 0;
    channel22BreakerFault = (faults & 0x400000) != 0;
    channel23BreakerFault = (faults & 0x800000) != 0;
    brownout = (faults & 0x1000000) != 0;
    canWarning = (faults & 0x2000000) != 0;
    hardwareFault = (faults & 0x4000000) != 0;
  }
}
