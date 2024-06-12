// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

/**
 * Represents a pair of two objects.
 *
 * @param <A> The first object's type.
 * @param <B> The second object's type.
 */
public class Pair<A, B> {
  private final A m_first;
  private final B m_second;

  /**
   * Constructs a pair.
   *
   * @param first The first object.
   * @param second The second object.
   */
  public Pair(A first, B second) {
    m_first = first;
    m_second = second;
  }

  /**
   * Returns the first object.
   *
   * @return The first object.
   */
  public A getFirst() {
    return m_first;
  }

  /**
   * Returns the second object.
   *
   * @return The second object.
   */
  public B getSecond() {
    return m_second;
  }

  /**
   * Returns a pair comprised of the two given objects.
   *
   * @param <A> The first object's type.
   * @param <B> The second object's type.
   * @param a The first object.
   * @param b The second object.
   * @return A pair comprised of the two given objects.
   */
  public static <A, B> Pair<A, B> of(A a, B b) {
    return new Pair<>(a, b);
  }

  @Override
  public String toString() {
    return String.format("Pair(%s, %s)", m_first, m_second);
  }
}
