// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.Vector;
import edu.wpi.first.math.controller.LTVUnicycleController;
import edu.wpi.first.math.controller.LinearPlantInversionFeedforward;
import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N7;
import edu.wpi.first.math.system.NumericalIntegration;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.util.Units;
import java.util.List;
import org.junit.jupiter.api.Test;

class DifferentialDrivetrainSimTest {
  @Test
  void testCurrent() {
    var motor = DCMotor.getNEO(2);
    var plant =
        LinearSystemId.createDrivetrainVelocitySystem(
            motor, 50, Units.inchesToMeters(2), Units.inchesToMeters(12), 0.5, 1.0);
    var kinematics = new DifferentialDriveKinematics(Units.inchesToMeters(24));
    var sim =
        new DifferentialDrivetrainSim(
            plant, motor, 1, kinematics.trackwidth, Units.inchesToMeters(2), null);

    sim.setInputs(-12, -12);
    for (int i = 0; i < 10; i++) {
      sim.update(0.020);
    }
    assertTrue(sim.getCurrentDraw() > 0);

    sim.setInputs(12, 12);
    for (int i = 0; i < 20; i++) {
      sim.update(0.020);
    }
    assertTrue(sim.getCurrentDraw() > 0);

    sim.setInputs(-12, 12);
    for (int i = 0; i < 30; i++) {
      sim.update(0.020);
    }
    assertTrue(sim.getCurrentDraw() > 0);
  }

  @Test
  void testModelStability() {
    var motor = DCMotor.getNEO(2);
    var plant =
        LinearSystemId.createDrivetrainVelocitySystem(
            motor, 50, Units.inchesToMeters(2), Units.inchesToMeters(12), 2.0, 5.0);

    var kinematics = new DifferentialDriveKinematics(Units.inchesToMeters(24));
    var sim =
        new DifferentialDrivetrainSim(
            plant,
            motor,
            5,
            kinematics.trackwidth,
            Units.inchesToMeters(2),
            VecBuilder.fill(0, 0, 0.0001, 0.1, 0.1, 0.005, 0.005));

    sim.setInputs(2, 4);

    // 10 seconds should be enough time to verify stability
    for (int i = 0; i < 500; i++) {
      sim.update(0.020);
    }

    assertTrue(Math.abs(sim.getPose().getTranslation().getNorm()) < 100);
  }
}
