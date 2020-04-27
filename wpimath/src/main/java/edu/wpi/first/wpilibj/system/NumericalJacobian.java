/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.system;

import java.util.function.BiFunction;
import java.util.function.Function;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

public final class NumericalJacobian {
  private NumericalJacobian() {
    // Utility Class.
  }

  private static final double kEpsilon = 1e-5;

  /**
   * Computes the numerical Jacobian with respect to x for f(x).
   *
   * @param <R>   Number of rows in the result of f(x).
   * @param <S> Num representing the number of rows in the output of f.
   * @param <C>   Number of columns in the result of f(x).
   * @param rows     Number of rows in the result of f(x).
   * @param cols     Number of columns in the result of f(x).
   * @param f        Vector-valued function from which to compute the Jacobian.
   * @param x        Vector argument.
   * @return The numerical Jacobian with respect to x for f(x, u, ...).
   */
  @SuppressWarnings("ParameterName")
  public static <R extends Num, C extends Num, S extends Num> Matrix<R, C> numericalJacobian(
          Nat<R> rows,
          Nat<C> cols,
          Function<Matrix<C, N1>, Matrix<S, N1>> f,
          Matrix<C, N1> x
  ) {
    var result = new Matrix<>(rows, cols);

    for (int i = 0; i < cols.getNum(); i++) {
      var dxPlus = x.copy();
      var dxMinus = x.copy();
      dxPlus.set(i, 0, dxPlus.get(i, 0) + kEpsilon);
      dxMinus.set(i, 0, dxMinus.get(i, 0) - kEpsilon);
      @SuppressWarnings("LocalVariableName")
      var dF = f.apply(dxPlus).minus(f.apply(dxMinus)).div(2 * kEpsilon);

      result.setColumn(i, Matrix.changeBoundsUnchecked(dF));
    }

    return result;
  }

  /**
   * Returns numerical Jacobian with respect to x for f(x, u, ...).
   *
   * @param <R>   Number of rows in the result of f(x, u).
   * @param <S> Number of rows in x.
   * @param <I> Number of rows in the second input to f.
   * @param <D>      Num representing the rows in the output of f.
   * @param rows     Number of rows in the result of f(x, u).
   * @param states   Number of rows in x.
   * @param f        Vector-valued function from which to compute Jacobian.
   * @param x        State vector.
   * @param u        Input vector.
   * @return The numerical Jacobian with respect to x for f(x, u, ...).
   */
  @SuppressWarnings("ParameterName")
  public static <R extends Num, S extends Num, I extends Num, D extends Num> Matrix<R, S>
      numericalJacobianX(
          Nat<R> rows,
          Nat<S> states,
          BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<D, N1>> f,
          Matrix<S, N1> x,
          Matrix<I, N1> u
  ) {
    return numericalJacobian(rows, states, _x -> f.apply(_x, u), x);
  }

  /**
   * Returns the numerical Jacobian with respect to u for f(x, u).
   *
   * @param <S> The states of the system.
   * @param <I> The inputs to the system.
   * @param <R>   Number of rows in the result of f(x, u).
   * @param rows     Number of rows in the result of f(x, u).
   * @param inputs   Number of rows in u.
   * @param f        Vector-valued function from which to compute the Jacobian.
   * @param x        State vector.
   * @param u        Input vector.
   * @return the numerical Jacobian with respect to u for f(x, u).
   */
  @SuppressWarnings("ParameterName")
  public static <R extends Num, S extends Num, I extends Num> Matrix<R, I> numericalJacobianU(
          Nat<R> rows,
          Nat<I> inputs,
          BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<S, N1>> f,
          Matrix<S, N1> x,
          Matrix<I, N1> u
  ) {
    return numericalJacobian(rows, inputs, _u -> f.apply(x, _u), u);
  }
}
