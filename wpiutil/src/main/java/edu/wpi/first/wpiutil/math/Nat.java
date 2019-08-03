/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.math;

import edu.wpi.first.wpiutil.math.numbers.N0;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N10;
import edu.wpi.first.wpiutil.math.numbers.N11;
import edu.wpi.first.wpiutil.math.numbers.N12;
import edu.wpi.first.wpiutil.math.numbers.N13;
import edu.wpi.first.wpiutil.math.numbers.N14;
import edu.wpi.first.wpiutil.math.numbers.N15;
import edu.wpi.first.wpiutil.math.numbers.N16;
import edu.wpi.first.wpiutil.math.numbers.N17;
import edu.wpi.first.wpiutil.math.numbers.N18;
import edu.wpi.first.wpiutil.math.numbers.N19;
import edu.wpi.first.wpiutil.math.numbers.N2;
import edu.wpi.first.wpiutil.math.numbers.N20;
import edu.wpi.first.wpiutil.math.numbers.N3;
import edu.wpi.first.wpiutil.math.numbers.N4;
import edu.wpi.first.wpiutil.math.numbers.N5;
import edu.wpi.first.wpiutil.math.numbers.N6;
import edu.wpi.first.wpiutil.math.numbers.N7;
import edu.wpi.first.wpiutil.math.numbers.N8;
import edu.wpi.first.wpiutil.math.numbers.N9;

/**
 * A natural number expressed as a java class.
 * The counterpart to {@link Num} that should be used as a concrete value.
 *
 * @param <T> The {@link Num} this represents.
 */
@SuppressWarnings({"MethodName", "unused", "PMD.TooManyMethods"})
public interface Nat<T extends Num> {
  /**
   * The number this interface represents.
   *
   * @return The number backing this value.
   */
  int getNum();

  static Nat<N0> N0() {
    return N0.instance;
  }

  static Nat<N1> N1() {
    return N1.instance;
  }

  static Nat<N2> N2() {
    return N2.instance;
  }

  static Nat<N3> N3() {
    return N3.instance;
  }

  static Nat<N4> N4() {
    return N4.instance;
  }

  static Nat<N5> N5() {
    return N5.instance;
  }

  static Nat<N6> N6() {
    return N6.instance;
  }

  static Nat<N7> N7() {
    return N7.instance;
  }

  static Nat<N8> N8() {
    return N8.instance;
  }

  static Nat<N9> N9() {
    return N9.instance;
  }

  static Nat<N10> N10() {
    return N10.instance;
  }

  static Nat<N11> N11() {
    return N11.instance;
  }

  static Nat<N12> N12() {
    return N12.instance;
  }

  static Nat<N13> N13() {
    return N13.instance;
  }

  static Nat<N14> N14() {
    return N14.instance;
  }

  static Nat<N15> N15() {
    return N15.instance;
  }

  static Nat<N16> N16() {
    return N16.instance;
  }

  static Nat<N17> N17() {
    return N17.instance;
  }

  static Nat<N18> N18() {
    return N18.instance;
  }

  static Nat<N19> N19() {
    return N19.instance;
  }

  static Nat<N20> N20() {
    return N20.instance;
  }
}
