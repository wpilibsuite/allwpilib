// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * Faults for a PowerDistribution device. These faults are only active while the condition is
 * active.
 */
@SuppressWarnings("MemberName")
public class PowerDistributionFaults {
  /** Breaker fault on channel 0. */
  public final boolean Channel0BreakerFault;

  /** Breaker fault on channel 1. */
  public final boolean Channel1BreakerFault;

  /** Breaker fault on channel 2. */
  public final boolean Channel2BreakerFault;

  /** Breaker fault on channel 3. */
  public final boolean Channel3BreakerFault;

  /** Breaker fault on channel 4. */
  public final boolean Channel4BreakerFault;

  /** Breaker fault on channel 5. */
  public final boolean Channel5BreakerFault;

  /** Breaker fault on channel 6. */
  public final boolean Channel6BreakerFault;

  /** Breaker fault on channel 7. */
  public final boolean Channel7BreakerFault;

  /** Breaker fault on channel 8. */
  public final boolean Channel8BreakerFault;

  /** Breaker fault on channel 9. */
  public final boolean Channel9BreakerFault;

  /** Breaker fault on channel 10. */
  public final boolean Channel10BreakerFault;

  /** Breaker fault on channel 11. */
  public final boolean Channel11BreakerFault;

  /** Breaker fault on channel 12. */
  public final boolean Channel12BreakerFault;

  /** Breaker fault on channel 13. */
  public final boolean Channel13BreakerFault;

  /** Breaker fault on channel 14. */
  public final boolean Channel14BreakerFault;

  /** Breaker fault on channel 15. */
  public final boolean Channel15BreakerFault;

  /** Breaker fault on channel 16. */
  public final boolean Channel16BreakerFault;

  /** Breaker fault on channel 17. */
  public final boolean Channel17BreakerFault;

  /** Breaker fault on channel 18. */
  public final boolean Channel18BreakerFault;

  /** Breaker fault on channel 19. */
  public final boolean Channel19BreakerFault;

  /** Breaker fault on channel 20. */
  public final boolean Channel20BreakerFault;

  /** Breaker fault on channel 21. */
  public final boolean Channel21BreakerFault;

  /** Breaker fault on channel 22. */
  public final boolean Channel22BreakerFault;

  /** Breaker fault on channel 23. */
  public final boolean Channel23BreakerFault;

  /** The input voltage is below the minimum voltage. */
  public final boolean Brownout;

  /** A warning was raised by the device's CAN controller. */
  public final boolean CanWarning;

  /** The hardware on the device has malfunctioned. */
  public final boolean HardwareFault;

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
      case 0 -> Channel0BreakerFault;
      case 1 -> Channel1BreakerFault;
      case 2 -> Channel2BreakerFault;
      case 3 -> Channel3BreakerFault;
      case 4 -> Channel4BreakerFault;
      case 5 -> Channel5BreakerFault;
      case 6 -> Channel6BreakerFault;
      case 7 -> Channel7BreakerFault;
      case 8 -> Channel8BreakerFault;
      case 9 -> Channel9BreakerFault;
      case 10 -> Channel10BreakerFault;
      case 11 -> Channel11BreakerFault;
      case 12 -> Channel12BreakerFault;
      case 13 -> Channel13BreakerFault;
      case 14 -> Channel14BreakerFault;
      case 15 -> Channel15BreakerFault;
      case 16 -> Channel16BreakerFault;
      case 17 -> Channel17BreakerFault;
      case 18 -> Channel18BreakerFault;
      case 19 -> Channel19BreakerFault;
      case 20 -> Channel20BreakerFault;
      case 21 -> Channel21BreakerFault;
      case 22 -> Channel22BreakerFault;
      case 23 -> Channel23BreakerFault;
      default -> throw new IndexOutOfBoundsException(
          "Power distribution fault channel out of bounds!");
    };
  }

  /**
   * Constructs from a bitfield.
   *
   * @param faults faults
   */
  public PowerDistributionFaults(int faults) {
    Channel0BreakerFault = (faults & 0x1) != 0;
    Channel1BreakerFault = (faults & 0x2) != 0;
    Channel2BreakerFault = (faults & 0x4) != 0;
    Channel3BreakerFault = (faults & 0x8) != 0;
    Channel4BreakerFault = (faults & 0x10) != 0;
    Channel5BreakerFault = (faults & 0x20) != 0;
    Channel6BreakerFault = (faults & 0x40) != 0;
    Channel7BreakerFault = (faults & 0x80) != 0;
    Channel8BreakerFault = (faults & 0x100) != 0;
    Channel9BreakerFault = (faults & 0x200) != 0;
    Channel10BreakerFault = (faults & 0x400) != 0;
    Channel11BreakerFault = (faults & 0x800) != 0;
    Channel12BreakerFault = (faults & 0x1000) != 0;
    Channel13BreakerFault = (faults & 0x2000) != 0;
    Channel14BreakerFault = (faults & 0x4000) != 0;
    Channel15BreakerFault = (faults & 0x8000) != 0;
    Channel16BreakerFault = (faults & 0x10000) != 0;
    Channel17BreakerFault = (faults & 0x20000) != 0;
    Channel18BreakerFault = (faults & 0x40000) != 0;
    Channel19BreakerFault = (faults & 0x80000) != 0;
    Channel20BreakerFault = (faults & 0x100000) != 0;
    Channel21BreakerFault = (faults & 0x200000) != 0;
    Channel22BreakerFault = (faults & 0x400000) != 0;
    Channel23BreakerFault = (faults & 0x800000) != 0;
    Brownout = (faults & 0x1000000) != 0;
    CanWarning = (faults & 0x2000000) != 0;
    HardwareFault = (faults & 0x4000000) != 0;
  }
}
