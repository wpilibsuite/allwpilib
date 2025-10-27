// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.filter;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class MedianFilterTest {
  @Test
  void medianFilterNotFullTestEven() {
    MedianFilter filter = new MedianFilter(10);

    filter.calculate(3);
    filter.calculate(0);
    filter.calculate(4);

    assertEquals(3.5, filter.calculate(1000));
  }

  @Test
  void medianFilterNotFullTestOdd() {
    MedianFilter filter = new MedianFilter(10);

    filter.calculate(3);
    filter.calculate(0);
    filter.calculate(4);
    filter.calculate(7);

    assertEquals(4, filter.calculate(1000));
  }

  @Test
  void medianFilterFullTestEven() {
    MedianFilter filter = new MedianFilter(6);

    filter.calculate(3);
    filter.calculate(0);
    filter.calculate(0);
    filter.calculate(5);
    filter.calculate(4);
    filter.calculate(1000);

    assertEquals(4.5, filter.calculate(99));
  }

  @Test
  void medianFilterFullTestOdd() {
    MedianFilter filter = new MedianFilter(5);

    filter.calculate(3);
    filter.calculate(0);
    filter.calculate(5);
    filter.calculate(4);
    filter.calculate(1000);

    assertEquals(5, filter.calculate(99));
  }
}
