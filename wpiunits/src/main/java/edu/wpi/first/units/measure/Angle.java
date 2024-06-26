// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.measure;

import static edu.wpi.first.units.Units.Radians;
import static edu.wpi.first.units.Units.RadiansPerSecond;
import static edu.wpi.first.units.Units.Value;

import edu.wpi.first.units.AngleUnit;
import edu.wpi.first.units.ImmutableMeasure;
import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.PerUnit;
import edu.wpi.first.units.TimeUnit;
import edu.wpi.first.units.Unit;

public interface Angle extends Measure<AngleUnit> {
  MathHelper<AngleUnit, Angle> mathHelper = new MathHelper<>(Radians::of);

  default Dimensionless divide(Angle other) {
    return mathHelper.divide(this, other);
  }

  default Angle plus(Angle other) {
    return mathHelper.add(this, other);
  }

  default Angle minus(Angle other) {
    return mathHelper.minus(this, other);
  }

  default Angle divide(double divisor) {
    return mathHelper.divide(this, divisor);
  }

  default AngularVelocity divide(Time period) {
    return mathHelper.divide(this, period, RadiansPerSecond::of);
  }

  default <Other extends Unit> Measure<Other> divide(
      Measure<? extends PerUnit<AngleUnit, ? extends Other>> ratio) {
    return ImmutableMeasure.ofBaseUnits(
        baseUnitMagnitude() / ratio.baseUnitMagnitude(), ratio.unit().denominator());
  }

  default AngularVelocity per(TimeUnit unit) {
    return mathHelper.divide(this, unit.of(1), RadiansPerSecond::of);
  }

  default Dimensionless per(AngleUnit unit) {
    return mathHelper.divide(this, unit.of(1), Value::of);
  }

  @SuppressWarnings("unchecked")
  default <Divisor extends Unit> Measure<? extends PerUnit<AngleUnit, Divisor>> per(
      Divisor divisor) {
    return PerUnit.combine(this.baseUnit(), (Divisor) divisor.getBaseUnit())
        .of(baseUnitMagnitude() / divisor.fromBaseUnits(1));
  }

  default <Other extends Unit> Measure<Other> times(
      Measure<? extends PerUnit<? extends Other, AngleUnit>> ratio) {
    return ImmutableMeasure.ofBaseUnits(
        baseUnitMagnitude() * ratio.baseUnitMagnitude(), ratio.unit().numerator());
  }
}
