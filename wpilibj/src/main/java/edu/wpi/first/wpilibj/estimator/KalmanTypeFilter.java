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
