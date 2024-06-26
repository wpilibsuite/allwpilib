// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public interface Mult<A extends Unit, B extends Unit> extends Measure<MultUnit<A, B>> {
  @Override
  Measure<MultUnit<A, B>> copy();
}
