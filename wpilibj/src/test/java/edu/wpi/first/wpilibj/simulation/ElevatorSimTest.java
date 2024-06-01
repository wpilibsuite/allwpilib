// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.controller.PIDController;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.motorcontrol.PWMVictorSPX;
import org.junit.jupiter.api.Test;

class ElevatorSimTest {
  @Test
  void testStateSpaceSimWithElevator() {
    RoboRioSim.resetData();

    @SuppressWarnings("resource")
    var controller = new PIDController(10, 0, 0);

    var sim =
        new ElevatorSim(
            DCMotor.getVex775Pro(4),
            14.67,
            8,
            0.75 * 25.4 / 1000.0,
            0.0,
            3.0,
            true,
            0.0,
            0.01,
            0.0);

    try (var motor = new PWMVictorSPX(0);
        var encoder = new Encoder(0, 1)) {
      var encoderSim = new EncoderSim(encoder);

      for (int i = 0; i < 100; i++) {
        controller.setSetpoint(2.0);

        double nextVoltage = controller.calculate(encoderSim.getDistance());

        double currentBatteryVoltage = RobotController.getBatteryVoltage();
        motor.set(nextVoltage / currentBatteryVoltage);

        // ------ SimulationPeriodic() happens after user code -------

        var u = VecBuilder.fill(motor.get() * currentBatteryVoltage);
        sim.setInput(u);
        sim.update(0.020);
        var y = sim.getOutput();
        encoderSim.setDistance(y.get(0, 0));
      }

      assertEquals(controller.getSetpoint(), sim.getPositionMeters(), 0.2);
    }
  }

  @Test
  void testMinMax() {
    var sim =
        new ElevatorSim(
            DCMotor.getVex775Pro(4),
            14.67,
            8.0,
            0.75 * 25.4 / 1000.0,
            0.0,
            1.0,
            true,
            0.0,
            0.01,
            0.0);

    for (int i = 0; i < 100; i++) {
      sim.setInput(VecBuilder.fill(0));
      sim.update(0.020);
      var height = sim.getPositionMeters();
      assertTrue(height >= -0.05);
    }

    for (int i = 0; i < 100; i++) {
      sim.setInput(VecBuilder.fill(12.0));
      sim.update(0.020);
      var height = sim.getPositionMeters();
      assertTrue(height <= 1.05);
    }
  }

  @Test
  void testStability() {
    var sim =
        new ElevatorSim(
            DCMotor.getVex775Pro(4), 100, 4, Units.inchesToMeters(0.5), 0, 10, false, 0.0);

    sim.setState(VecBuilder.fill(0, 0));
    sim.setInput(12);
    for (int i = 0; i < 50; ++i) {
      sim.update(0.02);
    }

    var system =
        LinearSystemId.createElevatorSystem(
            DCMotor.getVex775Pro(4), 4, Units.inchesToMeters(0.5), 100);
    assertEquals(
        system.calculateX(VecBuilder.fill(0, 0), VecBuilder.fill(12), 0.02 * 50.0).get(0, 0),
        sim.getPositionMeters(),
        0.01);
  }
}
