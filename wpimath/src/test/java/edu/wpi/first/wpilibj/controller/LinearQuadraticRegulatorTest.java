/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;


import static org.junit.jupiter.api.Assertions.assertEquals;

public class LinearQuadraticRegulatorTest {
  public static LinearSystem<N2, N1, N1> elevatorPlant;
  static LinearSystem<N2, N1, N1> armPlant;

  static {
    createArm();
    createElevator();
  }

  @SuppressWarnings("LocalVariableName")
  public static void createArm() {
    var motors = DCMotor.getVex775Pro(2);

    var m = 4.0;
    var r = 0.4;
    var J = 1d / 3d * m * r * r;
    var G = 100.0;

    armPlant = LinearSystemId.createSingleJointedArmSystem(motors, J, G);
  }

  @SuppressWarnings("LocalVariableName")
  public static void createElevator() {
    var motors = DCMotor.getVex775Pro(2);

    var m = 5.0;
    var r = 0.0181864;
    var G = 1.0;
    elevatorPlant = LinearSystemId.createElevatorSystem(motors, m, r, G);
  }

  @Test
  @SuppressWarnings("LocalVariableName")
  public void testLQROnElevator() {

    var qElms = VecBuilder.fill(0.02, 0.4);
    var rElms = VecBuilder.fill(12.0);
    var dt = 0.00505;

    var controller = new LinearQuadraticRegulator<>(
          elevatorPlant, qElms, rElms, dt);

    var k = controller.getK();

    assertEquals(522.153, k.get(0, 0), 0.1);
    assertEquals(38.2, k.get(0, 1), 0.1);
  }

  @Test
  public void testFourMotorElevator() {

    var dt = 0.020;

    var plant = LinearSystemId.createElevatorSystem(
          DCMotor.getVex775Pro(4),
          8.0,
          0.75 * 25.4 / 1000.0,
          14.67);

    var regulator = new LinearQuadraticRegulator<>(
          plant,
          VecBuilder.fill(0.1, 0.2),
          VecBuilder.fill(12.0),
          dt);

    assertEquals(10.381, regulator.getK().get(0, 0), 1e-2);
    assertEquals(0.6929, regulator.getK().get(0, 1), 1e-2);

  }

  @Test
  @SuppressWarnings("LocalVariableName")
  public void testLQROnArm() {

    var motors = DCMotor.getVex775Pro(2);

    var m = 4.0;
    var r = 0.4;
    var G = 100.0;

    var plant = LinearSystemId.createSingleJointedArmSystem(motors, 1d / 3d * m * r * r, G);

    var qElms = VecBuilder.fill(0.01745, 0.08726);
    var rElms = VecBuilder.fill(12.0);
    var dt = 0.00505;

    var controller = new LinearQuadraticRegulator<>(
          plant, qElms, rElms, dt);

    var k = controller.getK();

    assertEquals(19.16, k.get(0, 0), 0.1);
    assertEquals(3.32, k.get(0, 1), 0.1);
  }

  @Test
  public void testLatencyCompensate() {
    var dt = 0.02;

    var plant = LinearSystemId.createElevatorSystem(
          DCMotor.getVex775Pro(4),
          8.0,
          0.75 * 25.4 / 1000.0,
          14.67);

    var regulator = new LinearQuadraticRegulator<>(
          plant,
          VecBuilder.fill(0.1, 0.2),
          VecBuilder.fill(12.0),
          dt);

    regulator.latencyCompensate(plant, dt, 0.01);

    assertEquals(8.97115941, regulator.getK().get(0, 0), 1e-3);
    assertEquals(0.07904881, regulator.getK().get(0, 1), 1e-3);
  }
}
