// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.motor.PWMVictorSPX;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.math.controller.PIDController;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.system.DCMotor;
import org.wpilib.math.system.Models;
import org.wpilib.math.util.Units;
import org.wpilib.system.RobotController;

class ElevatorSimTest {
  @Test
  void testStateSpaceSimWithElevator() {
    RoboRioSim.resetData();

    @SuppressWarnings("resource")
    var controller = new PIDController(10, 0, 0);

    var gearbox = DCMotor.getVex775Pro(4);
    var plant = Models.elevatorFromPhysicalConstants(gearbox, 8, 0.75 * 25.4 / 1000.0, 14.67);
    var sim =
        new ElevatorSim(plant, gearbox, 14.67, 9.8 / plant.getB(1, 0), 0.0, 3.0, 0.0, 0.01, 0.0);

    try (var motor = new PWMVictorSPX(0);
        var encoder = new Encoder(0, 1)) {
      var encoderSim = new EncoderSim(encoder);

      for (int i = 0; i < 100; i++) {
        controller.setSetpoint(2.0);

        double nextVoltage = controller.calculate(encoderSim.getDistance());

        double currentBatteryVoltage = RobotController.getBatteryVoltage();
        motor.setThrottle(nextVoltage / currentBatteryVoltage);

        // ------ SimulationPeriodic() happens after user code -------

        var u = VecBuilder.fill(motor.getThrottle() * currentBatteryVoltage);
        sim.setInput(u);
        sim.update(0.020);
        var y = sim.getOutput();
        encoderSim.setDistance(y.get(0, 0));
      }

      assertEquals(controller.getSetpoint(), sim.getPosition(), 0.2);
    }
  }

  @Test
  void testInitialState() {
    double startingHeightMeters = 0.5;
    var gearbox = DCMotor.getKrakenX60(2);
    var plant = Models.elevatorFromPhysicalConstants(gearbox, 8.0, 0.1, 20);
    var sim =
        new ElevatorSim(
            plant, gearbox, 20, 9.8 / plant.getB(1, 0), 0.0, 1.0, startingHeightMeters, 0.01, 0.0);

    assertEquals(startingHeightMeters, sim.getPosition());
    assertEquals(0, sim.getVelocity());
  }

  @Test
  void testMinMax() {
    var gearbox = DCMotor.getVex775Pro(4);
    var plant = Models.elevatorFromPhysicalConstants(gearbox, 8.0, 0.75 * 25.4 / 1000.0, 14.67);
    var sim = new ElevatorSim(plant, gearbox, 14.67, 9.8 / plant.getB(1, 0), 0.0, 1.0, 0.0);

    for (int i = 0; i < 100; i++) {
      sim.setInput(VecBuilder.fill(0));
      sim.update(0.020);
      var height = sim.getPosition();
      assertTrue(height >= 0.0);
    }

    for (int i = 0; i < 100; i++) {
      sim.setInput(VecBuilder.fill(12.0));
      sim.update(0.020);
      var height = sim.getPosition();
      assertTrue(height <= 1.0);
    }
  }

  @Test
  void testStability() {
    var gearbox = DCMotor.getVex775Pro(4);
    var plant = Models.elevatorFromPhysicalConstants(gearbox, 4, Units.inchesToMeters(0.5), 100);
    var sim = new ElevatorSim(plant, gearbox, 100, 0.0, 0, 10, 0.0);

    sim.setState(VecBuilder.fill(0, 0));
    sim.setInput(12);
    for (int i = 0; i < 50; ++i) {
      sim.update(0.02);
    }

    // This plant's velocity pole is near -22000, so its time constant is far shorter than the
    // 20 ms timestep. The sim must stay stable and settle at the steady-state velocity
    // -B₁,₀u/A₁,₁ rather than diverging.
    double steadyStateVelocity = -plant.getB(1, 0) * 12.0 / plant.getA(1, 1);
    assertEquals(steadyStateVelocity, sim.getVelocity(), 1e-6);
    assertEquals(steadyStateVelocity * 0.02 * 50.0, sim.getPosition(), 0.01);
  }

  @Test
  void testCurrentDraw() {
    RoboRioSim.resetData();

    var motor = DCMotor.getKrakenX60(2);
    var plant = Models.elevatorFromPhysicalConstants(motor, 8.0, 0.1, 20);
    var sim = new ElevatorSim(plant, motor, 20, 9.8 / plant.getB(1, 0), 0.0, 1.0, 0.0, 0.01, 0.0);

    assertEquals(0.0, sim.getCurrentDraw());

    // Apply the voltage that pushes 60 A through the motor at zero velocity. getCurrentDraw()
    // reports battery-side current, so the expected draw is that scaled by the duty cycle.
    double appliedVoltage = motor.getVoltage(motor.getTorque(60.0), 0.0);
    double dutyCycle = appliedVoltage / RobotController.getBatteryVoltage();
    sim.setInputVoltage(appliedVoltage);
    assertEquals(60.0 * dutyCycle, sim.getCurrentDraw(), 1e-9);

    // Current draw decreases as the back-EMF catches up.
    sim.update(0.100);
    assertTrue(0.0 < sim.getCurrentDraw() && sim.getCurrentDraw() < 60.0 * dutyCycle);
  }
}
