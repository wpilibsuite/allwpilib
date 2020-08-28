/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.math;

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

  @SuppressWarnings("ParameterName")
  public static <A, B> Pair<A, B> of(A a, B b) {
    return new Pair<>(a, b);
  }
}
