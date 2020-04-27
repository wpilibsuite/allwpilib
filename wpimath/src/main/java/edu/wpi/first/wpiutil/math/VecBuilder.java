/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpiutil.math;

import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N10;
import edu.wpi.first.wpiutil.math.numbers.N2;
import edu.wpi.first.wpiutil.math.numbers.N3;
import edu.wpi.first.wpiutil.math.numbers.N4;
import edu.wpi.first.wpiutil.math.numbers.N5;
import edu.wpi.first.wpiutil.math.numbers.N6;
import edu.wpi.first.wpiutil.math.numbers.N7;
import edu.wpi.first.wpiutil.math.numbers.N8;
import edu.wpi.first.wpiutil.math.numbers.N9;

/**
 * A specialization of {@link MatBuilder} for constructing vectors (Nx1 matrices).
 *
 * @param <N> The dimension of the vector to be constructed.
 */
public class VecBuilder<N extends Num> extends MatBuilder<N, N1> {
  public VecBuilder(Nat<N> rows) {
    super(rows, Nat.N1());
  }

  /**
   * Returns a 1x1 vector containing the given elements.
   *
   * @param n1 the first element.
   */
  public static Matrix<N1, N1> fill(double n1) {
    return new MatBuilder<>(Nat.N1(), Nat.N1()).fill(n1);
  }

  /**
   * Returns a 2x1 vector containing the given elements.
   *
   * @param n1 the first element.
   * @param n2 the second element.
   */
  public static Matrix<N2, N1> fill(double n1, double n2) {
    return new MatBuilder<>(Nat.N2(), Nat.N1()).fill(n1, n2);
  }

  /**
   * Returns a 3x1 vector containing the given elements.
   *
   * @param n1 the first element.
   * @param n2 the second element.
   * @param n3 the third element.
   */
  public static Matrix<N3, N1> fill(double n1, double n2, double n3) {
    return new MatBuilder<>(Nat.N3(), Nat.N1()).fill(n1, n2, n3);
  }

  /**
   * Returns a 4x1 vector containing the given elements.
   *
   * @param n1 the first element.
   * @param n2 the second element.
   * @param n3 the third element.
   * @param n4 the fourth element.
   */
  public static Matrix<N4, N1> fill(double n1, double n2, double n3, double n4) {
    return new MatBuilder<>(Nat.N4(), Nat.N1()).fill(n1, n2, n3, n4);
  }

  /**
   * Returns a 5x1 vector containing the given elements.
   *
   * @param n1 the first element.
   * @param n2 the second element.
   * @param n3 the third element.
   * @param n4 the fourth element.
   * @param n5 the fifth element.
   */
  public static Matrix<N5, N1> fill(double n1, double n2, double n3, double n4, double n5) {
    return new MatBuilder<>(Nat.N5(), Nat.N1()).fill(n1, n2, n3, n4, n5);
  }

  /**
   * Returns a 6x1 vector containing the given elements.
   *
   * @param n1 the first element.
   * @param n2 the second element.
   * @param n3 the third element.
   * @param n4 the fourth element.
   * @param n5 the fifth element.
   * @param n6 the sixth element.
   */
  public static Matrix<N6, N1> fill(double n1, double n2, double n3, double n4, double n5,
                                    double n6) {
    return new MatBuilder<>(Nat.N6(), Nat.N1()).fill(n1, n2, n3, n4, n5, n6);
  }

  /**
   * Returns a 7x1 vector containing the given elements.
   *
   * @param n1 the first element.
   * @param n2 the second element.
   * @param n3 the third element.
   * @param n4 the fourth element.
   * @param n5 the fifth element.
   * @param n6 the sixth element.
   * @param n7 the seventh element.
   */
  public static Matrix<N7, N1> fill(double n1, double n2, double n3, double n4, double n5,
                                    double n6, double n7) {
    return new MatBuilder<>(Nat.N7(), Nat.N1()).fill(n1, n2, n3, n4, n5, n6, n7);
  }

  /**
   * Returns a 8x1 vector containing the given elements.
   *
   * @param n1 the first element.
   * @param n2 the second element.
   * @param n3 the third element.
   * @param n4 the fourth element.
   * @param n5 the fifth element.
   * @param n6 the sixth element.
   * @param n7 the seventh element.
   * @param n8 the eighth element.
   */
  public static Matrix<N8, N1> fill(double n1, double n2, double n3, double n4, double n5,
                                    double n6, double n7, double n8) {
    return new MatBuilder<>(Nat.N8(), Nat.N1()).fill(n1, n2, n3, n4, n5, n6, n7, n8);
  }

  /**
   * Returns a 9x1 vector containing the given elements.
   *
   * @param n1 the first element.
   * @param n2 the second element.
   * @param n3 the third element.
   * @param n4 the fourth element.
   * @param n5 the fifth element.
   * @param n6 the sixth element.
   * @param n7 the seventh element.
   * @param n8 the eighth element.
   * @param n9 the ninth element.
   */
  public static Matrix<N9, N1> fill(double n1, double n2, double n3, double n4, double n5,
                                    double n6, double n7, double n8, double n9) {
    return new MatBuilder<>(Nat.N9(), Nat.N1()).fill(n1, n2, n3, n4, n5, n6, n7, n8, n9);
  }

  /**
   * Returns a 10x1 vector containing the given elements.
   *
   * @param n1 the first element.
   * @param n2 the second element.
   * @param n3 the third element.
   * @param n4 the fourth element.
   * @param n5 the fifth element.
   * @param n6 the sixth element.
   * @param n7 the seventh element.
   * @param n8 the eighth element.
   * @param n9 the ninth element.
   * @param n10 the tenth element.
   */
  @SuppressWarnings("PMD.ExcessiveParameterList")
  public static Matrix<N10, N1> fill(double n1, double n2, double n3, double n4, double n5,
                                    double n6, double n7, double n8, double n9, double n10) {
    return new MatBuilder<>(Nat.N10(), Nat.N1()).fill(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10);
  }
}
