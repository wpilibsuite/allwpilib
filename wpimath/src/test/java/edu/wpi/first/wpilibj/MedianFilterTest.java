/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

public class MedianFilterTest {
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
