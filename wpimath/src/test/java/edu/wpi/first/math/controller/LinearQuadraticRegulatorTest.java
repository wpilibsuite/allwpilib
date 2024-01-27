// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.system.Discretization;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import org.junit.jupiter.api.Test;

class LinearQuadraticRegulatorTest {
  @Test
  void testLQROnElevator() {
    var motors = DCMotor.getVex775Pro(2);

    var m = 5.0;
    var r = 0.0181864;
    var G = 1.0;

    var plant = LinearSystemId.createElevatorSystem(motors, m, r, G);

    var qElms = VecBuilder.fill(0.02, 0.4);
    var rElms = VecBuilder.fill(12.0);
    var dt = 0.00505;

    var K = new LinearQuadraticRegulator<>(plant, qElms, rElms, dt).getK();

    assertEquals(522.153, K.get(0, 0), 0.1);
    assertEquals(38.2, K.get(0, 1), 0.1);
  }

  @Test
  void testFourMotorElevator() {
    var dt = 0.020;

    var plant =
        LinearSystemId.createElevatorSystem(
            DCMotor.getVex775Pro(4), 8.0, 0.75 * 25.4 / 1000.0, 14.67);

    var K =
        new LinearQuadraticRegulator<>(plant, VecBuilder.fill(0.1, 0.2), VecBuilder.fill(12.0), dt)
            .getK();

    assertEquals(10.381, K.get(0, 0), 1e-2);
    assertEquals(0.6929, K.get(0, 1), 1e-2);
  }

  @Test
  void testLQROnArm() {
    var motors = DCMotor.getVex775Pro(2);

    var m = 4.0;
    var r = 0.4;
    var G = 100.0;

    var plant = LinearSystemId.createSingleJointedArmSystem(motors, 1d / 3d * m * r * r, G);

    var qElms = VecBuilder.fill(0.01745, 0.08726);
    var rElms = VecBuilder.fill(12.0);
    var dt = 0.00505;

    var K = new LinearQuadraticRegulator<>(plant, qElms, rElms, dt).getK();

    assertEquals(19.16, K.get(0, 0), 0.1);
    assertEquals(3.32, K.get(0, 1), 0.1);
  }

  /**
   * Returns feedback control gain for implicit model following.
   *
   * <p>This is used to test the QRN overload of LQR.
   *
   * @param <States> Number of states.
   * @param <Inputs> Number of inputs.
   * @param A State matrix.
   * @param B Input matrix.
   * @param Q State cost matrix.
   * @param R Input cost matrix.
   * @param Aref Desired state matrix.
   * @param dtSeconds Discretization timestep in seconds.
   */
  <States extends Num, Inputs extends Num> Matrix<Inputs, States> getImplicitModelFollowingK(
      Matrix<States, States> A,
      Matrix<States, Inputs> B,
      Matrix<States, States> Q,
      Matrix<Inputs, Inputs> R,
      Matrix<States, States> Aref,
      double dtSeconds) {
    // Discretize real dynamics
    var discABPair = Discretization.discretizeAB(A, B, dtSeconds);
    var discA = discABPair.getFirst();
    var discB = discABPair.getSecond();

    // Discretize desired dynamics
    var discAref = Discretization.discretizeA(Aref, dtSeconds);

    Matrix<States, States> Qimf =
        (discA.minus(discAref)).transpose().times(Q).times(discA.minus(discAref));
    Matrix<Inputs, Inputs> Rimf = discB.transpose().times(Q).times(discB).plus(R);
    Matrix<States, Inputs> Nimf = (discA.minus(discAref)).transpose().times(Q).times(discB);

    return new LinearQuadraticRegulator<>(A, B, Qimf, Rimf, Nimf, dtSeconds).getK();
  }

  @Test
  void testMatrixOverloadsWithSingleIntegrator() {
    var A = MatBuilder.fill(Nat.N2(), Nat.N2(), 0, 0, 0, 0);
    var B = MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 1);
    var Q = MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 1);
    var R = MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 1);

    // QR overload
    var K = new LinearQuadraticRegulator<>(A, B, Q, R, 0.005).getK();
    assertEquals(0.9975031249951226, K.get(0, 0), 1e-10);
    assertEquals(0.0, K.get(0, 1), 1e-10);
    assertEquals(0.0, K.get(1, 0), 1e-10);
    assertEquals(0.9975031249951226, K.get(1, 1), 1e-10);

    // QRN overload
    var N = MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 1);
    var Kimf = new LinearQuadraticRegulator<>(A, B, Q, R, N, 0.005).getK();
    assertEquals(1.0, Kimf.get(0, 0), 1e-10);
    assertEquals(0.0, Kimf.get(0, 1), 1e-10);
    assertEquals(0.0, Kimf.get(1, 0), 1e-10);
    assertEquals(1.0, Kimf.get(1, 1), 1e-10);
  }

  @Test
  void testMatrixOverloadsWithDoubleIntegrator() {
    double Kv = 3.02;
    double Ka = 0.642;

    var A = MatBuilder.fill(Nat.N2(), Nat.N2(), 0, 1, 0, -Kv / Ka);
    var B = MatBuilder.fill(Nat.N2(), Nat.N1(), 0, 1.0 / Ka);
    var Q = MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 0.2);
    var R = MatBuilder.fill(Nat.N1(), Nat.N1(), 0.25);

    // QR overload
    var K = new LinearQuadraticRegulator<>(A, B, Q, R, 0.005).getK();
    assertEquals(1.9960017786537287, K.get(0, 0), 1e-10);
    assertEquals(0.5118212835109273, K.get(0, 1), 1e-10);

    // QRN overload
    var Aref = MatBuilder.fill(Nat.N2(), Nat.N2(), 0, 1, 0, -Kv / (Ka * 5.0));
    var Kimf = getImplicitModelFollowingK(A, B, Q, R, Aref, 0.005);
    assertEquals(0.0, Kimf.get(0, 0), 1e-10);
    assertEquals(-6.919050011675146e-05, Kimf.get(0, 1), 1e-10);
  }

  @Test
  void testLatencyCompensate() {
    var dt = 0.02;

    var plant =
        LinearSystemId.createElevatorSystem(
            DCMotor.getVex775Pro(4), 8.0, 0.75 * 25.4 / 1000.0, 14.67);

    var regulator =
        new LinearQuadraticRegulator<>(plant, VecBuilder.fill(0.1, 0.2), VecBuilder.fill(12.0), dt);

    regulator.latencyCompensate(plant, dt, 0.01);

    assertEquals(8.97115941, regulator.getK().get(0, 0), 1e-3);
    assertEquals(0.07904881, regulator.getK().get(0, 1), 1e-3);
  }
}
