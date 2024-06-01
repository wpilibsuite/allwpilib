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
    Measure<Distance> m = Units.Feet.of(1);
    Measure<Distance> m2 = m.times(10);
    assertEquals(10, m2.magnitude(), 1e-12);
    assertNotSame(m2, m); // make sure state wasn't changed
  }

  @Test
  void testDivide() {
    Measure<Distance> m = Units.Meters.of(1);
    Measure<Distance> m2 = m.divide(10);
    assertEquals(0.1, m2.magnitude(), 0);
    assertNotSame(m2, m);
  }

  @Test
  void testAdd() {
    Measure<Distance> m1 = Units.Feet.of(1);
    Measure<Distance> m2 = Units.Inches.of(2);
    assertTrue(m1.plus(m2).isEquivalent(Units.Feet.of(1 + 2 / 12d)));
    assertTrue(m2.plus(m1).isEquivalent(Units.Inches.of(14)));
  }

  @Test
  void testSubtract() {
    Measure<Distance> m1 = Units.Feet.of(1);
    Measure<Distance> m2 = Units.Inches.of(2);
    assertTrue(m1.minus(m2).isEquivalent(Units.Feet.of(1 - 2 / 12d)));
    assertTrue(m2.minus(m1).isEquivalent(Units.Inches.of(-10)));
  }

  @Test
  void testNegate() {
    Measure<Distance> m = Units.Feet.of(123);
    Measure<Distance> n = m.negate();
    assertEquals(-m.magnitude(), n.magnitude(), 1e-12);
    assertEquals(m.unit(), n.unit());
  }

  @Test
  void testEquivalency() {
    Measure<Distance> inches = Units.Inches.of(12);
    Measure<Distance> feet = Units.Feet.of(1);
    assertTrue(inches.isEquivalent(feet));
    assertTrue(feet.isEquivalent(inches));
  }

  @Test
  void testAs() {
    Measure<Distance> m = Units.Inches.of(12);
    assertEquals(1, m.in(Units.Feet), Measure.EQUIVALENCE_THRESHOLD);
  }

  @Test
  void testPerMeasureTime() {
    var measure = Units.Kilograms.of(144);
    var dt = Units.Milliseconds.of(53);

    // 144 Kg / (53 ms) = (1000 / 53) * 144 Kg/s = (144,000 / 53) Kg/s

    var result = measure.per(dt);
    assertEquals(144_000.0 / 53, result.baseUnitMagnitude(), 1e-5);
    assertEquals(Units.Kilograms.per(Units.Milliseconds), result.unit());
  }

  @Test
  void testPerUnitTime() {
    var measure = Units.Kilograms.of(144);
    var result = measure.per(Units.Millisecond);

    assertEquals(Velocity.class, result.unit().getClass());
    assertEquals(144_000.0, result.baseUnitMagnitude(), 1e-5);
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
    var measure = unit.of(2.5);
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
    assertTrue(Units.Feet.of(836_000).isNear(m1.times(m2), 1e-12));

    // 42 ex per foot * 17mm = 42 ex * 17mm / (304.8mm/ft) = 42 * 17 / 304.8 = 2.34252
    var exampleUnit = new ExampleUnit(1);
    var m3 = exampleUnit.per(Units.Feet).of(42);
    var m4 = Units.Millimeters.of(17);
    assertEquals(exampleUnit.of(42 * 17 / (12 * 25.4)), m3.times(m4));
  }

  @Test
  void testDivideMeasure() {
    // Dimensionless divide
    var m1 = Units.Meters.of(6);
    var m2 = Units.Value.of(3);
    var result = m1.divide(m2);
    assertEquals(m1.divide(m2).magnitude(), 2);
    assertEquals(result.unit(), Units.Meters);
    // Velocity divide
    var m3 = Units.Meters.of(8);
    var m4 = Units.Meters.per(Units.Second).of(4);
    result = m3.divide(m4);
    assertEquals(result.magnitude(), 2);
    assertEquals(result.unit(), Units.Second);
    // Per divide
    var m5 = Units.Volts.of(6);
    var m6 = Units.Volts.per(Units.Meter).of(2);
    result = m5.divide(m6);
    assertEquals(result.magnitude(), 3);
    assertEquals(result.unit(), Units.Meter);
    // Fallthrough divide
    var m7 = Units.Seconds.of(10);
    var m8 = Units.Amps.of(2);
    result = m7.divide(m8);
    assertEquals(result.magnitude(), 5);
    assertEquals(result.unit(), Units.Seconds.per(Units.Amps));
    // Same base unit divide
    var m9 = Units.Meters.of(8);
    var m10 = Units.Meters.of(4);
    result = m9.divide(m10);
    assertEquals(result.magnitude(), 2);
    assertEquals(result.unit(), Units.Value);
  }

  @Test
  void testToShortString() {
    var measure = Units.Volts.of(343);
    assertEquals("3.430e+02 V", measure.toShortString());
  }

  @Test
  void testToLongString() {
    var measure = Units.Volts.of(343);
    assertEquals("343.0 Volt", measure.toLongString());
    assertEquals("343.0001 Volt", Units.Volts.of(343.0001).toLongString());
    assertEquals("1.2345678912345678E8 Volt", Units.Volts.of(123456789.12345678).toLongString());
  }

  @Test
  void testOfBaseUnits() {
    var unit = new ExampleUnit(16);
    var measure = unit.ofBaseUnits(1);
    assertEquals(unit, measure.unit());
    assertEquals(1, measure.baseUnitMagnitude());
    assertEquals(1 / 16.0, measure.magnitude());
  }

  @Test
  @SuppressWarnings("SelfComparison")
  void testCompare() {
    var unit = new ExampleUnit(7);
    var base = unit.of(1);
    var less = unit.of(0.5);
    var more = unit.of(2);

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
    var measure = unit.of(4.2);
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
    var measure = unitA.of(1.2);
    var result = measure.per(unitB);
    assertEquals(Per.combine(unitA, unitB), result.unit()); // A/B has base equivalent of 10/12
    assertEquals(1, result.baseUnitMagnitude()); // 10/12 * 12/10 = 1
    assertEquals(measure.magnitude(), result.magnitude());
  }

  @Test
  void testAddMeasureSameUnit() {
    var unit = new ExampleUnit(8.2);
    var measureA = unit.of(3.1);
    var measureB = unit.of(91.6);
    var result = measureA.plus(measureB);
    assertEquals(unit, result.unit());
    assertEquals(94.7, result.magnitude(), 1e-12);
  }

  @Test
  void testAddMeasuresDifferentUnits() {
    var unitA = new ExampleUnit(8.2);
    var unitB = new ExampleUnit(7.3);
    var measureA = unitA.of(5);
    var measureB = unitB.of(16);
    var aPlusB = measureA.plus(measureB);

    assertEquals(unitA, aPlusB.unit());
    assertEquals(8.2 * 5 + 7.3 * 16, aPlusB.baseUnitMagnitude(), 1e-12);
    assertEquals(5 + (16 * 7.3 / 8.2), aPlusB.magnitude(), 1e-12);

    var bPlusA = measureB.plus(measureA);
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
    var one = unit.of(1);
    var two = unit.of(2);
    var zero = unit.of(0);
    var veryNegative = unit.of(-12839712);

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
    var one = unit.of(1);
    var two = unit.of(2);
    var zero = unit.of(0);
    var veryLarge = unit.of(8217234);

    var max = Measure.max(one, two, zero, veryLarge);
    assertSame(veryLarge, max);
  }

  @Test
  void testIsNearVarianceThreshold() {
    var unit = new ExampleUnit(92);
    var measureA = unit.of(1.21);
    var measureB = unit.ofBaseUnits(64);
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

    var measureC = unit.of(-1.21);
    var measureD = unit.ofBaseUnits(-64);

    assertTrue(measureC.isNear(measureC, 0));
    assertTrue(measureD.isNear(measureD, 0));

    assertFalse(measureC.isNear(measureD, 0));
    assertFalse(measureC.isNear(measureD, 0.50));
    assertFalse(measureC.isNear(measureD, 0.739370));
    assertTrue(measureC.isNear(measureD, 0.739375));
    assertTrue(measureC.isNear(measureD, 100)); // some stupidly large range +/- 10000%

    var measureE = Units.Meters.of(1);
    var measureF = Units.Feet.of(-3.28084);

    assertTrue(measureE.isNear(measureF, 2.01));
    assertFalse(measureE.isNear(measureF, 1.99));

    assertTrue(measureF.isNear(measureE, 2.01));
    assertFalse(measureF.isNear(measureE, 1.99));

    assertTrue(Units.Feet.zero().isNear(Units.Millimeters.zero(), 0.001));
    assertFalse(Units.Feet.of(2).isNear(Units.Millimeters.of(0), 0.001));
  }

  @Test
  void testIsNearMeasureTolerance() {
    var measureCompared = Units.Meters.of(1);
    var measureComparing = Units.Meters.of(1.2);

    // Positive value with positive tolerance
    assertTrue(measureCompared.isNear(measureComparing, Units.Millimeters.of(300)));
    assertFalse(measureCompared.isNear(measureComparing, Units.Centimeters.of(10)));

    measureCompared = measureCompared.negate();
    measureComparing = measureComparing.negate();

    // Negative value with positive tolerance
    assertTrue(measureCompared.isNear(measureComparing, Units.Millimeters.of(300)));
    assertFalse(measureCompared.isNear(measureComparing, Units.Centimeters.of(10)));

    measureCompared = measureCompared.negate();
    measureComparing = measureComparing.negate();

    // Positive value with negative tolerance
    assertTrue(measureCompared.isNear(measureComparing, Units.Millimeters.of(-300)));
    assertFalse(measureCompared.isNear(measureComparing, Units.Centimeters.of(-10)));

    measureCompared = measureCompared.negate();
    measureComparing = measureComparing.negate();

    // Negative value with negative tolerance.
    assertTrue(measureCompared.isNear(measureComparing, Units.Millimeters.of(-300)));
    assertFalse(measureCompared.isNear(measureComparing, Units.Centimeters.of(-10)));

    measureCompared = measureCompared.negate();
    measureComparing = measureComparing.negate();

    // Tolerance exact difference between measures.
    assertTrue(measureCompared.isNear(measureComparing, Units.Millimeters.of(200)));
    assertTrue(measureCompared.isNear(measureComparing, Units.Centimeters.of(-20)));
  }
}
