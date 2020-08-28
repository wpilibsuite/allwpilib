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

@SuppressWarnings({"ParameterName", "InterfaceTypeParameterName"})
interface KalmanTypeFilter<States extends Num, Inputs extends Num, Outputs extends Num> {
  Matrix<States, States> getP();

  double getP(int i, int j);

  void setP(Matrix<States, States> newP);

  Matrix<States, N1> getXhat();

  double getXhat(int i);

  void setXhat(Matrix<States, N1> xHat);

  void setXhat(int i, double value);

  void reset();

  void predict(Matrix<Inputs, N1> u, double dtSeconds);

  void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y);
}
