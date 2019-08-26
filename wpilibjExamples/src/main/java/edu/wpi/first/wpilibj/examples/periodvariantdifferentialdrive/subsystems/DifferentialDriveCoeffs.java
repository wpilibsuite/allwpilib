/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.periodvariantdifferentialdrive.subsystems;

//CHECKSTYLE.OFF: AvoidStarImport
import edu.wpi.first.wpilibj.controller.PeriodVariantLoop;
import edu.wpi.first.wpilibj.controller.PeriodVariantObserverCoeffs;
import edu.wpi.first.wpilibj.controller.PeriodVariantPlantCoeffs;
import edu.wpi.first.wpilibj.controller.StateSpaceControllerCoeffs;
import edu.wpi.first.wpiutil.math.*;
import edu.wpi.first.wpiutil.math.numbers.*;
//CHECKSTYLE.ON

public final class DifferentialDriveCoeffs {
  private DifferentialDriveCoeffs() {
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static PeriodVariantPlantCoeffs<N4, N2, N2>
      makeDifferentialDrivePlantCoeffs() {
    Matrix<N4, N4> Acontinuous = MatrixUtils.mat(Nat.N4(), Nat.N4())
        .fill(0.0, 1.0, 0.0, 0.0,
            0.0, -5.027892724619944, 0.0, -0.6998646867368526,
            0.0, 0.0, 0.0, 1.0, 0.0, -0.6998646867368526, 0.0, -5.027892724619944);
    Matrix<N4, N2> Bcontinuous = MatrixUtils.mat(Nat.N4(), Nat.N2())
        .fill(0.0, 0.0, 1.7995488399804316, 0.25049076305735457, 0.0,
            0.0, 0.25049076305735457, 1.7995488399804316);
    Matrix<N2, N4> C = MatrixUtils.mat(Nat.N2(), Nat.N4())
        .fill(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    Matrix<N2, N2> D = MatrixUtils.mat(Nat.N2(), Nat.N2()).fill(0.0, 0.0, 0.0, 0.0);
    return new PeriodVariantPlantCoeffs<N4, N2, N2>(Nat.N4(), Nat.N2(), Nat.N2(),
        Acontinuous, Bcontinuous, C, D);
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static StateSpaceControllerCoeffs<N4, N2, N2>
      makeDifferentialDriveControllerCoeffs() {
    Matrix<N2, N4> K = MatrixUtils.mat(Nat.N2(), Nat.N4())
        .fill(80.65341818675287, 12.715868897125914, -0.5333100328368774, -0.4931520992909172,
            -0.5333100328378465, -0.49315209929093684, 80.65341818675415, 12.715868897126045);
    Matrix<N2, N4> Kff = MatrixUtils.mat(Nat.N2(), Nat.N4())
        .fill(129.05358519143596, 1.272274192517092, 17.31489107452048,
            0.16996589760178937, 17.314891074520485, 0.16996589760178946,
            129.053585191436, 1.2722741925170924);
    Matrix<N2, N1> Umin = MatrixUtils.mat(Nat.N2(), Nat.N1()).fill(-12.0, -12.0);
    Matrix<N2, N1> Umax = MatrixUtils.mat(Nat.N2(), Nat.N1()).fill(12.0, 12.0);
    return new StateSpaceControllerCoeffs<N4, N2, N2>(K, Kff, Umin, Umax);
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static PeriodVariantObserverCoeffs<N4, N2, N2>
      makeDifferentialDriveObserverCoeffs() {
    Matrix<N4, N4> Qcontinuous = MatrixUtils.mat(Nat.N4(), Nat.N4())
        .fill(0.0025000000000000005, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0025000000000000005, 0.0, 0.0, 0.0, 0.0, 1.0);
    Matrix<N2, N2> Rcontinuous = MatrixUtils.mat(Nat.N2(), Nat.N2()).fill(1e-08, 0.0, 0.0, 1e-08);
    Matrix<N4, N4> PsteadyState = MatrixUtils.mat(Nat.N4(), Nat.N4())
        .fill(9.999963029693572e-09, 1.480896750811403e-07, -9.883742854659257e-17,
            -5.093915996060619e-09, 1.4808967508016596e-07, 8.238974828958966,
            -5.093915996016745e-09, -0.2618332351519213, -9.883754614491571e-17,
            -5.0939159960621195e-09, 9.999963029693453e-09, 1.4808967508114087e-07,
            -5.093915996044057e-09, -0.26183323515192136, 1.4808967508522786e-07,
            8.238974828958884);
    return new PeriodVariantObserverCoeffs<N4, N2, N2>(Qcontinuous, Rcontinuous,
                                                       PsteadyState);
  }

  @SuppressWarnings("JavadocMethod")
  public static PeriodVariantLoop<N4, N2, N2> makeDifferentialDriveLoop() {
    return new PeriodVariantLoop<N4, N2, N2>(makeDifferentialDrivePlantCoeffs(),
                                             makeDifferentialDriveControllerCoeffs(),
                                             makeDifferentialDriveObserverCoeffs());
  }
}
