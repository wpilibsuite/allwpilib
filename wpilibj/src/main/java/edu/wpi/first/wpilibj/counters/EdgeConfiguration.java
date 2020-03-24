package edu.wpi.first.wpilibj.counters;

public enum EdgeConfiguration {
  kNone(false, false),
  kRisingEdge(true, false),
  kFallingEdge(false, true),
  kBoth(true, true);

  @SuppressWarnings("MemberName")
  public final boolean rising;


  @SuppressWarnings("MemberName")
  public final boolean falling;

  EdgeConfiguration(boolean rising, boolean falling) {
    this.rising = rising;
    this.falling = falling;
  }
}
