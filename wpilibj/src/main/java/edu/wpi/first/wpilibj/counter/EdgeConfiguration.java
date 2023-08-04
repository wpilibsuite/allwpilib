// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.counter;

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
