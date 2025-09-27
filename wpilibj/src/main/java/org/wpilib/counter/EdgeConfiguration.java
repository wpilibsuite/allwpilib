// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.counter;

/** Edge configuration. */
public enum EdgeConfiguration {
  /** Rising edge configuration. */
  kRisingEdge(true),
  /** Falling edge configuration. */
  kFallingEdge(false);

  /** True if triggering on rising edge. */
  @SuppressWarnings("MemberName")
  public final boolean rising;

  EdgeConfiguration(boolean rising) {
    this.rising = rising;
  }
}
