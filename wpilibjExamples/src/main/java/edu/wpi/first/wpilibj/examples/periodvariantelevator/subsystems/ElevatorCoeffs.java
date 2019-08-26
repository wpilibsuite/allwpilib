package edu.wpi.first.wpilibj.examples.periodvariantelevator.subsystems;

//CHECKSTYLE.OFF: AvoidStarImport
import edu.wpi.first.wpilibj.controller.PeriodVariantLoop;
import edu.wpi.first.wpilibj.controller.PeriodVariantObserverCoeffs;
import edu.wpi.first.wpilibj.controller.PeriodVariantPlantCoeffs;
import edu.wpi.first.wpilibj.controller.StateSpaceControllerCoeffs;
import edu.wpi.first.wpiutil.math.*;
import edu.wpi.first.wpiutil.math.numbers.*;
//CHECKSTYLE.ON

public final class ElevatorCoeffs {

  private ElevatorCoeffs() {
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static PeriodVariantPlantCoeffs<N2, N1, N1> makeElevatorPlantCoeffs() {
    Matrix<N2, N2> Acontinuous = MatrixUtils.mat(Nat.N2(), Nat.N2())
        .fill(0.0, 1.0, 0.0, -164.20904979369794);
    Matrix<N2, N1> Bcontinuous = MatrixUtils.mat(Nat.N2(), Nat.N1())
        .fill(0.0, 21.457377744727182);
    Matrix<N1, N2> C = MatrixUtils.mat(Nat.N1(), Nat.N2())
        .fill(1.0, 0.0);
    Matrix<N1, N1> D = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(0.0);
    return new PeriodVariantPlantCoeffs<N2, N1, N1>(Nat.N2(), Nat.N1(), Nat.N1(),
        Acontinuous, Bcontinuous, C, D);
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static StateSpaceControllerCoeffs<N2, N1, N1>
      makeElevatorControllerCoeffs() {
    Matrix<N1, N2> K = MatrixUtils.mat(Nat.N1(), Nat.N2())
        .fill(232.76812610676956, 5.540693882702349);
    Matrix<N1, N2> Kff = MatrixUtils.mat(Nat.N1(), Nat.N2())
        .fill(12.902151500051774, 11.23863895630079);
    Matrix<N1, N1> Umin = MatrixUtils.mat(Nat.N1(), Nat.N1())
        .fill(-12.0);
    Matrix<N1, N1> Umax = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(12.0);
    return new StateSpaceControllerCoeffs<N2, N1, N1>(K, Kff, Umin, Umax);
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static PeriodVariantObserverCoeffs<N2, N1, N1>
      makeElevatorObserverCoeffs() {
    Matrix<N2, N2> Qcontinuous = MatrixUtils.mat(Nat.N2(), Nat.N2())
        .fill(0.0025000000000000005, 0.0, 0.0, 1.0);
    Matrix<N1, N1> Rcontinuous = MatrixUtils.mat(Nat.N1(), Nat.N1())
        .fill(1e-08);
    Matrix<N2, N2> PsteadyState = MatrixUtils.mat(Nat.N2(), Nat.N2())
        .fill(9.99996023173184e-09, 7.34757941922686e-09,
            7.347579419241315e-09, 1.2335349474145199);
    return new PeriodVariantObserverCoeffs<N2, N1, N1>(Qcontinuous, Rcontinuous,
                                                       PsteadyState);
  }

  @SuppressWarnings("JavadocMethod")
  public static PeriodVariantLoop<N2, N1, N1> makeElevatorLoop() {
    return new PeriodVariantLoop<N2, N1, N1>(makeElevatorPlantCoeffs(),
                                             makeElevatorControllerCoeffs(),
                                             makeElevatorObserverCoeffs());
  }
}
