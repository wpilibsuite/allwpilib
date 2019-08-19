package edu.wpi.first.wpilibj.examples.statespaceflywheel.subsystems;

import edu.wpi.first.wpilibj.controller.StateSpaceControllerCoeffs;
import edu.wpi.first.wpilibj.controller.StateSpaceLoop;
import edu.wpi.first.wpilibj.controller.StateSpaceObserverCoeffs;
import edu.wpi.first.wpilibj.controller.StateSpacePlantCoeffs;
import edu.wpi.first.wpiutil.math.*;
import edu.wpi.first.wpiutil.math.numbers.*;

public class FlywheelCoeffs {
  public static StateSpacePlantCoeffs<N1, N1, N1> makeFlywheelPlantCoeffs() {
    Matrix<N1, N1> A = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(0.9974775192550039);
    Matrix<N1, N1> B = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(0.6216972081698791);
    Matrix<N1, N1> C = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(1.0);
    Matrix<N1, N1> D = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(0.0);
    return new StateSpacePlantCoeffs<N1, N1, N1>(Nat.N1(), Nat.N1(), Nat.N1(), A, B, C, D);
  }

  public static StateSpaceControllerCoeffs<N1, N1, N1>
    makeFlywheelControllerCoeffs() {
    Matrix<N1, N1> K = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(0.8623214751211266);
    Matrix<N1, N1> Kff = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(0.6200031001383457);
    Matrix<N1, N1> Umin = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(-12.0);
    Matrix<N1, N1> Umax = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(12.0);
    return new StateSpaceControllerCoeffs<N1, N1, N1>(K, Kff, Umin, Umax);
  }

  public static StateSpaceObserverCoeffs<N1, N1, N1>
    makeFlywheelObserverCoeffs() {
    Matrix<N1, N1> K = MatrixUtils.mat(Nat.N1(), Nat.N1()).fill(0.9999000199446406);
    return new StateSpaceObserverCoeffs<N1, N1, N1>(K);
  }

  public static StateSpaceLoop<N1, N1, N1> makeFlywheelLoop() {
    return new StateSpaceLoop<N1, N1, N1>(makeFlywheelPlantCoeffs(),
                                          makeFlywheelControllerCoeffs(),
                                          makeFlywheelObserverCoeffs());
  }
}
