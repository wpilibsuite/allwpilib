// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.Objects;
import org.junit.jupiter.api.Test;

class InterpolatingTreeMapTest {
  static class DoubleInterpolatable
      implements Interpolatable<DoubleInterpolatable>,
          InverseInterpolatable<DoubleInterpolatable>,
          Comparable<DoubleInterpolatable> {
    public final double value;

    DoubleInterpolatable(double value) {
      this.value = value;
    }

    @Override
    public DoubleInterpolatable interpolate(DoubleInterpolatable endValue, double t) {
      return new DoubleInterpolatable(t * (endValue.value - value) + value);
    }

    @Override
    public double inverseInterpolate(DoubleInterpolatable lower, DoubleInterpolatable upper) {
      double upperToLower = upper.value - lower.value;
      if (upperToLower <= 0) {
        return 0;
      }
      double valueToLower = value - lower.value;
      if (valueToLower <= 0) {
        return 0;
      }
      return valueToLower / upperToLower;
    }

    @Override
    public int compareTo(DoubleInterpolatable other) {
      return Double.compare(value, other.value);
    }

    // Satisfy Spotbugs
    @Override
    public boolean equals(Object obj) {
      if (this == obj) {
        return true;
      }
      if (obj == null || getClass() != obj.getClass()) {
        return false;
      }
      DoubleInterpolatable other = (DoubleInterpolatable) obj;
      return this.value == other.value;
    }

    @Override
    public int hashCode() {
      return Objects.hash(value);
    }
  }

  private static DoubleInterpolatable wrap(double value) {
    return new DoubleInterpolatable(value);
  }

  @Test
  void testInterpolation() {
    InterpolatingTreeMap<DoubleInterpolatable, DoubleInterpolatable> table =
        new InterpolatingTreeMap<>();

    table.put(wrap(125.0), wrap(450.0));
    table.put(wrap(200.0), wrap(510.0));
    table.put(wrap(268.0), wrap(525.0));
    table.put(wrap(312.0), wrap(550.0));
    table.put(wrap(326.0), wrap(650.0));

    // Key below minimum gives the smallest value
    assertEquals(450.0, table.get(wrap(100.0)).value);

    // Minimum key gives exact value
    assertEquals(450.0, table.get(wrap(125.0)).value);

    // Key gives interpolated value
    assertEquals(480.0, table.get(wrap(162.5)).value);

    // Key at right of interpolation range gives exact value
    assertEquals(510.0, table.get(wrap(200.0)).value);

    // Maximum key gives exact value
    assertEquals(650.0, table.get(wrap(326.0)).value);

    // Key above maximum gives largest value
    assertEquals(650.0, table.get(wrap(400.0)).value);

    table.clear();

    table.put(wrap(100.0), wrap(250.0));
    table.put(wrap(200.0), wrap(500.0));

    assertEquals(375.0, table.get(wrap(150.0)).value);
  }
}
