package edu.wpi.first.util;

import org.junit.jupiter.api.Test;

public class InterpolatingTreeMapTest {
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
  }

  @Test
  void testInterpolationComparable() {}
}
