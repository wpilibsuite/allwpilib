// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.measure;

import edu.wpi.first.units.AccelerationUnit;
import edu.wpi.first.units.MathHelper;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Unit;

public interface Acceleration<D extends Unit> extends Measure<AccelerationUnit<D>> {
  @Override
  Acceleration<D> copy();
  
  MathHelper<AccelerationUnit<D>, Acceleration<D>> getMathHelper();

  default Acceleration<D> plus(Acceleration<D> other) {
    return getMathHelper().add(this, other);
  }

  default Acceleration<D> minus(Acceleration<D> other) {
    return getMathHelper().minus(this, other);
  }

  default Dimensionless divide(Acceleration<D> divisor) {
    return getMathHelper().divide(this, divisor);
  }

  default Acceleration<D> divide(double divisor) {
    return getMathHelper().divide(this, divisor);
  }

  default Acceleration<D> divide(Dimensionless divisor) {
    return getMathHelper().divide(this, divisor);
  }

  default Acceleration<D> times(double multiplier) {
    return getMathHelper().multiply(this, multiplier);
  }

  default Acceleration<D> times(Dimensionless multiplier) {
    return getMathHelper().multiply(this, multiplier);
  }

  default Velocity<D> times(Time time) {
    return getMathHelper().multiply(this, time, baseUnit().numerator()::of);
  }
}
