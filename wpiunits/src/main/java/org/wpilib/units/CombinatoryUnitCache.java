// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;
import java.util.function.BiFunction;

/**
 * A helper class for creating and caching combined unit objects. This helps to reduce unnecessary
 * object allocation by reusing already-created units.
 *
 * @param <A> the type of the first unit to be combined
 * @param <B> the type of the second unit to be combined
 * @param <Out> the type of the combinatorial unit
 */
public final class CombinatoryUnitCache<A extends Unit, B extends Unit, Out extends Unit> {
  /**
   * Keep a cache of created instances so expressions like Volts.per(Meter) don't do any allocations
   * after the first.
   */
  private final LongToObjectHashMap<Out> m_cache = new LongToObjectHashMap<>();

  private final BiFunction<? super A, ? super B, ? extends Out> m_constructor;

  /**
   * Creates a new combinatory unit cache. The cache is initially empty and is not shared across
   * instances.
   *
   * @param constructor the constructor function to use to create new combined units
   */
  public CombinatoryUnitCache(BiFunction<? super A, ? super B, ? extends Out> constructor) {
    this.m_constructor =
        Objects.requireNonNull(constructor, "Cache unit constructor must be provided");
  }

  /**
   * Combines two units together and returns the result. The resulting units are cached and will be
   * returned on successive calls to avoid allocating many duplicate objects. The combination output
   * type is determined by the factory function passed into the cache's constructor.
   *
   * @param a the first unit
   * @param b the second unit
   * @return the combined unit
   */
  public Out combine(A a, B b) {
    final long key = ((long) a.hashCode()) << 32L | (b.hashCode() & 0xFFFFFFFFL);

    var existing = m_cache.get(key);
    if (existing != null) {
      return existing;
    }

    var newUnit = m_constructor.apply(a, b);
    m_cache.put(key, newUnit);
    return newUnit;
  }
}
