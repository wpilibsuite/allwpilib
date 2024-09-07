// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Degrees;
import static edu.wpi.first.units.Units.Foot;
import static edu.wpi.first.units.Units.Inches;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;
import static org.junit.jupiter.api.Assertions.assertSame;

import org.junit.jupiter.api.Test;

class PerUnitTest {
  @Test
  void ofNative() {
    ExampleUnit part = new ExampleUnit(1);
    var unit = PerUnit.combine(part, part);
    assertEquals(10, unit.ofNative(10).in(unit));

    // Does not compile:
    // unit.of(10).in(unit);
    //
    // error: incompatible types: PerUnit<ExampleUnit,ExampleUnit> cannot be converted to CAP#1
    //    unit.of(10).in(unit);
    //                   ^
    //  where CAP#1 is a fresh type-variable:
    //    CAP#1 extends PerUnit<ExampleUnit,ExampleUnit>
    //    from capture of ? extends PerUnit<ExampleUnit,ExampleUnit>
    // This is because `of` returns a `Measure<? extends PerUnit<ExampleUnit, ExampleUnit>>`,
    // and a `Per<ExampleUnit, ExampleUnit>` object isn't a subtype of that anonymous wildcard bound
  }

  @Test
  void multSameDenom() {
    var unit = Degrees.per(Foot);
    var result = unit.mult(Foot);

    // Multiplying by the same unit as the divisor should return the dividend unit
    assertSame(Degrees, result);
  }

  @Test
  void multOtherDenom() {
    var unit = Degrees.per(Foot);

    // (Degrees / Foot) x Inches, or 1/12 of a degree
    var result = unit.mult(Inches);
    assertInstanceOf(AngleUnit.class, result);
    assertEquals(1 / 12.0, result.of(1).in(Degrees), 1e-9);
    assertEquals("Degree per Foot Inch", result.name());
    assertEquals("°/ft-in", result.symbol());
  }
}
