// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

public class Pair<A, B> {
  private final A m_first;
  private final B m_second;

  public Pair(A first, B second) {
    m_first = first;
    m_second = second;
  }

  public A getFirst() {
    return m_first;
  }

  public B getSecond() {
    return m_second;
  }

  public static <A, B> Pair<A, B> of(A a, B b) {
    return new Pair<>(a, b);
  }
}
