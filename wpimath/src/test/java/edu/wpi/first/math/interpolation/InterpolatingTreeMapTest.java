// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

public class InterpolatingTreeMapTest {
  @Test
  void testInterpolation() {
    InterpolatingTreeMap<InterpolatingDouble, InterpolatingDouble> interpolatingTreeMap =
        new InterpolatingTreeMap<>();

    interpolatingTreeMap.put(new InterpolatingDouble(1.0), new InterpolatingDouble(500.0));
    interpolatingTreeMap.put(new InterpolatingDouble(2.0), new InterpolatingDouble(1000.0));
    interpolatingTreeMap.put(new InterpolatingDouble(6.0), new InterpolatingDouble(4000.0));

    assertEquals(
        new InterpolatingDouble(750.0).value.doubleValue(),
        interpolatingTreeMap.getInterpolated(new InterpolatingDouble(1.5)).value.doubleValue());
    assertEquals(
        new InterpolatingDouble(1750.0).value.doubleValue(),
        interpolatingTreeMap.getInterpolated(new InterpolatingDouble(3.0)).value.doubleValue());
  }
}
