// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units.measure;

import static edu.wpi.first.units.Units.Hertz;
import static edu.wpi.first.units.Units.Value;

import edu.wpi.first.units.DimensionlessUnit;
import edu.wpi.first.units.Measure;
import edu.wpi.first.units.PerUnit;
import edu.wpi.first.units.TimeUnit;
import edu.wpi.first.units.Unit;

public interface Ratio<N extends Unit, D extends Unit> extends Measure<PerUnit<N, D>> {
  @Override
  Ratio<N, D> copy();

  @SuppressWarnings("unchecked")
  default Measure<? extends N> times(Measure<? extends D> divisor) {
    return (Measure<? extends N>)
        baseUnit().numerator().ofBaseUnits(baseUnitMagnitude() * divisor.baseUnitMagnitude());
  }

  @SuppressWarnings("unchecked")
  default Measure<? extends PerUnit<DimensionlessUnit, D>> divide(Measure<? extends N> dividend) {
    if (unit().denominator() instanceof TimeUnit timeUnit) {
      // Divisor is time; return frequency
      return (Measure<? extends PerUnit<DimensionlessUnit, D>>)
          Hertz.ofBaseUnits(baseUnitMagnitude() / dividend.baseUnitMagnitude());
    } else {
      // Generic
      return PerUnit.combine(Value, unit().denominator())
          .ofBaseUnits(baseUnitMagnitude() / dividend.baseUnitMagnitude());
    }
  }
}
