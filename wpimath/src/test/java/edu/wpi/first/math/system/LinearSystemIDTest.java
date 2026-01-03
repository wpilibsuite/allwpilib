// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import org.junit.jupiter.api.Test;

class LinearSystemIDTest {
  @Test
  void testDrivetrainVelocitySystem() {
    var model =
        LinearSystemId.createDrivetrainVelocitySystem(DCMotor.getNEO(4), 70, 0.05, 0.4, 6.0, 6);
    assertTrue(
        model
            .getA()
            .isEqual(
                MatBuilder.fill(Nat.N2(), Nat.N2(), -10.3182, 3.11946, 3.11946, -10.3182),
                0.001));

    assertTrue(
        model
            .getB()
            .isEqual(
                MatBuilder.fill(Nat.N2(), Nat.N2(), 4.33333, -1.31008, -1.31008, 4.33333), 0.001));

    assertTrue(
        model.getC().isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 0.0, 0.0, 1.0), 0.001));

    assertTrue(
        model.getD().isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), 0.0, 0.0, 0.0, 0.0), 0.001));
  }

  @Test
  void testElevatorSystem() {
    var model = LinearSystemId.createElevatorSystem(DCMotor.getNEO(2), 5, 0.05, 12);
    assertTrue(
        model.getA().isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), 0, 1, 0, -100.782), 0.001));

    assertTrue(model.getB().isEqual(VecBuilder.fill(0, 21.1628), 0.001));

    assertTrue(model.getC().isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 1), 0.001));

    assertTrue(model.getD().isEqual(VecBuilder.fill(0, 0), 0.001));
  }

  @Test
  void testFlywheelSystem() {
    var model = LinearSystemId.createFlywheelSystem(DCMotor.getNEO(2), 0.00032, 1.0);
    assertTrue(model.getA().isEqual(VecBuilder.fill(-27.339), 0.001));

    assertTrue(model.getB().isEqual(VecBuilder.fill(1377.785), 0.001));

    assertTrue(model.getC().isEqual(VecBuilder.fill(1), 0.001));

    assertTrue(model.getD().isEqual(VecBuilder.fill(0), 0.001));
  }

  @Test
  void testDCMotorSystem() {
    var model = LinearSystemId.createDCMotorSystem(DCMotor.getNEO(2), 0.00032, 1.0);
    assertTrue(
        model.getA().isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), 0, 1, 0, -27.339), 0.001));

    assertTrue(model.getB().isEqual(VecBuilder.fill(0, 1377.785), 0.001));

    assertTrue(model.getC().isEqual(Matrix.eye(Nat.N2()), 0.001));

    assertTrue(model.getD().isEqual(new Matrix<>(Nat.N2(), Nat.N1()), 0.001));
  }

  @Test
  void testIdentifyPositionSystem() {
    // By controls engineering in frc,
    // x-dot = [0 1 | 0 -kv/ka] x = [0 | 1/ka] u
    var kv = 1.0;
    var ka = 0.5;
    var model = LinearSystemId.identifyPositionSystem(kv, ka);

    assertEquals(model.getA(), MatBuilder.fill(Nat.N2(), Nat.N2(), 0, 1, 0, -kv / ka));
    assertEquals(model.getB(), VecBuilder.fill(0, 1 / ka));
  }

  @Test
  void testIdentifyVelocitySystem() {
    // By controls engineering in frc,
    // V = kv * velocity + ka * acceleration
    // x-dot =  -kv/ka * v + 1/ka \cdot V
    var kv = 1.0;
    var ka = 0.5;
    var model = LinearSystemId.identifyVelocitySystem(kv, ka);

    assertEquals(model.getA(), VecBuilder.fill(-kv / ka));
    assertEquals(model.getB(), VecBuilder.fill(1 / ka));
  }
}
