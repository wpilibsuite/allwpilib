// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.immutable.ImmutableMass;
import edu.wpi.first.units.measure.Mass;
import edu.wpi.first.units.mutable.MutMass;

/**
 * Unit of mass dimension.
 *
 * <p>This is the base type for units of mass dimension. It is also used to specify the dimension
 * for {@link Measure}: <code>Measure&lt;MassUnit&gt;</code>.
 *
 * <p>Actual units (such as {@link Units#Grams} and {@link Units#Pounds}) can be found in the {@link
 * Units} class.
 */
public class MassUnit extends Unit {
  /** Creates a new unit with the given name and multiplier to the base unit. */
  MassUnit(MassUnit baseUnit, double baseUnitEquivalent, String name, String symbol) {
    super(baseUnit, baseUnitEquivalent, name, symbol);
  }

  MassUnit(
      MassUnit baseUnit,
      UnaryFunction toBaseConverter,
      UnaryFunction fromBaseConverter,
      String name,
      String symbol) {
    super(baseUnit, toBaseConverter, fromBaseConverter, name, symbol);
  }

  @Override
  public MassUnit getBaseUnit() {
    return (MassUnit) super.getBaseUnit();
  }

  @Override
  public Mass of(double magnitude) {
    return new ImmutableMass(magnitude, toBaseUnits(magnitude), this);
  }

  @Override
  public Mass ofBaseUnits(double baseUnitMagnitude) {
    return new ImmutableMass(fromBaseUnits(baseUnitMagnitude), baseUnitMagnitude, this);
  }

  @Override
  public MutMass mutable(double initialMagnitude) {
    return new MutMass(initialMagnitude, toBaseUnits(initialMagnitude), this);
  }

  public VelocityUnit<MassUnit> per(TimeUnit period) {
    return VelocityUnit.combine(this, period);
  }

  public double convertFrom(double magnitude, MassUnit otherUnit) {
    return fromBaseUnits(otherUnit.toBaseUnits(magnitude));
  }

  public LinearMomentumUnit mult(LinearVelocityUnit velocity) {
    return LinearMomentumUnit.combine(this, velocity);
  }

  public ForceUnit mult(LinearAccelerationUnit acceleration) {
    return ForceUnit.combine(this, acceleration);
  }
}
