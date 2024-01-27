// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import edu.wpi.first.units.collections.LongToObjectHashMap;
import java.util.Objects;

/**
 * A combinatory unit type that is equivalent to the product of two other others. Note that
 * algebraic reduction is not possible in Java's generic type system, so {@code Mult<A, B>} is not
 * type-compatible with {@code Mult<B, A>}!
 *
 * @param <A> the type of the first unit in the result
 * @param <B> the type of the second unit in the result
 */
public class Mult<A extends Unit<A>, B extends Unit<B>> extends Unit<Mult<A, B>> {
  private final A m_unitA;
  private final B m_unitB;

  @SuppressWarnings("rawtypes")
  private static final LongToObjectHashMap<Mult> cache = new LongToObjectHashMap<>();

  /**
   * Creates a new product unit. Consider using {@link #combine} instead of manually calling this
   * constructor.
   *
   * @param baseType the base type representing the unit product
   * @param a the first unit of the product
   * @param b the second unit of the product
   */
  protected Mult(Class<? extends Mult<A, B>> baseType, A a, B b) {
    super(
        baseType,
        a.toBaseUnits(1) * b.toBaseUnits(1),
        a.name() + "-" + b.name(),
        a.symbol() + "*" + b.symbol());
    m_unitA = a;
    m_unitB = b;
  }

  /**
   * Creates a new Mult unit derived from two arbitrary units multiplied together.
   *
   * <pre>
   *   Mult.combine(Volts, Meters) // Volt-Meters
   * </pre>
   *
   * <p>It's recommended to use the convenience function {@link Unit#mult(Unit)} instead of calling
   * this factory directly.
   *
   * @param <A> the type of the first unit
   * @param <B> the type of the second unit
   * @param a the first unit
   * @param b the second unit
   * @return the combined unit
   */
  @SuppressWarnings({"unchecked", "rawtypes"})
  public static <A extends Unit<A>, B extends Unit<B>> Mult<A, B> combine(A a, B b) {
    final long key = ((long) a.hashCode()) << 32L | (((long) b.hashCode()) & 0xFFFFFFFFL);
    if (cache.containsKey(key)) {
      return cache.get(key);
    }

    var mult = new Mult<A, B>((Class) Mult.class, a, b);
    cache.put(key, mult);
    return mult;
  }

  /**
   * Gets the first unit of the product.
   *
   * @return the first unit
   */
  public A unitA() {
    return m_unitA;
  }

  /**
   * Gets the second unit of the product.
   *
   * @return the second unit
   */
  public B unitB() {
    return m_unitB;
  }

  @Override
  public String toString() {
    return "(" + m_unitA.toString() + " * " + m_unitB.toString() + ")";
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }
    if (!super.equals(o)) {
      return false;
    }
    Mult<?, ?> mult = (Mult<?, ?>) o;
    return Objects.equals(m_unitA, mult.m_unitA) && Objects.equals(m_unitB, mult.m_unitB);
  }

  @Override
  public int hashCode() {
    return Objects.hash(super.hashCode(), m_unitA, m_unitB);
  }
}
