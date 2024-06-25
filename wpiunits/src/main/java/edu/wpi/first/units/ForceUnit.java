// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.mutable.MutForce;

public class ForceUnit extends MultUnit<MassUnit, LinearAccelerationUnit> {
  private static final CombinatoryUnitCache<MassUnit, LinearAccelerationUnit, ForceUnit> cache =
      new CombinatoryUnitCache<>(ForceUnit::new);

  protected ForceUnit(MassUnit mass, LinearAccelerationUnit acceleration) {
    this(
        mass.isBaseUnit() && acceleration.isBaseUnit()
            ? null
            : combine(mass.getBaseUnit(), acceleration.getBaseUnit()),
        mass.getConverterToBase().div(acceleration.getConverterToBase()),
        mass.getConverterFromBase().div(acceleration.getConverterFromBase()),
        mass.name() + "-" + acceleration.name(),
        mass.symbol() + "-" + acceleration.symbol());
  }

  ForceUnit(
      ForceUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  public static ForceUnit combine(MassUnit mass, LinearAccelerationUnit acceleration) {
    return cache.combine(mass, acceleration);
  }

  @Override
  public ForceUnit getBaseUnit() {
    return (ForceUnit) super.getBaseUnit();
  }

  // force times distance = energy
  // distance times force = torque
  public EnergyUnit mult(DistanceUnit distance) {
    // TODO
    throw new UnsupportedOperationException();
  }

  @Override
  public Force of(double magnitude) {
    return new Force(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Force ofBaseUnits(double baseUnitMagnitude) {
    return new Force(toBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public MutForce mutable(double initialMagnitude) {
    return new MutForce(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }
}
