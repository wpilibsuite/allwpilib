// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.units;

import java.util.Objects;

/**
 * A combinatory unit type that is equivalent to the product of two other others. For example,
 * Newton * Meters for torque could be represented as a unit of <code>
 * Mult&lt;Force, Distance, Torque&gt;</code>
 *
 * @param <A> the type of the first unit in the result
 * @param <B> the type of the second unit in the result
 */
public class Mult<A extends Unit<A>, B extends Unit<B>> extends Unit<Mult<A, B>> {
  private final A m_unitA;
  private final B m_unitB;

  protected Mult(Class<? extends Mult<A, B>> baseType, A a, B b) {
    super(
        baseType,
        a.toBaseUnits(1) * b.toBaseUnits(1),
        a.name() + " " + b.name(),
        a.symbol() + "*" + b.symbol());
    m_unitA = a;
    m_unitB = b;
  }

  @SuppressWarnings({"unchecked", "rawtypes"})
  public static <A extends Unit<A>, B extends Unit<B>> Mult<A, B> combine(A a, B b) {
    return new Mult<A, B>((Class) Mult.class, a, b);
  }

  public A unitA() {
    return m_unitA;
  }

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
