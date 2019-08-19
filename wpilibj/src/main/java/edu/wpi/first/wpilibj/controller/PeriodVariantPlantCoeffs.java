/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;

/**
 * A container for all the period-variant plant coefficients.
 */
@SuppressWarnings({"ClassTypeParameterName", "MemberName", "LineLength"})
public final class PeriodVariantPlantCoeffs<States extends Num, Inputs extends Num, Outputs extends Num> {

  // The states, inputs, and outputs for this system
  private final Nat<States> states;
  private final Nat<Inputs> inputs;
  private final Nat<Outputs> outputs;

  /**
   * Continuous system matrix.
   */
  private final Matrix<States, States> Acontinuous;

  /**
   * Continuous input matrix.
   */
  private final Matrix<States, Inputs> Bcontinuous;

  /**
   * Output matrix.
   */
  private final Matrix<Outputs, States> C;

  /**
   * Feedthrough matrix.
   */
  private final Matrix<Outputs, Inputs> D;

  /**
   * Construct the container for the period-variant plant coefficients.
   *
   * @param Acontinuous Continuous system matrix.
   * @param Bcontinuous Continuous input matrix.
   * @param C           Output matrix.
   * @param D           Feedthrough matrix.
   */
  @SuppressWarnings("ParameterName")
  public PeriodVariantPlantCoeffs(Nat<States> states, Nat<Inputs> inputs, Nat<Outputs> outputs,
                                  Matrix<States, States> Acontinuous,
                                  Matrix<States, Inputs> Bcontinuous,
                                  Matrix<Outputs, States> C,
                                  Matrix<Outputs, Inputs> D) {
    this.states = states;
    this.inputs = inputs;
    this.outputs = outputs;
    this.Acontinuous = Acontinuous;
    this.Bcontinuous = Bcontinuous;
    this.C = C;
    this.D = D;
  }

  public Matrix<States, States> getAcontinuous() {
    return Acontinuous;
  }

  public Matrix<States, Inputs> getBcontinuous() {
    return Bcontinuous;
  }

  public Matrix<Outputs, States> getC() {
    return C;
  }

  public Matrix<Outputs, Inputs> getD() {
    return D;
  }

  public Nat<States> getStates() {
    return states;
  }

  public Nat<Inputs> getInputs() {
    return inputs;
  }

  public Nat<Outputs> getOutputs() {
    return outputs;
  }
}
