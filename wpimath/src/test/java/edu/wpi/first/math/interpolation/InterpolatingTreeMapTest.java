// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class InterpolatingTreeMapTest {
  @Test
  void testInterpolationDouble() {
    InterpolatingTreeMap<Double, Double> interpolatingDoubleTreeMap = new InterpolatingTreeMap<>();

    interpolatingDoubleTreeMap.put(125.0, 450.0);
    interpolatingDoubleTreeMap.put(200.0, 510.0);
    interpolatingDoubleTreeMap.put(268.0, 525.0);
    interpolatingDoubleTreeMap.put(312.0, 550.0);
    interpolatingDoubleTreeMap.put(326.0, 650.0);

    // Key below minimum gives smallest value
    assertEquals(450.0, interpolatingDoubleTreeMap.getInterpolated(100.0));

    // Minimum key gives exact value
    assertEquals(450.0, interpolatingDoubleTreeMap.getInterpolated(125.0));

    // Key gives interpolated value
    assertEquals(480.0, interpolatingDoubleTreeMap.getInterpolated(162.5));

    // Key at right of interpolation range gives exact value
    assertEquals(510.0, interpolatingDoubleTreeMap.getInterpolated(200.0));

    // Maximum key gives exact value
    assertEquals(650.0, interpolatingDoubleTreeMap.getInterpolated(326.0));

    // Key above maximum gives largest value
    assertEquals(650.0, interpolatingDoubleTreeMap.getInterpolated(400.0));

    interpolatingDoubleTreeMap.clear();

    interpolatingDoubleTreeMap.put(100.0, 250.0);
    interpolatingDoubleTreeMap.put(200.0, 500.0);

    assertEquals(375.0, interpolatingDoubleTreeMap.getInterpolated(150.0));
  }

  @Test
  void testInterpolationInteger() {
    InterpolatingTreeMap<Integer, Integer> interpolatingIntegerTreeMap =
        new InterpolatingTreeMap<>();

    interpolatingIntegerTreeMap.put(125, 450);
    interpolatingIntegerTreeMap.put(200, 510);
    interpolatingIntegerTreeMap.put(268, 525);
    interpolatingIntegerTreeMap.put(312, 550);
    interpolatingIntegerTreeMap.put(326, 650);

    // Key below minimum gives smallest value
    assertEquals(450.0, interpolatingIntegerTreeMap.getInterpolated(100));

    // Minimum key gives exact value
    assertEquals(450.0, interpolatingIntegerTreeMap.getInterpolated(125));

    // Key gives interpolated value
    assertEquals(479.6, interpolatingIntegerTreeMap.getInterpolated(162));

    // Key at right of interpolation range gives exact value
    assertEquals(510.0, interpolatingIntegerTreeMap.getInterpolated(200));

    // Maximum key gives exact value
    assertEquals(650.0, interpolatingIntegerTreeMap.getInterpolated(326));

    // Key above maximum gives largest value
    assertEquals(650.0, interpolatingIntegerTreeMap.getInterpolated(400));
  }

  @Test
  void testInterpolationLong() {
    InterpolatingTreeMap<Long, Long> interpolatingIntegerTreeMap = new InterpolatingTreeMap<>();

    interpolatingIntegerTreeMap.put(125L, 450L);
    interpolatingIntegerTreeMap.put(200L, 510L);
    interpolatingIntegerTreeMap.put(268L, 525L);
    interpolatingIntegerTreeMap.put(312L, 550L);
    interpolatingIntegerTreeMap.put(326L, 650L);

    // Key below minimum gives smallest value
    assertEquals(450.0, interpolatingIntegerTreeMap.getInterpolated(100L));

    // Minimum key gives exact value
    assertEquals(450.0, interpolatingIntegerTreeMap.getInterpolated(125L));

    // Key gives interpolated value
    assertEquals(479.6, interpolatingIntegerTreeMap.getInterpolated(162L));

    // Key at right of interpolation range gives exact value
    assertEquals(510.0, interpolatingIntegerTreeMap.getInterpolated(200L));

    // Maximum key gives exact value
    assertEquals(650.0, interpolatingIntegerTreeMap.getInterpolated(326L));

    // Key above maximum gives largest value
    assertEquals(650.0, interpolatingIntegerTreeMap.getInterpolated(400L));
  }
}
