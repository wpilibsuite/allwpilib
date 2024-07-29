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
                MatBuilder.fill(Nat.N2(), Nat.N2(), -10.14132, 3.06598, 3.06598, -10.14132),
                0.001));

    assertTrue(
        model
            .getB()
            .isEqual(
                MatBuilder.fill(Nat.N2(), Nat.N2(), 4.2590, -1.28762, -1.2876, 4.2590), 0.001));

    assertTrue(
        model.getC().isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), 1.0, 0.0, 0.0, 1.0), 0.001));

    assertTrue(
        model.getD().isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), 0.0, 0.0, 0.0, 0.0), 0.001));
  }

  @Test
  void testElevatorSystem() {
    var model = LinearSystemId.createElevatorSystem(DCMotor.getNEO(2), 5, 0.05, 12);
    assertTrue(
        model.getA().isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), 0, 1, 0, -99.05473), 0.001));

    assertTrue(model.getB().isEqual(VecBuilder.fill(0, 20.8), 0.001));

    assertTrue(model.getC().isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 1), 0.001));

    assertTrue(model.getD().isEqual(VecBuilder.fill(0, 0), 0.001));
  }

  @Test
  void testFlywheelSystem() {
    var model = LinearSystemId.createFlywheelSystem(DCMotor.getNEO(2), 0.00032, 1.0);
    assertTrue(model.getA().isEqual(VecBuilder.fill(-26.87032), 0.001));

    assertTrue(model.getB().isEqual(VecBuilder.fill(1354.166667), 0.001));

    assertTrue(model.getC().isEqual(VecBuilder.fill(1), 0.001));

    assertTrue(model.getD().isEqual(VecBuilder.fill(0), 0.001));
  }

  @Test
  void testDCMotorSystem() {
    var model = LinearSystemId.createDCMotorSystem(DCMotor.getNEO(2), 0.00032, 1.0);
    assertTrue(
        model.getA().isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), 0, 1, 0, -26.87032), 0.001));

    assertTrue(model.getB().isEqual(VecBuilder.fill(0, 1354.166667), 0.001));

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
