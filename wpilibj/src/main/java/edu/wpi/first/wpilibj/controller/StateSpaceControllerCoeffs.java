/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

/**
 * A container for all the state-space controller coefficients.
 */
@SuppressWarnings({"unused", "ParameterName", "ClassTypeParameterName", "MemberName", "LineLength"})
public final class StateSpaceControllerCoeffs<States extends Num, Inputs extends Num, Outputs extends Num> {
  /**
   * Controller gain matrix.
   */
  private final Matrix<Inputs, States> K;

  /**
   * Controller feedforward gain matrix.
   */
  private final Matrix<Inputs, States> Kff;

  /**
   * Minimum control input.
   */
  private final Matrix<Inputs, N1> Umin;

  /**
   * Maximum control input.
   */
  private final Matrix<Inputs, N1> Umax;

  /**
   * Construct the container for the controller coefficients.
   *
   * @param K    Controller gain matrix.
   * @param Kff  Controller feedforward gain matrix.
   * @param Umin Minimum control input.
   * @param Umax Maximum control input.
   */
  public StateSpaceControllerCoeffs(Matrix<Inputs, States> K, Matrix<Inputs, States> Kff,
                                    Matrix<Inputs, N1> Umin, Matrix<Inputs, N1> Umax) {
    this.K = K;
    this.Kff = Kff;
    this.Umin = Umin;
    this.Umax = Umax;
  }

  public Matrix<Inputs, States> getK() {
    return K;
  }

  public Matrix<Inputs, States> getKff() {
    return Kff;
  }

  public Matrix<Inputs, N1> getUmin() {
    return Umin;
  }

  public double getUmin(int i) {
    return getUmin().get(i, 0);
  }

  public Matrix<Inputs, N1> getUmax() {
    return Umax;
  }

  public double getUmax(int i) {
    return getUmax().get(i, 0);
  }
}
