package edu.wpi.first.wpilibj.examples.statespacesinglejointedarm.subsystems;

//CHECKSTYLE.OFF: AvoidStarImport
import edu.wpi.first.wpilibj.controller.StateSpaceControllerCoeffs;
import edu.wpi.first.wpilibj.controller.StateSpaceLoop;
import edu.wpi.first.wpilibj.controller.StateSpaceObserverCoeffs;
import edu.wpi.first.wpilibj.controller.StateSpacePlantCoeffs;
import edu.wpi.first.wpiutil.math.*;
import edu.wpi.first.wpiutil.math.numbers.*;
//CHECKSTYLE.ON

public final class SingleJointedArmCoeffs {

  private SingleJointedArmCoeffs() {
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static StateSpacePlantCoeffs<N2, N1, N1>
      makeSingleJointedArmPlantCoeffs() {
    Matrix<N2, N2> A = MatrixUtils.mat(Nat.N2(), Nat.N2())
        .fill(1.0, 0.005049987902346862, 0.0, 0.9999952088540683);
    Matrix<N2, N1> B = MatrixUtils.mat(Nat.N2(), Nat.N1())
        .fill(1.1444063060619785e-06, 0.0004532298583066335);
    Matrix<N1, N2> C = MatrixUtils.mat(Nat.N1(), Nat.N2()).fill(1.0, 0.0);
    Matrix<N1, N1> D = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(0.0);
    return new StateSpacePlantCoeffs<N2, N1, N1>(Nat.N2(), Nat.N1(), Nat.N1(), A, B, C, D);
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static StateSpaceControllerCoeffs<N2, N1, N1>
      makeSingleJointedArmControllerCoeffs() {
    Matrix<N1, N2> K = MatrixUtils.mat(Nat.N1(), Nat.N2())
        .fill(659.4457876860411, 179.11558938712352);
    Matrix<N1, N2> Kff = MatrixUtils.mat(Nat.N1(), Nat.N2())
        .fill(0.5390980036447967, 8.538202281633405);
    Matrix<N1, N1> Umin = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(-12.0);
    Matrix<N1, N1> Umax = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(12.0);
    return new StateSpaceControllerCoeffs<N2, N1, N1>(K, Kff, Umin, Umax);
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static StateSpaceObserverCoeffs<N2, N1, N1>
      makeSingleJointedArmObserverCoeffs() {
    Matrix<N2, N1> K = MatrixUtils.mat(Nat.N2(), Nat.N1())
        .fill(0.8033915976431203, 7.738121910642457);
    return new StateSpaceObserverCoeffs<N2, N1, N1>(K);
  }

  @SuppressWarnings("JavadocMethod")
  public static StateSpaceLoop<N2, N1, N1> makeSingleJointedArmLoop() {
    return new StateSpaceLoop<N2, N1, N1>(makeSingleJointedArmPlantCoeffs(),
                                          makeSingleJointedArmControllerCoeffs(),
                                          makeSingleJointedArmObserverCoeffs());
  }
}
