// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.system;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.util.Nat;

class ModelsTest {
  @Test
  void testFlywheelFromPhysicalConstants() {
    var model = Models.flywheelFromPhysicalConstants(DCMotor.getNEO(2), 0.00032, 1.0);
    assertTrue(model.getA().isEqual(VecBuilder.fill(-26.87032), 0.001));

    assertTrue(model.getB().isEqual(VecBuilder.fill(1354.166667), 0.001));

    assertTrue(model.getC().isEqual(VecBuilder.fill(1), 0.001));

    assertTrue(model.getD().isEqual(VecBuilder.fill(0), 0.001));
  }

  @Test
  void testFlywheelFromSysId() {
    var kv = 1.0;
    var ka = 0.5;
    var model = Models.flywheelFromSysId(kv, ka);

    assertEquals(model.getA(), VecBuilder.fill(-kv / ka));
    assertEquals(model.getB(), VecBuilder.fill(1 / ka));
  }

  @Test
  void testDifferentialDriveFromPhysicalConstants() {
    var model =
        Models.differentialDriveFromPhysicalConstants(DCMotor.getNEO(4), 70, 0.05, 0.4, 6.0, 6);
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
  void testElevatorFromPhysicalConstants() {
    var model = Models.elevatorFromPhysicalConstants(DCMotor.getNEO(2), 5, 0.05, 12);
    assertTrue(
        model.getA().isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), 0, 1, 0, -99.05473), 0.001));

    assertTrue(model.getB().isEqual(VecBuilder.fill(0, 20.8), 0.001));

    assertTrue(model.getC().isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 1), 0.001));

    assertTrue(model.getD().isEqual(VecBuilder.fill(0, 0), 0.001));
  }

  @Test
  void testElevatorFromSysId() {
    var kv = 1.0;
    var ka = 0.5;
    var model = Models.elevatorFromSysId(kv, ka);

    assertEquals(model.getA(), MatBuilder.fill(Nat.N2(), Nat.N2(), 0, 1, 0, -kv / ka));
    assertEquals(model.getB(), VecBuilder.fill(0, 1 / ka));
  }

  @Test
  void testSingleJointedArmFromSysId() {
    var kv = 1.0;
    var ka = 0.5;

    var model = Models.singleJointedArmFromSysId(kv, ka);

    assertTrue(
        model.getA().isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), 0.0, 1.0, 0.0, -kv / ka), 0.001));
    assertTrue(model.getB().isEqual(MatBuilder.fill(Nat.N2(), Nat.N1(), 0.0, 1.0 / ka), 0.001));
  }
}
