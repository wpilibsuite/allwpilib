// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class Voltage extends Unit<Voltage> {
  Voltage(double baseUnitEquivalent, String name, String symbol) {
    super(Voltage.class, baseUnitEquivalent, name, symbol);
  }

  Voltage(
      UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super(Voltage.class, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public Power times(Unit<Current> current, String name, String symbol) {
    return new Power(toBaseUnits(1) * current.toBaseUnits(1), name, symbol);
  }
}
