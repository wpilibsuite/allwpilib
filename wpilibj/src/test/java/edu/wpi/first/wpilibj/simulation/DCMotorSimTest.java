// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.motorcontrol.PWMVictorSPX;
import org.junit.jupiter.api.Test;

class DCMotorSimTest {
  @Test
  void testVoltageSteadyState() {
    RoboRioSim.resetData();

    DCMotor gearbox = DCMotor.getNEO(1);
    DCMotorSim sim = new DCMotorSim(gearbox, 1.0, 0.0005);

    try (var motor = new PWMVictorSPX(0);
        var encoder = new Encoder(0, 1)) {
      var encoderSim = new EncoderSim(encoder);

      for (int i = 0; i < 100; i++) {
        motor.setVoltage(12);

        // ------ SimulationPeriodic() happens after user code -------
        sim.setInputVoltage(motor.get() * RobotController.getBatteryVoltage());
        sim.update(0.020);
        encoderSim.setRate(sim.getAngularVelocityRadPerSec());
      }

      assertEquals(gearbox.KvRadPerSecPerVolt * 12, encoder.getRate(), 0.1);

      for (int i = 0; i < 100; i++) {
        motor.setVoltage(0);

        // ------ SimulationPeriodic() happens after user code -------
        sim.setInputVoltage(motor.get() * RobotController.getBatteryVoltage());
        sim.update(0.020);
        encoderSim.setRate(sim.getAngularVelocityRadPerSec());
      }

      assertEquals(0, encoder.getRate(), 0.1);
    }
  }

