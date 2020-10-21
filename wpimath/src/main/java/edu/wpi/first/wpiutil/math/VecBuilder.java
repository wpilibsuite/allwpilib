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

  private Vector<N> fillVec(double... data) {
    return new Vector<>(fill(data));
  }

  /**
   * Returns a 1x1 vector containing the given elements.
   *
   * @param n1 the first element.
   */
  public static Vector<N1> fill(double n1) {
    return new VecBuilder<>(Nat.N1()).fillVec(n1);
  }

  /**
   * Returns a 2x1 vector containing the given elements.
   *
   * @param n1 the first element.
   * @param n2 the second element.
   */
  public static Vector<N2> fill(double n1, double n2) {
    return new VecBuilder<>(Nat.N2()).fillVec(n1, n2);
  }

  /**
   * Returns a 3x1 vector containing the given elements.
   *
   * @param n1 the first element.
   * @param n2 the second element.
   * @param n3 the third element.
   */
  public static Vector<N3> fill(double n1, double n2, double n3) {
    return new VecBuilder<>(Nat.N3()).fillVec(n1, n2, n3);
  }

  /**
   * Returns a 4x1 vector containing the given elements.
   *
   * @param n1 the first element.
   * @param n2 the second element.
   * @param n3 the third element.
   * @param n4 the fourth element.
   */
  public static Vector<N4> fill(double n1, double n2, double n3, double n4) {
    return new VecBuilder<>(Nat.N4()).fillVec(n1, n2, n3, n4);
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
  public static Vector<N5> fill(double n1, double n2, double n3, double n4, double n5) {
    return new VecBuilder<>(Nat.N5()).fillVec(n1, n2, n3, n4, n5);
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
  public static Vector<N6> fill(double n1, double n2, double n3, double n4, double n5,
                                    double n6) {
    return new VecBuilder<>(Nat.N6()).fillVec(n1, n2, n3, n4, n5, n6);
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
  public static Vector<N7> fill(double n1, double n2, double n3, double n4, double n5,
                                    double n6, double n7) {
    return new VecBuilder<>(Nat.N7()).fillVec(n1, n2, n3, n4, n5, n6, n7);
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
  public static Vector<N8> fill(double n1, double n2, double n3, double n4, double n5,
                                    double n6, double n7, double n8) {
    return new VecBuilder<>(Nat.N8()).fillVec(n1, n2, n3, n4, n5, n6, n7, n8);
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
  public static Vector<N9> fill(double n1, double n2, double n3, double n4, double n5,
                                    double n6, double n7, double n8, double n9) {
    return new VecBuilder<>(Nat.N9()).fillVec(n1, n2, n3, n4, n5, n6, n7, n8, n9);
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
  public static Vector<N10> fill(double n1, double n2, double n3, double n4, double n5,
                                    double n6, double n7, double n8, double n9, double n10) {
    return new VecBuilder<>(Nat.N10()).fillVec(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10);
  }
}
