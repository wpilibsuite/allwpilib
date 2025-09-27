// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import java.util.ArrayList;

public final class Range {
  private Range() {
    // Utility class.
  }

  public static ArrayList<Double> range(double start, double end, double step) {
    var ret = new ArrayList<Double>();

    int steps = (int) ((end - start) / step);
    for (int i = 0; i < steps; ++i) {
      ret.add(start + i * step);
    }

    return ret;
  }
}
