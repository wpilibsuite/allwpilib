// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

import java.util.Map;

/**
 * Interpolating Tree Maps are used to get values at points that are not defined by making a guess
 * from points that are defined. This uses linear interpolation.
 */
public class InterpolatingDoubleTreeMap extends InterpolatingTreeMap<Double, Double> {
  /** Default constructor. */
  public InterpolatingDoubleTreeMap() {
    super(InverseInterpolator.forDouble(), Interpolator.forDouble());
  }

  /**
   * Creates an {@link InterpolatingDoubleTreeMap} from the given entries.
   *
   * @param entries The entries to add to the map.
   * @return The map filled with the {@code entries}.
   */
  @SafeVarargs
  public static InterpolatingDoubleTreeMap ofEntries(Map.Entry<Double, Double>... entries) {
    InterpolatingDoubleTreeMap map = new InterpolatingDoubleTreeMap();
    for (var entry : entries) {
      map.put(entry.getKey(), entry.getValue());
    }
    return map;
  }
}
