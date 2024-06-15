// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

public record Temperature(double magnitude, double baseUnitMagnitude, TemperatureUnit unit)
    implements Measure<TemperatureUnit> {
  @Override
  public Temperature copy() {
    return this;
  }

  public Velocity<TemperatureUnit> per(Time period) {
    return unit.per(period.unit()).of(this.magnitude / period.magnitude());
  }

  public Velocity<TemperatureUnit> per(TimeUnit period) {
    return unit.per(period).of(this.magnitude / period.fromBaseUnits(1));
  }
}
