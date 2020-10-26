package edu.wpi.first.wpilibj.estimator;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;
import org.ejml.simple.SimpleMatrix;

public class AngleStatistics {
  public static <S extends Num> Matrix<S, N1> angleResidual(Matrix<S, N1> a, Matrix<S, N1> b, int angleStateIdx) {
    Matrix<S, N1> ret = a.minus(b);

    System.out.println(ret);

    double angle = ret.get(angleStateIdx, 0);
    angle = angle - 2 * Math.PI * Math.floor((angle + Math.PI) / Math.PI);
//    angle = angle % (2 * Math.PI);
//    if (angle > Math.PI) {
//      angle -= 2 * Math.PI;
//    }
    ret.set(angleStateIdx, 0, angle);

    return ret;
  }

  public static <S extends Num> Matrix<S, N1> angleMean(Matrix<S, ?> sigmas, Matrix<?, N1> Wm, int angleStateIdx) {
    double[] angleSigmas = sigmas.extractRowVector(angleStateIdx).getData();
    Matrix<N1, ?> sinAngleSigmas = new Matrix<>(new SimpleMatrix(1, sigmas.getNumCols()));
    Matrix<N1, ?> cosAngleSigmas = new Matrix<>(new SimpleMatrix(1, sigmas.getNumCols()));
    for (int i = 0; i < angleSigmas.length; i++) {
      sinAngleSigmas.set(0, i, Math.sin(angleSigmas[i]));
      cosAngleSigmas.set(0, i, Math.cos(angleSigmas[i]));
    }

    double sumSin = sinAngleSigmas.times(Matrix.changeBoundsUnchecked(Wm)).elementSum();
    double sumCos = cosAngleSigmas.times(Matrix.changeBoundsUnchecked(Wm)).elementSum();

    Matrix<S, N1> ret = sigmas.times(Matrix.changeBoundsUnchecked(Wm));
    ret.set(angleStateIdx, 0, Math.atan2(sumSin, sumCos));

    return ret;
  }
}
