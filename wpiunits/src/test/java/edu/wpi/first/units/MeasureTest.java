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

import edu.wpi.first.units.measure.Angle;
import edu.wpi.first.units.measure.AngularMomentum;
import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.LinearVelocity;
import edu.wpi.first.units.measure.Per;
import edu.wpi.first.units.measure.Time;
import org.junit.jupiter.api.Test;

class MeasureTest {
  @Test
  void testBasics() {
    DistanceUnit unit = Units.Feet;
    double magnitude = 10;
    Distance m = unit.of(magnitude);
    assertEquals(unit, m.unit(), "Wrong units");
    assertEquals(magnitude, m.magnitude(), 0, "Wrong magnitude");
  }

  @Test
  void testMultiply() {
    Distance m = Units.Feet.of(1);
    Distance m2 = m.times(10);
    assertEquals(10, m2.in(Units.Feet), 1e-12);
    assertNotSame(m2, m); // make sure state wasn't changed
  }

  @Test
  void testTimesConversionFactor() {
    Distance m = Units.Feet.of(10);

    Per<AngleUnit, DistanceUnit> conversion = Units.Degrees.of(10).div(Units.Feet.of(1));
    Angle result = m.timesConversionFactor(conversion);
    assertEquals(Units.Degrees.of(100), result);
  }

  @Test
  void testTimesConversionFactorComplex() {
    Distance m = Units.Feet.of(1);

    // Using a complex compound unit here
    // (Per<Mult<Mult<Mass, Per<Distance, Time>>, Distance>, Distance>)
    Per<AngularMomentumUnit, DistanceUnit> conversion =
        Units.KilogramMetersSquaredPerSecond.of(1).div(Units.Foot.one());

    AngularMomentum result = m.timesConversionFactor(conversion);
    assertEquals(Units.KilogramMetersSquaredPerSecond.of(1), result);
  }

  @Test
  void testTimesVelocityConversionFactor() {
    Time m = Units.Seconds.of(10);

    LinearVelocity conversion = Units.MetersPerSecond.of(10);
    Distance result = m.timesConversionFactor(conversion);
    assertEquals(Units.Meters.of(100), result);
  }

  @Test
  void testDivide() {
    Distance m = Units.Meters.of(1);
    Distance m2 = m.div(10);
    assertEquals(0.1, m2.magnitude(), 0);
    assertNotSame(m2, m);
  }

  @Test
  void testAdd() {
    Distance m1 = Units.Feet.of(1);
    Distance m2 = Units.Inches.of(2);
    assertTrue(m1.plus(m2).isEquivalent(Units.Feet.of(1 + 2 / 12d)));
    assertTrue(m2.plus(m1).isEquivalent(Units.Inches.of(14)));
  }

  @Test
  void testSubtract() {
    Distance m1 = Units.Feet.of(1);
    Distance m2 = Units.Inches.of(2);
    assertTrue(m1.minus(m2).isEquivalent(Units.Feet.of(1 - 2 / 12d)));
    assertTrue(m2.minus(m1).isEquivalent(Units.Inches.of(-10)));
  }

  @Test
  void testUnaryMinus() {
    Distance m = Units.Feet.of(123);
    Distance negated = m.unaryMinus();
    assertEquals(-123, negated.in(Units.Feet), 1e-12);
    assertEquals(Units.Feet, negated.unit());
  }

  @Test
  void testEquivalency() {
    Distance inches = Units.Inches.of(12);
    Distance feet = Units.Feet.of(1);
    assertTrue(inches.isEquivalent(feet));
    assertTrue(feet.isEquivalent(inches));
  }

  @Test
  void testAs() {
    Distance m = Units.Inches.of(12);
    assertEquals(1, m.in(Units.Feet), Measure.EQUIVALENCE_THRESHOLD);
  }

  @Test
  void testPerMeasureTime() {
    var measure = Units.Kilograms.of(144);
    var dt = Units.Milliseconds.of(53);

    // 144 Kg / (53 ms) = (1000 / 53) * 144 Kg/s = (144,000 / 53) Kg/s

    var result = measure.div(dt);
    assertEquals(144_000.0 / 53, result.baseUnitMagnitude(), 1e-5);
    assertEquals(Units.Kilograms.per(Units.Milliseconds), result.unit());
  }

  @Test
  void testPerUnitTime() {
    var measure = Units.Kilograms.of(144);
    var result = measure.per(Units.Millisecond);

    assertEquals(VelocityUnit.class, result.unit().getClass());
    assertEquals(144_000.0, result.baseUnitMagnitude(), 1e-5);
    assertEquals(Units.Kilograms.per(Units.Milliseconds), result.unit());
  }

  @Test
  void testDivideMeasure() {
    // Dimensionless divide
    var m1 = Units.Meters.of(6);
    var m2 = Units.Value.of(3);
    var result = m1.div(m2);
    assertEquals(2, m1.div(m2).magnitude());
    assertEquals(Units.Meters, result.unit());
    // Velocity divide
    var m3 = Units.Meters.of(8);
    var m4 = Units.Meters.per(Units.Second).of(4);
    var time = m3.div(m4);
    assertEquals(2, time.magnitude());
    assertEquals(Units.Second, time.unit());
    // PerUnit divide
    var m5 = Units.Volts.of(6);
    var m6 = Units.Volts.per(Units.Meter).of(2);

    // Voltage/(Voltage/Distance) -> Voltage * Distance/Voltage -> Distance
    var dist = m5.div(m6);
    assertEquals(3, dist.magnitude());
    assertEquals(Units.Meter, dist.unit());
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

    measureCompared = measureCompared.unaryMinus();
    measureComparing = measureComparing.unaryMinus();

    // Negative value with positive tolerance
    assertTrue(measureCompared.isNear(measureComparing, Units.Millimeters.of(300)));
    assertFalse(measureCompared.isNear(measureComparing, Units.Centimeters.of(10)));

    measureCompared = measureCompared.unaryMinus();
    measureComparing = measureComparing.unaryMinus();

    // Positive value with negative tolerance
    assertTrue(measureCompared.isNear(measureComparing, Units.Millimeters.of(-300)));
    assertFalse(measureCompared.isNear(measureComparing, Units.Centimeters.of(-10)));

    measureCompared = measureCompared.unaryMinus();
    measureComparing = measureComparing.unaryMinus();

    // Negative value with negative tolerance.
    assertTrue(measureCompared.isNear(measureComparing, Units.Millimeters.of(-300)));
    assertFalse(measureCompared.isNear(measureComparing, Units.Centimeters.of(-10)));

    measureCompared = measureCompared.unaryMinus();
    measureComparing = measureComparing.unaryMinus();

    // Tolerance exact difference between measures.
    assertTrue(measureCompared.isNear(measureComparing, Units.Millimeters.of(200)));
    assertTrue(measureCompared.isNear(measureComparing, Units.Centimeters.of(-20)));
  }
}
