// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class Power extends Unit<Power> {
  Power(double baseUnitEquivalent, String name, String symbol) {
    super(Power.class, baseUnitEquivalent, name, symbol);
  }

  Power(
      UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super(Power.class, toBaseConverter, fromBaseConverter, name, symbol);
  }
}
