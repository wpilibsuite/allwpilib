// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.function.BiFunction;

public final class CombinatoryUnitCache<A extends Unit, B extends Unit, Out extends Unit> {
  /**
   * Keep a cache of created instances so expressions like Volts.per(Meter) don't do any allocations
   * after the first.
   */
  private final LongToObjectHashMap<Out> cache = new LongToObjectHashMap<>();

  private final BiFunction<? super A, ? super B, ? extends Out> constructor;

  public CombinatoryUnitCache(BiFunction<? super A, ? super B, ? extends Out> constructor) {
    this.constructor = constructor;
  }

  public Out combine(A a, B b) {
    final long key = ((long) a.hashCode()) << 32L | (b.hashCode() & 0xFFFFFFFFL);

    var existing = cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = constructor.apply(a, b);
    cache.put(key, newUnit);
    return newUnit;
  }
}
