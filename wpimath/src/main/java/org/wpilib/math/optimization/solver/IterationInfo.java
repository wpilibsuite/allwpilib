// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.optimization.solver;

import org.ejml.simple.SimpleMatrix;

/** Solver iteration information exposed to an iteration callback. */
public class IterationInfo {
  /** The solver iteration. */
  public final int iteration;

  /** The decision variables (dense internal storage). */
  public final SimpleMatrix x;

  /** The gradient of the cost function (sparse internal storage). */
  public final SimpleMatrix g;

  /** The Hessian of the Lagrangian (sparse internal storage). */
  public final SimpleMatrix H;

  /** The equality constraint Jacobian (sparse internal storage). */
  public final SimpleMatrix A_e;

  /** The inequality constraint Jacobian (sparse internal storage). */
  public final SimpleMatrix A_i;

  /**
   * Constructs iteration info.
   *
   * @param iteration The solver iteration.
   * @param x The decision variables (dense internal storage).
   * @param g The gradient of the cost function (sparse internal storage).
   * @param H The Hessian of the Lagrangian (sparse internal storage).
   * @param A_e The equality constraint Jacobian (sparse internal storage).
   * @param A_i The inequality constraint Jacobian (sparse internal storage).
   */
  public IterationInfo(
      int iteration,
      SimpleMatrix x,
      SimpleMatrix g,
      SimpleMatrix H,
      SimpleMatrix A_e,
      SimpleMatrix A_i) {
    this.iteration = iteration;
    this.x = x;
    this.g = g;
    this.H = H;
    this.A_e = A_e;
    this.A_i = A_i;
  }
}
