/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

class BooleanMajorityFilterTest {
  @Test
  void reset() {
    BooleanMajorityFilter filter = new BooleanMajorityFilter(10);
    filter.calculate(true);
    filter.calculate(true);
    filter.calculate(true);
    filter.calculate(true);
    filter.calculate(true);
    filter.calculate(true);
    assertTrue(filter.calculate(true));
    filter.reset();
    assertFalse(filter.calculate(false));
  }

  @Test
  void empty() {
    BooleanMajorityFilter filter = new BooleanMajorityFilter(10);
    assertTrue(filter.calculate(true));
  }

  @Test
  void zero() {
    assertThrows(IllegalArgumentException.class, () -> new BooleanMajorityFilter(0, -3));
  }

  @Test
  void singleton() {
    BooleanMajorityFilter filter = new BooleanMajorityFilter(1);
    assertTrue(filter.calculate(true));
    assertFalse(filter.calculate(false));
  }

  @Test
  void equal() {
    BooleanMajorityFilter filter = new BooleanMajorityFilter(10, 0.4);
    filter.calculate(true);
    filter.calculate(false);
    filter.calculate(false);
    filter.calculate(true);
    filter.calculate(false);
    assertTrue(filter.calculate(true));
    filter.reset();
  }


  @Test
  void oneInHundred() {
    BooleanMajorityFilter filter = new BooleanMajorityFilter(100, 0.009);
    for (int i = 0; i < 99; i++) {
      filter.calculate(false);
    }
    assertTrue(filter.calculate(true));
  }

  @Test
  void cutoffEven() {
    BooleanMajorityFilter filter = new BooleanMajorityFilter(6);
    filter.calculate(true);
    filter.calculate(true);
    filter.calculate(true);
    filter.calculate(true);
    filter.calculate(true);
    assertTrue(filter.calculate(true));
    filter.calculate(false);
    filter.calculate(false);
    filter.calculate(false);
    assertFalse(filter.calculate(false));
  }

  @Test
  void cutoffOdd() {
    BooleanMajorityFilter filter = new BooleanMajorityFilter(7);
    filter.calculate(true);
    filter.calculate(true);
    filter.calculate(true);
    filter.calculate(true);
    filter.calculate(true);
    filter.calculate(true);
    assertTrue(filter.calculate(true));
    filter.calculate(false);
    filter.calculate(false);
    filter.calculate(false);
    assertFalse(filter.calculate(false));

  }

}
