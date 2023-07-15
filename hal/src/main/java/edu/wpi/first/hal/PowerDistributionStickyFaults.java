// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

@SuppressWarnings("MemberName")
public class PowerDistributionStickyFaults {
  public final boolean Channel0BreakerFault;

  public final boolean Channel1BreakerFault;

  public final boolean Channel2BreakerFault;

  public final boolean Channel3BreakerFault;

  public final boolean Channel4BreakerFault;

  public final boolean Channel5BreakerFault;

  public final boolean Channel6BreakerFault;

  public final boolean Channel7BreakerFault;

  public final boolean Channel8BreakerFault;

  public final boolean Channel9BreakerFault;

  public final boolean Channel10BreakerFault;

  public final boolean Channel11BreakerFault;

  public final boolean Channel12BreakerFault;

  public final boolean Channel13BreakerFault;

  public final boolean Channel14BreakerFault;

  public final boolean Channel15BreakerFault;

  public final boolean Channel16BreakerFault;

  public final boolean Channel17BreakerFault;

  public final boolean Channel18BreakerFault;

  public final boolean Channel19BreakerFault;

  public final boolean Channel20BreakerFault;

  public final boolean Channel21BreakerFault;

  public final boolean Channel22BreakerFault;

  public final boolean Channel23BreakerFault;

  public final boolean Brownout;

  public final boolean CanWarning;

  public final boolean CanBusOff;

  public final boolean HasReset;

  /**
   * Constructs from a bitfield.
   *
   * @param faults faults
   */
  public PowerDistributionStickyFaults(int faults) {
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
    CanBusOff = (faults & 0x4000000) != 0;
    HasReset = (faults & 0x8000000) != 0;
  }
}
