// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static edu.wpi.first.units.Units.Centimeters;
import static edu.wpi.first.units.Units.FeetPerSecond;
import static edu.wpi.first.units.Units.InchesPerSecond;
import static edu.wpi.first.units.Units.Meters;
import static edu.wpi.first.units.Units.Millisecond;
import static edu.wpi.first.units.Units.Millivolts;
import static edu.wpi.first.units.Units.Second;
import static edu.wpi.first.units.Units.Volts;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;

import org.junit.jupiter.api.Test;

class MutableMeasureTest {
  @Test
  void testWrapper() {
    var unit = new ExampleUnit(1);
    var measure = unit.of(1234);
    var mutable = MutableMeasure.mutable(measure);
    assertEquals(measure.magnitude(), mutable.magnitude(), 0);
    assertEquals(measure.unit(), mutable.unit());
  }

  @Test
  void testSetMagnitude() {
    var measure = MutableMeasure.ofRelativeUnits(0, FeetPerSecond);
    double newMagnitude = 48.123;
    measure.mut_setMagnitude(newMagnitude);
    assertEquals(newMagnitude, measure.magnitude(), 0); // should be an exact match
    double feetToMeters = 0.3048;
    assertEquals(newMagnitude * feetToMeters, measure.baseUnitMagnitude(), 1e-12);
  }

  @Test
  void testMut_times() {
    var measure = MutableMeasure.ofRelativeUnits(18, FeetPerSecond);
    double scalar = Math.PI;
    var result = measure.mut_times(scalar);
    assertSame(measure, result, "mut_times should return the same object");
    assertEquals(18 * Math.PI, measure.magnitude(), 1e-12);
  }

  @Test
  void testMut_divide() {
    var measure = MutableMeasure.ofRelativeUnits(18, FeetPerSecond);
    double scalar = Math.PI;
    var result = measure.mut_divide(scalar);
    assertSame(measure, result, "mut_times should return the same object");
    assertEquals(18 / Math.PI, measure.magnitude(), 1e-12);
  }

  @Test
  void testReplaceMeasure() {
    var measure = MutableMeasure.ofRelativeUnits(17.6, FeetPerSecond);
    var replacer = Meters.per(Millisecond).of(94.872);
    var result = measure.mut_replace(replacer);
    assertSame(measure, result, "Replacing should return the mutable measure");
    assertSame(replacer.unit(), measure.unit());
    assertEquals(replacer.magnitude(), measure.magnitude(), 0);
  }

  @Test
  void testReplaceRaw() {
    var measure = MutableMeasure.ofRelativeUnits(-542, FeetPerSecond);
    var result = measure.mut_replace(62, Meters.per(Millisecond));
    assertSame(measure, result, "Replacing should return the mutable measure");
    assertSame(Meters.per(Millisecond), measure.unit());
    assertEquals(62, measure.magnitude(), 0);
  }

  @Test
  void testAccMeasure() {
    var measure = MutableMeasure.ofRelativeUnits(8.5431, FeetPerSecond);
    var acc = Meters.per(Millisecond).of(-23.62);
    var result = measure.mut_acc(acc);
    assertSame(measure, result, "Acc should return the mutable measure");
    assertSame(FeetPerSecond, measure.unit(), "Unit shouldn't change");
    assertEquals(8.5431 - (23.62 * (1 / 0.3048) * 1000), measure.magnitude(), 1e-10);
  }

  @Test
  void testAccRaw() {
    var measure = MutableMeasure.ofRelativeUnits(99.999999, FeetPerSecond);
    var result = measure.mut_acc(22.981);
    assertSame(measure, result);
    assertSame(FeetPerSecond, measure.unit());
    assertEquals(122.980999, measure.magnitude(), 0);
  }

  @Test
  void testMutPlusMeasure() {
    var measure = MutableMeasure.ofRelativeUnits(400, InchesPerSecond);
    var other = Centimeters.per(Second).of(41.312);
    var result = measure.mut_plus(other);
    assertSame(measure, result);
    assertSame(InchesPerSecond, result.unit());
    assertEquals(416.2645669291339, measure.magnitude(), 1e-12);
  }

  @Test
  void testMutPlusRaw() {
    var measure = MutableMeasure.ofRelativeUnits(31.51, Volts);
    var result = measure.mut_plus(66.641, Millivolts);
    assertSame(measure, result);
    assertSame(Volts, result.unit());
    assertEquals(31.576641, result.magnitude(), 1e-10);
  }

  @Test
  void testMutMinusMeasure() {
    var measure = MutableMeasure.ofRelativeUnits(400, InchesPerSecond);
    var other = Centimeters.per(Second).of(41.312);
    var result = measure.mut_minus(other);
    assertSame(measure, result);
    assertSame(InchesPerSecond, result.unit());
    assertEquals(383.7354330708662, measure.magnitude(), 1e-12);
  }

  @Test
  void testMutMinusRaw() {
    var measure = MutableMeasure.ofRelativeUnits(31.51, Volts);
    var result = measure.mut_minus(66.641, Millivolts);
    assertSame(measure, result);
    assertSame(Volts, result.unit());
    assertEquals(31.443359, result.magnitude(), 1e-10);
  }
}
