package edu.wpi.first.wpilibj.examples.periodvariantflywheel.subsystems;

//CHECKSTYLE.OFF: AvoidStarImport
import edu.wpi.first.wpilibj.controller.PeriodVariantLoop;
import edu.wpi.first.wpilibj.controller.PeriodVariantObserverCoeffs;
import edu.wpi.first.wpilibj.controller.PeriodVariantPlantCoeffs;
import edu.wpi.first.wpilibj.controller.StateSpaceControllerCoeffs;
import edu.wpi.first.wpiutil.math.*;
import edu.wpi.first.wpiutil.math.numbers.*;
//CHECKSTYLE.ON

public final class FlywheelCoeffs {

  private FlywheelCoeffs() {
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static PeriodVariantPlantCoeffs<N1, N1, N1> makeFlywheelPlantCoeffs() {
    Matrix<N1, N1> Acontinuous = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(-0.5001321900589898);
    Matrix<N1, N1> Bcontinuous = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(123.26388888888889);
    Matrix<N1, N1> C = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(1.0);
    Matrix<N1, N1> D = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(0.0);
    return new PeriodVariantPlantCoeffs<N1, N1, N1>(Nat.N1(), Nat.N1(), Nat.N1(),
        Acontinuous, Bcontinuous, C, D);
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static StateSpaceControllerCoeffs<N1, N1, N1>
      makeFlywheelControllerCoeffs() {
    Matrix<N1, N1> K = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(0.8623214751211266);
    Matrix<N1, N1> Kff = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(0.6200031001383457);
    Matrix<N1, N1> Umin = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(-12.0);
    Matrix<N1, N1> Umax = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(12.0);
    return new StateSpaceControllerCoeffs<N1, N1, N1>(K, Kff, Umin, Umax);
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static PeriodVariantObserverCoeffs<N1, N1, N1>
      makeFlywheelObserverCoeffs() {
    Matrix<N1, N1> Qcontinuous = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(1.0);
    Matrix<N1, N1> Rcontinuous = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(0.0001);
    Matrix<N1, N1> PsteadyState = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(9.999000199441811e-05);
    return new PeriodVariantObserverCoeffs<N1, N1, N1>(Qcontinuous, Rcontinuous,
                                                       PsteadyState);
  }

  @SuppressWarnings("JavadocMethod")
  public static PeriodVariantLoop<N1, N1, N1> makeFlywheelLoop() {
    return new PeriodVariantLoop<N1, N1, N1>(makeFlywheelPlantCoeffs(),
                                             makeFlywheelControllerCoeffs(),
                                             makeFlywheelObserverCoeffs());
  }
}
