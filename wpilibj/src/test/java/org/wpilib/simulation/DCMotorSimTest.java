// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.system.LinearSystem;
import org.wpilib.math.system.Models;
import org.wpilib.system.RobotController;

class DCMotorSimTest {
  @Test
  void testVoltageSteadyState() {
    RoboRioSim.resetData();

    DCMotor gearbox = DCMotor.getNeo(1);
    LinearSystem<N2, N1, N2> plant =
        Models.angularPositionFromPhysicalConstants(gearbox, 0.0005, 1);
    DCMotorSim sim = new DCMotorSim(plant, gearbox);

    try (var motor = new TestPWMMotorController(0);
        var encoder = new Encoder(0, 1)) {
      var encoderSim = new EncoderSim(encoder);
      encoderSim.resetData();

      for (int i = 0; i < 100; i++) {
        motor.setVoltage(12);

        // ------ SimulationPeriodic() happens after user code -------
        RoboRioSim.setVInVoltage(
            BatterySim.calculateDefaultBatteryLoadedVoltage(sim.getCurrentDraw()));
        sim.setInputVoltage(motor.getThrottle() * RobotController.getBatteryVoltage());
        sim.update(0.020);
        encoderSim.setRate(sim.getAngularVelocity());
      }

      assertEquals(gearbox.Kv * 12, encoder.getRate(), 0.1);

      for (int i = 0; i < 100; i++) {
        motor.setVoltage(0);

        // ------ SimulationPeriodic() happens after user code -------
        RoboRioSim.setVInVoltage(
            BatterySim.calculateDefaultBatteryLoadedVoltage(sim.getCurrentDraw()));
        sim.setInputVoltage(motor.getThrottle() * RobotController.getBatteryVoltage());
        sim.update(0.020);
        encoderSim.setRate(sim.getAngularVelocity());
      }

      assertEquals(0, encoder.getRate(), 0.1);
    }
  }

  @Test
  void testPositionFeedbackControl() {
    RoboRioSim.resetData();

    DCMotor gearbox = DCMotor.getNeo(1);
    LinearSystem<N2, N1, N2> plant =
        Models.angularPositionFromPhysicalConstants(gearbox, 0.0005, 1);
    DCMotorSim sim = new DCMotorSim(plant, gearbox);

    try (var motor = new TestPWMMotorController(0);
        var encoder = new Encoder(0, 1)) {
      var controller = new PIDController(0.04, 0.0, 0.001);
      var encoderSim = new EncoderSim(encoder);
      encoderSim.resetData();

      for (int i = 0; i < 140; i++) {
        motor.setThrottle(controller.calculate(encoder.getDistance(), 750));

        // ------ SimulationPeriodic() happens after user code -------
        RoboRioSim.setVInVoltage(
            BatterySim.calculateDefaultBatteryLoadedVoltage(sim.getCurrentDraw()));
        sim.setInputVoltage(motor.getThrottle() * RobotController.getBatteryVoltage());
        sim.update(0.020);
        encoderSim.setDistance(sim.getAngularPosition());
        encoderSim.setRate(sim.getAngularVelocity());
      }

      assertEquals(750, encoder.getDistance(), 1.0);
      assertEquals(0, encoder.getRate(), 0.1);
    }
  }

  @Test
  void testCurrentDrawAtZeroVoltage() {
    RoboRioSim.resetData();
    RoboRioSim.setVInVoltage(12);

    DCMotor gearbox = DCMotor.getNeo(1);
    LinearSystem<N2, N1, N2> plant =
        Models.angularPositionFromPhysicalConstants(gearbox, 0.0005, 1);
    DCMotorSim sim = new DCMotorSim(plant, gearbox);

    // Spin up at full voltage so the motor has back-EMF to report
    sim.setInputVoltage(12);
    for (int i = 0; i < 100; i++) {
      sim.update(0.020);
    }
    assertTrue(sim.getAngularVelocity() > 0.0);

    // Cutting the voltage must not zero the current: the spinning motor still
    // drives a braking current through its windings
    sim.setInputVoltage(0);
    assertTrue(sim.getCurrentDraw() < 0.0);
    assertEquals(
        gearbox.getCurrent(sim.getAngularVelocity(), 0), sim.getCurrentDraw(), 1.0);
  }
}
