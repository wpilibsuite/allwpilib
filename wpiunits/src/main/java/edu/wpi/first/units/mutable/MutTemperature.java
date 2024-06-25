// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.mutable;

import edu.wpi.first.units.MutableMeasure;
import edu.wpi.first.units.Temperature;
import edu.wpi.first.units.TemperatureUnit;

public class MutTemperature extends Temperature
    implements MutableMeasure<TemperatureUnit, Temperature, MutTemperature> {
  protected MutTemperature(double magnitude, double baseUnitMagnitude, TemperatureUnit unit) {
    super(magnitude, baseUnitMagnitude, unit);
  }

  @Override
  public MutTemperature mut_replace(double magnitude, TemperatureUnit newUnit) {
    this.unit = newUnit;
    this.magnitude = magnitude;
    this.baseUnitMagnitude = unit.toBaseUnits(magnitude);

    return this;
  }

  @Override
  public Temperature copy() {
    return new Temperature(magnitude, baseUnitMagnitude, unit);
  }
}
