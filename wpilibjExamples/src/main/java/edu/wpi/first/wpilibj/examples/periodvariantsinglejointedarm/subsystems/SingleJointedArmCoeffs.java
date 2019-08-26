/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.periodvariantsinglejointedarm.subsystems;

//CHECKSTYLE.OFF: AvoidStarImport
import edu.wpi.first.wpilibj.controller.PeriodVariantLoop;
import edu.wpi.first.wpilibj.controller.PeriodVariantObserverCoeffs;
import edu.wpi.first.wpilibj.controller.PeriodVariantPlantCoeffs;
import edu.wpi.first.wpilibj.controller.StateSpaceControllerCoeffs;
import edu.wpi.first.wpiutil.math.*;
import edu.wpi.first.wpiutil.math.numbers.*;
//CHECKSTYLE.ON

public final class SingleJointedArmCoeffs {
  private SingleJointedArmCoeffs() {
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static PeriodVariantPlantCoeffs<N2, N1, N1>
      makeSingleJointedArmPlantCoeffs() {
    Matrix<N2, N2> Acontinuous = MatrixUtils.mat(Nat.N2(), Nat.N2())
        .fill(0.0, 1.0, 0.0, -0.000948744041441224);
    Matrix<N2, N1> Bcontinuous = MatrixUtils.mat(Nat.N2(), Nat.N1())
        .fill(0.0, 0.08974870179312819);
    Matrix<N1, N2> C = MatrixUtils.mat(Nat.N1(), Nat.N2())
        .fill(1.0, 0.0);
    Matrix<N1, N1> D = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(0.0);
    return new PeriodVariantPlantCoeffs<N2, N1, N1>(Nat.N2(), Nat.N1(), Nat.N1(),
        Acontinuous, Bcontinuous, C, D);
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static StateSpaceControllerCoeffs<N2, N1, N1>
      makeSingleJointedArmControllerCoeffs() {
    Matrix<N1, N2> K = MatrixUtils.mat(Nat.N1(), Nat.N2())
        .fill(659.4457876860411, 179.11558938712352);
    Matrix<N1, N2> Kff = MatrixUtils.mat(Nat.N1(), Nat.N2())
        .fill(0.5390980036447967, 8.538202281633405);
    Matrix<N1, N1> Umin = MatrixUtils.mat(Nat.N1(), Nat.N1())
        .fill(-12.0);
    Matrix<N1, N1> Umax = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(12.0);
    return new StateSpaceControllerCoeffs<N2, N1, N1>(K, Kff, Umin, Umax);
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static PeriodVariantObserverCoeffs<N2, N1, N1>
      makeSingleJointedArmObserverCoeffs() {
    Matrix<N2, N2> Qcontinuous = MatrixUtils.mat(Nat.N2(), Nat.N2())
        .fill(0.0003045025, 0.0, 0.0, 0.030461733182409998);
    Matrix<N1, N1> Rcontinuous = MatrixUtils.mat(Nat.N1(), Nat.N1())
        .fill(0.0001);
    Matrix<N2, N2> PsteadyState = MatrixUtils.mat(Nat.N2(), Nat.N2())
        .fill(8.033915976431203e-05, 0.0007738121910642457, 0.000773812191064246, 0.62614228094563);
    return new PeriodVariantObserverCoeffs<N2, N1, N1>(Qcontinuous, Rcontinuous,
                                                       PsteadyState);
  }

  @SuppressWarnings("JavadocMethod")
  public static PeriodVariantLoop<N2, N1, N1> makeSingleJointedArmLoop() {
    return new PeriodVariantLoop<N2, N1, N1>(makeSingleJointedArmPlantCoeffs(),
                                             makeSingleJointedArmControllerCoeffs(),
                                             makeSingleJointedArmObserverCoeffs());
  }
}
