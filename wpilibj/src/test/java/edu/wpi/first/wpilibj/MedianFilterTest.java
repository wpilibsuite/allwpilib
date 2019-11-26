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
