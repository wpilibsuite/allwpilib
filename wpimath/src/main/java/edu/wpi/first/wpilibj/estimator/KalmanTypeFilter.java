/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.estimator;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

@SuppressWarnings("ParameterName")
interface KalmanTypeFilter<S extends Num, I extends Num, O extends Num> {
  Matrix<S, S> getP();

  double getP(int i, int j);

  void setP(Matrix<S, S> newP);

  Matrix<S, N1> getXhat();

  double getXhat(int i);

  void setXhat(Matrix<S, N1> xHat);

  void setXhat(int i, double value);

  void reset();

  void predict(Matrix<I, N1> u, double dtSeconds);

  void correct(Matrix<I, N1> u, Matrix<O, N1> y);
}
