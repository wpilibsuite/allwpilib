// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import edu.wpi.first.math.MatBuilder;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.numbers.N1;
import org.junit.jupiter.api.Test;

class SimpleMotorFeedforwardTest {
  @Test
  void testCalculate() {
    double Ks = 0.5;
    double Kv = 3.0;
    double Ka = 0.6;
    double dt = 0.02;

    var A = MatBuilder.fill(Nat.N1(), Nat.N1(), -Kv / Ka);
    var B = MatBuilder.fill(Nat.N1(), Nat.N1(), 1.0 / Ka);

    var plantInversion = new LinearPlantInversionFeedforward<N1, N1, N1>(A, B, dt);
    var simpleMotor = new SimpleMotorFeedforward(Ks, Kv, Ka, dt);

    var r = VecBuilder.fill(2.0);
    var nextR = VecBuilder.fill(3.0);
    double currentVelocity = 2.0; // rad/s
    double nextVelocity = 3.0; // rad/s

    assertEquals(
        37.52499583432516 + 0.5,
        simpleMotor.calculateWithVelocities(currentVelocity, nextVelocity),
        0.002);
    assertEquals(
        plantInversion.calculate(r, nextR).get(0, 0) + Ks,
        simpleMotor.calculateWithVelocities(currentVelocity, nextVelocity),
        0.002);

    // These won't match exactly. It's just an approximation to make sure they're
    // in the same ballpark.
    assertEquals(
        plantInversion.calculate(r, nextR).get(0, 0) + Ks,
        simpleMotor.calculateWithVelocities(currentVelocity, nextVelocity),
        2.0);
  }

  @Test
  void testNegativeGains() {
    double Ks = 0.5;
    double Kv = 3.5;
    double Ka = 3.5;
    double dt = 0.02;

    assertAll(
        () ->
            assertThrows(
                IllegalArgumentException.class, () -> new SimpleMotorFeedforward(Ks, -Kv, Ka, dt)),
        () ->
            assertThrows(
                IllegalArgumentException.class, () -> new SimpleMotorFeedforward(Ks, Kv, -Ka, dt)),
        () ->
            assertThrows(
                IllegalArgumentException.class, () -> new SimpleMotorFeedforward(Ks, Kv, Ka, 0)));
  }
}
