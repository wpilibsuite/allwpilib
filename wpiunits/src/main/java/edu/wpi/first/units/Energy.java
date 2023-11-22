// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class Energy extends Unit<Energy> {
  protected Energy(
      UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super(Energy.class, toBaseConverter, fromBaseConverter, name, symbol);
  }

  protected Energy(double baseUnitEquivalent, String name, String symbol) {
    super(Energy.class, baseUnitEquivalent, name, symbol);
  }
}
