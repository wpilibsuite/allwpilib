/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.examples.statespacedifferentialdrive.subsystems;

//CHECKSTYLE.OFF: AvoidStarImport
import edu.wpi.first.wpilibj.controller.StateSpaceControllerCoeffs;
import edu.wpi.first.wpilibj.controller.StateSpaceLoop;
import edu.wpi.first.wpilibj.controller.StateSpaceObserverCoeffs;
import edu.wpi.first.wpilibj.controller.StateSpacePlantCoeffs;
import edu.wpi.first.wpiutil.math.*;
import edu.wpi.first.wpiutil.math.numbers.*;
//CHECKSTYLE.ON

public final class DifferentialDriveCoeffs {
  private DifferentialDriveCoeffs() {
  }

  @SuppressWarnings({"LocalVariableName", "JavadocMethod"})
  public static StateSpacePlantCoeffs<N4, N2, N2>
      makeDifferentialDrivePlantCoeffs() {
    Matrix<N4, N4> A = MatrixUtils.mat(Nat.N4(), Nat.N4())
        .fill(1.0, 0.004986437590193592, 0.0, -8.774526130757167e-06,
            0.0, 0.974934867699476, 0.0, -0.003445714205898883, 0.0, -8.774526130757165e-06,
            1.0, 0.004986437590193592, 0.0, -0.003445714205898883, 0.0, 0.974934867699476);
    Matrix<N4, N2> B = MatrixUtils.mat(Nat.N4(), Nat.N2())
        .fill(2.2749821266747515e-05, 3.1405181424541263e-06, 0.008971140043321736,
            0.001233266368585487, 3.140518142454126e-06, 2.274982126674752e-05,
            0.0012332663685854868, 0.008971140043321736);
    Matrix<N2, N4> C = MatrixUtils.mat(Nat.N2(), Nat.N4())
        .fill(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    Matrix<N2, N2> D = MatrixUtils.mat(Nat.N2(), Nat.N2()).fill(0.0, 0.0, 0.0, 0.0);
    return new StateSpacePlantCoeffs<N4, N2, N2>(Nat.N4(), Nat.N2(), Nat.N2(), A, B, C, D);
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
  public static StateSpaceObserverCoeffs<N4, N2, N2>
      makeDifferentialDriveObserverCoeffs() {
    Matrix<N4, N2> K = MatrixUtils.mat(Nat.N4(), Nat.N2()).fill(0.999996302984025,
        -9.883770254689278e-09, 14.80896750833126, -0.5093915996139213, -9.883770254645686e-09,
        0.999996302984025, -0.5093915996139393, 14.808967508331486);
    return new StateSpaceObserverCoeffs<N4, N2, N2>(K);
  }

  @SuppressWarnings("JavadocMethod")
  public static StateSpaceLoop<N4, N2, N2> makeDifferentialDriveLoop() {
    return new StateSpaceLoop<N4, N2, N2>(makeDifferentialDrivePlantCoeffs(),
                                          makeDifferentialDriveControllerCoeffs(),
                                          makeDifferentialDriveObserverCoeffs());
  }
}
