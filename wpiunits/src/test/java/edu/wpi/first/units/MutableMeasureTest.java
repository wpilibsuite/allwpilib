// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.units.measure.Distance;
import edu.wpi.first.units.measure.MutDistance;
import org.junit.jupiter.api.Test;

class MutableMeasureTest {
  @Test
  void testBasics() {
    DistanceUnit unit = Units.Feet;
    double magnitude = 10;
    MutDistance m = unit.mutable(magnitude);
    assertEquals(unit, m.unit(), "Wrong units");
    assertEquals(magnitude, m.magnitude(), 0, "Wrong magnitude");
  }

  @Test
  void testMultiply() {
    MutDistance m = Units.Feet.mutable(1);
    MutDistance m2 = m.mut_times(Units.Value.of(10));
    assertEquals(10, m.in(Units.Feet), 1e-12);
    assertSame(m, m2);
  }

  @Test
  void testMultiplyScalar() {
    MutDistance m = Units.Feet.mutable(1);
    MutDistance m2 = m.mut_times(10);
    assertEquals(10, m.in(Units.Feet), 1e-12);
    assertSame(m, m2);
  }

  @Test
  void testDivide() {
    MutDistance m = Units.Meters.mutable(1);
    MutDistance m2 = m.mut_divide(Units.Value.of(10));
    assertEquals(0.1, m.magnitude(), 0);
    assertSame(m, m2);
  }

  @Test
  void testDivideScalar() {
    MutDistance m = Units.Meters.mutable(1);
    MutDistance m2 = m.mut_divide(10);
    assertEquals(0.1, m.magnitude(), 0);
    assertSame(m, m2);
  }

  @Test
  void testAdd() {
    MutDistance m1 = Units.Feet.mutable(1);
    MutDistance m2 = Units.Inches.mutable(2);

    Distance sum1 = m1.mut_plus(Units.Inches.of(2));
    assertTrue(sum1.isEquivalent(Units.Feet.of(1 + 2 / 12d)));
    assertSame(m1, sum1);

    Distance sum2 = m2.mut_plus(Units.Feet.of(1));
    assertTrue(sum2.isEquivalent(Units.Inches.of(14)));
    assertSame(m2, sum2);
  }

  @Test
  void testAddScalar() {
    MutDistance m1 = Units.Feet.mutable(1);
    MutDistance m2 = Units.Inches.mutable(2);

    Distance sum1 = m1.mut_plus(2, Units.Inches);
    assertTrue(sum1.isEquivalent(Units.Feet.of(1 + 2 / 12d)));
    assertSame(m1, sum1);

    Distance sum2 = m2.mut_plus(1, Units.Feet);
    assertTrue(sum2.isEquivalent(Units.Inches.of(14)));
    assertSame(m2, sum2);
  }

  @Test
  void testAcc() {
    MutDistance m1 = Units.Feet.mutable(1);
    MutDistance m2 = Units.Inches.mutable(2);

    Distance acc1 = m1.mut_acc(Units.Inches.of(2));
    assertTrue(acc1.isEquivalent(Units.Feet.of(1 + 2 / 12d)));
    assertSame(m1, acc1);

    Distance acc2 = m2.mut_acc(Units.Feet.of(1));
    assertTrue(acc2.isEquivalent(Units.Inches.of(14)));
    assertSame(m2, acc2);
  }

  @Test
  void testAccScalar() {
    MutDistance m1 = Units.Feet.mutable(1);
    MutDistance m2 = Units.Inches.mutable(2);

    Distance acc1 = m1.mut_acc(2 / 12d);
    assertTrue(acc1.isEquivalent(Units.Feet.of(1 + 2 / 12d)));
    assertSame(m1, acc1);

    Distance acc2 = m2.mut_acc(12);
    assertTrue(acc2.isEquivalent(Units.Inches.of(14)));
    assertSame(m2, acc2);
  }

  @Test
  void testSubtract() {
    MutDistance m1 = Units.Feet.mutable(1);
    MutDistance m2 = Units.Inches.mutable(2);

    Distance sub1 = m1.mut_minus(Units.Inches.of(2));
    assertTrue(sub1.isEquivalent(Units.Feet.of(1 - 2 / 12d)));
    assertSame(m1, sub1);

    Distance sub2 = m2.mut_minus(Units.Feet.of(1));
    assertTrue(sub2.isEquivalent(Units.Inches.of(-10)));
    assertSame(m2, sub2);
  }

  @Test
  void testSubtractScalar() {
    MutDistance m1 = Units.Feet.mutable(1);
    MutDistance m2 = Units.Inches.mutable(2);

    Distance sub1 = m1.mut_minus(2, Units.Inches);
    assertTrue(sub1.isEquivalent(Units.Feet.of(1 - 2 / 12d)));
    assertSame(m1, sub1);

    Distance sub2 = m2.mut_minus(1, Units.Feet);
    assertTrue(sub2.isEquivalent(Units.Inches.of(-10)));
    assertSame(m2, sub2);
  }

  @Test
  void testReplace() {
    MutDistance m1 = Units.Feet.mutable(1);
    MutDistance m2 = Units.Inches.mutable(2);

    Distance replace1 = m1.mut_replace(Units.Inches.of(2));
    assertTrue(replace1.isEquivalent(Units.Inches.of(2)));
    assertSame(m1, replace1);

    Distance replace2 = m2.mut_replace(Units.Feet.of(1));
    assertTrue(replace2.isEquivalent(Units.Feet.of(1)));
    assertSame(m2, replace2);
  }

  @Test
  void testReplaceScalar() {
    MutDistance m1 = Units.Feet.mutable(1);
    MutDistance m2 = Units.Inches.mutable(2);

    Distance replace1 = m1.mut_replace(2, Units.Inches);
    assertTrue(replace1.isEquivalent(Units.Inches.of(2)));
    assertSame(m1, replace1);

    Distance replace2 = m2.mut_replace(1, Units.Feet);
    assertTrue(replace2.isEquivalent(Units.Feet.of(1)));
    assertSame(m2, replace2);
  }
}
