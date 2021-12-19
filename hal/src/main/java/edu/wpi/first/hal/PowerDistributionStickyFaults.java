package edu.wpi.first.hal;

public class PowerDistributionStickyFaults {

  @SuppressWarnings("MemberName")
  public final boolean Channel0BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel1BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel2BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel3BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel4BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel5BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel6BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel7BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel8BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel9BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel10BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel11BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel12BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel13BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel14BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel15BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel16BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel17BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel18BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel19BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel20BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel21BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel22BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Channel23BreakerFault;

  @SuppressWarnings("MemberName")
  public final boolean Brownout;

  @SuppressWarnings("MemberName")
  public final boolean CanWarning;

  @SuppressWarnings("MemberName")
  public final boolean CanBusOff;

  @SuppressWarnings("MemberName")
  public final boolean HasReset;

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
