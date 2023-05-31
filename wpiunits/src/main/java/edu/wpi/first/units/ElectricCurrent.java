// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public class ElectricCurrent extends Unit<ElectricCurrent> {
  ElectricCurrent(double baseUnitEquivalent, String name, String symbol) {
    super(ElectricCurrent.class, baseUnitEquivalent, name, symbol);
  }

  ElectricCurrent(
      UnaryFunction toBaseConverter, UnaryFunction fromBaseConverter, String name, String symbol) {
    super(ElectricCurrent.class, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public Power times(Unit<ElectricPotential> voltage, String name, String symbol) {
    return new Power(this.toBaseUnits(1) * voltage.toBaseUnits(1), name, symbol);
  }
}
