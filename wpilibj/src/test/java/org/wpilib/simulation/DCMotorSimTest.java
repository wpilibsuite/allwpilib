// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.math.controller.PIDController;
import org.wpilib.math.numbers.N1;
import org.wpilib.math.numbers.N2;
import org.wpilib.math.system.LinearSystem;
import org.wpilib.math.system.plant.DCMotor;
import org.wpilib.math.system.plant.LinearSystemId;
import org.wpilib.Encoder;
import org.wpilib.RobotController;
import org.wpilib.motorcontrol.PWMVictorSPX;
import org.junit.jupiter.api.Test;

class DCMotorSimTest {
  @Test
  void testVoltageSteadyState() {
    RoboRioSim.resetData();

    DCMotor gearbox = DCMotor.getNEO(1);
    LinearSystem<N2, N1, N2> plant = LinearSystemId.createDCMotorSystem(gearbox, 0.0005, 1);
    DCMotorSim sim = new DCMotorSim(plant, gearbox);

    try (var motor = new PWMVictorSPX(0);
        var encoder = new Encoder(0, 1)) {
      var encoderSim = new EncoderSim(encoder);
      encoderSim.resetData();

      for (int i = 0; i < 100; i++) {
        motor.setVoltage(12);

        // ------ SimulationPeriodic() happens after user code -------
        RoboRioSim.setVInVoltage(
            BatterySim.calculateDefaultBatteryLoadedVoltage(sim.getCurrentDraw()));
        sim.setInputVoltage(motor.get() * RobotController.getBatteryVoltage());
        sim.update(0.020);
        encoderSim.setRate(sim.getAngularVelocity());
      }

      assertEquals(gearbox.Kv * 12, encoder.getRate(), 0.1);

      for (int i = 0; i < 100; i++) {
        motor.setVoltage(0);

        // ------ SimulationPeriodic() happens after user code -------
        RoboRioSim.setVInVoltage(
            BatterySim.calculateDefaultBatteryLoadedVoltage(sim.getCurrentDraw()));
        sim.setInputVoltage(motor.get() * RobotController.getBatteryVoltage());
        sim.update(0.020);
        encoderSim.setRate(sim.getAngularVelocity());
      }

      assertEquals(0, encoder.getRate(), 0.1);
    }
  }

  @Test
  void testPositionFeedbackControl() {
    RoboRioSim.resetData();

    DCMotor gearbox = DCMotor.getNEO(1);
    LinearSystem<N2, N1, N2> plant = LinearSystemId.createDCMotorSystem(gearbox, 0.0005, 1);
    DCMotorSim sim = new DCMotorSim(plant, gearbox);

    try (var motor = new PWMVictorSPX(0);
        var encoder = new Encoder(0, 1);
        var controller = new PIDController(0.04, 0.0, 0.001)) {
      var encoderSim = new EncoderSim(encoder);
      encoderSim.resetData();

      for (int i = 0; i < 140; i++) {
        motor.set(controller.calculate(encoder.getDistance(), 750));

        // ------ SimulationPeriodic() happens after user code -------
        RoboRioSim.setVInVoltage(
            BatterySim.calculateDefaultBatteryLoadedVoltage(sim.getCurrentDraw()));
        sim.setInputVoltage(motor.get() * RobotController.getBatteryVoltage());
        sim.update(0.020);
        encoderSim.setDistance(sim.getAngularPosition());
        encoderSim.setRate(sim.getAngularVelocity());
      }

      assertEquals(750, encoder.getDistance(), 1.0);
      assertEquals(0, encoder.getRate(), 0.1);
    }
  }
}
