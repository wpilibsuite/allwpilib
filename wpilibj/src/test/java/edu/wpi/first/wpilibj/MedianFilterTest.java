/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

public class MedianFilterTest {
  @Test
  void medianFilterNotFullTest() {
    MedianFilter filter = new MedianFilter(10);

    filter.calculate(3);
    filter.calculate(0);
    filter.calculate(4);

    assertEquals(3.5, filter.calculate(1000));
  }

  @Test
  void medianFilterFullTest() {
    MedianFilter filter = new MedianFilter(5);

    filter.calculate(3);
    filter.calculate(0);
    filter.calculate(5);
    filter.calculate(4);
    filter.calculate(1000);

    assertEquals(5, filter.calculate(99));
  }
}
