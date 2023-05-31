// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotSame;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class MeasureTest {
  @Test
  void testBasics() {
    Unit<Distance> unit = Units.Feet;
    double magnitude = 10;
    Measure<Distance> m = unit.of(magnitude);
    assertEquals(unit, m.unit(), "Wrong units");
    assertEquals(magnitude, m.magnitude(), 0, "Wrong magnitude");
  }

  @Test
  void testMultiply() {
    Measure<Distance> m = Measure.of(1, Units.Feet);
    Measure<Distance> m2 = m.times(10);
    assertEquals(10, m2.magnitude(), 1e-12);
    assertNotSame(m2, m); // make sure state wasn't changed
  }

  @Test
  void testDivide() {
    Measure<Distance> m = Measure.of(1, Units.Meters);
    Measure<Distance> m2 = m.divide(10);
    assertEquals(0.1, m2.magnitude(), 0);
    assertNotSame(m2, m);
  }

  @Test
  void testAdd() {
    Measure<Distance> m1 = Measure.of(1, Units.Feet);
    Measure<Distance> m2 = Measure.of(2, Units.Inches);
    assertTrue(m1.add(m2).isEquivalent(Measure.of(1 + 2 / 12d, Units.Feet)));
    assertTrue(m2.add(m1).isEquivalent(Measure.of(14, Units.Inches)));
  }

  @Test
  void testSubtract() {
    Measure<Distance> m1 = Measure.of(1, Units.Feet);
    Measure<Distance> m2 = Measure.of(2, Units.Inches);
    assertTrue(m1.subtract(m2).isEquivalent(Measure.of(1 - 2 / 12d, Units.Feet)));
    assertTrue(m2.subtract(m1).isEquivalent(Measure.of(-10, Units.Inches)));
  }

  @Test
  void testNegate() {
    Measure<Distance> m = Measure.of(123, Units.Feet);
    Measure<Distance> n = m.negate();
    assertEquals(-m.magnitude(), n.magnitude(), 1e-12);
    assertEquals(m.unit(), n.unit());
  }

  @Test
  void testEquivalency() {
    Measure<Distance> inches = Measure.of(12, Units.Inches);
    Measure<Distance> feet = Measure.of(1, Units.Feet);
    assertTrue(inches.isEquivalent(feet));
    assertTrue(feet.isEquivalent(inches));
  }

  @Test
  void testAs() {
    Measure<Distance> m = Measure.of(12, Units.Inches);
    assertEquals(1, m.in(Units.Feet), Measure.EQUIVALENCE_THRESHOLD);
  }

  @Test
  void testPerUnitTime() {
    var measure = Measure.of(144, Units.Kilograms);
    var dt = Measure.of(53, Units.Milliseconds);

    // 144 Kg / (53 ms) = (1000 / 53) * 144 Kg/s = (144,000 / 53) Kg/s

    var result = measure.per(dt);
    assertEquals(144_000.0 / 53, result.baseUnitMagnitude(), 1e-5);
    assertEquals(Units.Kilograms.per(Units.Milliseconds), result.unit());
  }

  @Test
  void testTimesMeasure() {
    var m1 = Units.Volts.of(1.567);
    var m2 = Units.Kilograms.of(8.4e-5);

    assertEquals(Units.Volts.mult(Units.Kilograms).of(1.567 * 8.4e-5), m1.times(m2));
  }

  @Test
  void testTimesUnitless() {
    var unit = new ExampleUnit(6);
    var measure = Measure.of(2.5, unit);
    var multiplier = Units.Percent.of(125); // 125% or 1.25x
    Measure<?> result = measure.times(multiplier);
    assertSame(unit, result.unit());

    assertEquals(2.5 * 1.25, result.magnitude());
    assertEquals(2.5 * 1.25 * 6, result.baseUnitMagnitude());
  }

  @Test
  void testTimesPerWithDimensionalAnalysis() {
    var measureA = Units.Feet.of(62); // 62 feet
    var measureB = Units.Radians.of(6).per(Units.Inches); // 6 radians per inch
    Measure<?> aTimesB = measureA.times(measureB); // (62 feet) * (6 rad/inch) = 4464 rad
    assertEquals(Units.Radians, aTimesB.unit());
    assertEquals(4464, aTimesB.magnitude(), 1e-12);

    Measure<?> bTimesA = measureB.times(measureA); // should be identical to the above
    assertTrue(bTimesA.isEquivalent(aTimesB));
    assertTrue(aTimesB.isEquivalent(bTimesA));
  }

  @Test
  void testPerTimesPerWithDimensionalAnalysis() {
    var measureA = Units.Inches.of(16).per(Units.Volts);
    var measureB = Units.Millivolts.of(14).per(Units.Meters);
    Measure<?> aTimesB = measureA.times(measureB);
    assertEquals(Units.Value, aTimesB.unit());
    assertEquals((16 * 25.4 / 1000) * (14 / 1000.0), aTimesB.magnitude());
    assertEquals((16 * 25.4 / 1000) * (14 / 1000.0), aTimesB.baseUnitMagnitude());

    Measure<?> bTimesA = measureB.times(measureA); // should be identical to the above
    assertTrue(bTimesA.isEquivalent(aTimesB));
    assertTrue(aTimesB.isEquivalent(bTimesA));
  }

  @Test
  void testPerTimesMeasure() {
    var m1 = Units.Feet.per(Units.Milliseconds).of(19);
    var m2 = Units.Seconds.of(44);

    // 19 ft/ms = 19,000 ft/s
    // 19,000 ft/s * 44s = 836,000 ft
    assertEquals(Units.Feet.of(836_000), m1.times(m2));

    // 42 ex per foot * 17mm = 42 ex * 17mm / (304.8mm/ft) = 42 * 17 / 304.8 = 2.34252
    var exampleUnit = new ExampleUnit(1);
    var m3 = exampleUnit.per(Units.Feet).of(42);
    var m4 = Units.Millimeters.of(17);
    assertEquals(exampleUnit.of(42 * 17 / (12 * 25.4)), m3.times(m4));
  }

  @Test
  void testToShortString() {
    var measure = Measure.of(343, Units.Volts);
    assertEquals("3.430e+02 V", measure.toShortString());
  }

  @Test
  void testToLongString() {
    var measure = Measure.of(343, Units.Volts);
    assertEquals("343.0 Volt", measure.toLongString());
    assertEquals("343.0001 Volt", Measure.of(343.0001, Units.Volts).toLongString());
    assertEquals(
        "1.2345678912345679E8 Volt", Measure.of(123456789.123456789, Units.Volts).toLongString());
  }

  @Test
  void testOfBaseUnits() {
    var unit = new ExampleUnit(16);
    var measure = Measure.ofBaseUnits(1, unit);
    assertEquals(unit, measure.unit());
    assertEquals(1, measure.baseUnitMagnitude());
    assertEquals(1 / 16.0, measure.magnitude());
  }

  @Test
  void testCompare() {
    var unit = new ExampleUnit(7);
    var base = Measure.of(1, unit);
    var less = Measure.of(0.5, unit);
    var more = Measure.of(2, unit);

    assertEquals(0, base.compareTo(base));
    assertEquals(-1, base.compareTo(more));
    assertEquals(1, base.compareTo(less));

    // lt, lte, gt, gte helper functions

    assertTrue(base.lt(more));
    assertTrue(base.lte(more));
    assertFalse(base.gt(more));
    assertFalse(base.gte(more));

    assertTrue(base.gt(less));
    assertTrue(base.gte(less));
    assertFalse(base.lt(less));
    assertFalse(base.lte(less));

    assertTrue(base.lte(base));
    assertTrue(base.gte(base));
    assertFalse(base.lt(base));
    assertFalse(base.gt(base));
  }

  @Test
  void testTimesScalar() {
    var unit = new ExampleUnit(42);
    var measure = Measure.of(4.2, unit);
    var scalar = 18;
    var result = measure.times(scalar);
    assertNotSame(measure, result);
    assertSame(unit, result.unit());
    assertEquals(4.2 * 18, result.magnitude());
    assertEquals(4.2 * 42 * 18, result.baseUnitMagnitude());
  }

  @Test
  void testPerUnit() {
    var unitA = new ExampleUnit(10);
    var unitB = new ExampleUnit(12);
    var measure = Measure.of(1.2, unitA);
    var result = measure.per(unitB);
    assertEquals(Per.combine(unitA, unitB), result.unit()); // A/B has base equivalent of 10/12
    assertEquals(1, result.baseUnitMagnitude()); // 10/12 * 12/10 = 1
    assertEquals(measure.magnitude(), result.magnitude());
  }

  @Test
  void testAddMeasureSameUnit() {
    var unit = new ExampleUnit(8.2);
    var measureA = Measure.of(3.1, unit);
    var measureB = Measure.of(91.6, unit);
    var result = measureA.add(measureB);
    assertEquals(unit, result.unit());
    assertEquals(94.7, result.magnitude(), 1e-12);
  }

  @Test
  void testAddMeasuresDifferentUnits() {
    var unitA = new ExampleUnit(8.2);
    var unitB = new ExampleUnit(7.3);
    var measureA = Measure.of(5, unitA);
    var measureB = Measure.of(16, unitB);
    var aPlusB = measureA.add(measureB);

    assertEquals(unitA, aPlusB.unit());
    assertEquals(8.2 * 5 + 7.3 * 16, aPlusB.baseUnitMagnitude(), 1e-12);
    assertEquals(5 + (16 * 7.3 / 8.2), aPlusB.magnitude(), 1e-12);

    var bPlusA = measureB.add(measureA);
    assertEquals(unitB, bPlusA.unit());
    assertEquals(8.2 * 5 + 7.3 * 16, bPlusA.baseUnitMagnitude(), 1e-12);
    assertEquals(16 + (5 * 8.2 / 7.3), bPlusA.magnitude(), 1e-12);
  }

  @Test
  void testMinNoArgs() {
    var min = Measure.min();
    assertNull(min);
  }

  @Test
  void testMin() {
    var unit = new ExampleUnit(56.1);
    var one = Measure.of(1, unit);
    var two = Measure.of(2, unit);
    var zero = Measure.of(0, unit);
    var veryNegative = Measure.of(-12839712, unit);

    var min = Measure.min(one, two, zero, veryNegative);
    assertSame(veryNegative, min);
  }

  @Test
  void testMaxNoArgs() {
    var min = Measure.max();
    assertNull(min);
  }

  @Test
  void testMax() {
    var unit = new ExampleUnit(6.551);
    var one = Measure.of(1, unit);
    var two = Measure.of(2, unit);
    var zero = Measure.of(0, unit);
    var veryLarge = Measure.of(8217234, unit);

    var max = Measure.max(one, two, zero, veryLarge);
    assertSame(veryLarge, max);
  }

  @Test
  void testIsNear() {
    var unit = new ExampleUnit(92);
    var measureA = Measure.of(1.21, unit);
    var measureB = Measure.ofBaseUnits(64, unit);
    // A = 1.21 * 92 base units, or 111.32
    // B = 64 base units
    // ratio = 111.32 / 64 = 1.739375 = 173.9375%

    assertTrue(measureA.isNear(measureA, 0));
    assertTrue(measureB.isNear(measureB, 0));

    assertFalse(measureA.isNear(measureB, 0));
    assertFalse(measureA.isNear(measureB, 0.50));
    assertFalse(measureA.isNear(measureB, 0.739370));
    assertTrue(measureA.isNear(measureB, 0.739375));
    assertTrue(measureA.isNear(measureB, 100)); // some stupidly large range +/- 10000%
  }
}
