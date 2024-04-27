// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.counter;

/** Edge configuration. */
public enum EdgeConfiguration {
  /** No edge configuration (neither rising nor falling). */
  kNone(false, false),
  /** Rising edge configuration. */
  kRisingEdge(true, false),
  /** Falling edge configuration. */
  kFallingEdge(false, true),
  /** Both rising and falling edge configuration. */
  kBoth(true, true);

  /** True if triggering on rising edge. */
  @SuppressWarnings("MemberName")
  public final boolean rising;

  /** True if triggering on falling edge. */
  @SuppressWarnings("MemberName")
  public final boolean falling;

  EdgeConfiguration(boolean rising, boolean falling) {
    this.rising = rising;
    this.falling = falling;
  }
}
