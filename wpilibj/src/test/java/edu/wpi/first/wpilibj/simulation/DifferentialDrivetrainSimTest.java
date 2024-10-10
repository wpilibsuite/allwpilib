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
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import edu.wpi.first.math.trajectory.constraint.DifferentialDriveKinematicsConstraint;
import edu.wpi.first.math.util.Units;
import java.util.List;
import org.junit.jupiter.api.Test;

class DifferentialDrivetrainSimTest {
  @Test
  void testConvergence() {
    var motor = DCMotor.getNEO(2);
    var plant =
        LinearSystemId.createDrivetrainVelocitySystem(
            motor, 50, Units.inchesToMeters(2), Units.inchesToMeters(12), 0.5, 1.0);

    var kinematics = new DifferentialDriveKinematics(Units.inchesToMeters(24));
    var sim =
        new DifferentialDrivetrainSim(
            plant,
            motor,
            1,
            kinematics.trackWidthMeters,
            Units.inchesToMeters(2),
            VecBuilder.fill(0, 0, 0.0001, 0.1, 0.1, 0.005, 0.005));

    var feedforward = new LinearPlantInversionFeedforward<>(plant, 0.020);
    var feedback = new LTVUnicycleController(0.020);
    feedforward.reset(VecBuilder.fill(0, 0));

    // ground truth
    Matrix<N7, N1> groundTruthX = new Vector<>(Nat.N7());

    var traj =
        TrajectoryGenerator.generateTrajectory(
            Pose2d.kZero,
            List.of(),
            new Pose2d(2, 2, Rotation2d.kZero),
            new TrajectoryConfig(1, 1)
                .addConstraint(new DifferentialDriveKinematicsConstraint(kinematics, 1)));

    for (double t = 0; t < traj.getTotalTimeSeconds(); t += 0.020) {
      var state = traj.sample(t);
      var feedbackOut = feedback.calculate(sim.getPose(), state);

      var wheelSpeeds = kinematics.toWheelSpeeds(feedbackOut);

      var voltages =
          feedforward.calculate(
              VecBuilder.fill(wheelSpeeds.leftMetersPerSecond, wheelSpeeds.rightMetersPerSecond));

      // Sim periodic code
      sim.setInputs(voltages.get(0, 0), voltages.get(1, 0));
      sim.update(0.020);

      // Update our ground truth
      groundTruthX = NumericalIntegration.rkdp(sim::getDynamics, groundTruthX, voltages, 0.020);
    }

    // 2 inch tolerance is OK since our ground truth is an approximation of the
    // ODE solution using RKDP anyway
    assertEquals(
        groundTruthX.get(DifferentialDrivetrainSim.State.kX.value, 0),
        sim.getState(DifferentialDrivetrainSim.State.kX),
        0.05);
    assertEquals(
        groundTruthX.get(DifferentialDrivetrainSim.State.kY.value, 0),
        sim.getState(DifferentialDrivetrainSim.State.kY),
        0.05);
    assertEquals(
        groundTruthX.get(DifferentialDrivetrainSim.State.kHeading.value, 0),
        sim.getState(DifferentialDrivetrainSim.State.kHeading),
        0.01);
  }

  @Test
  void testCurrent() {
    var motor = DCMotor.getNEO(2);
    var plant =
        LinearSystemId.createDrivetrainVelocitySystem(
            motor, 50, Units.inchesToMeters(2), Units.inchesToMeters(12), 0.5, 1.0);
    var kinematics = new DifferentialDriveKinematics(Units.inchesToMeters(24));
    var sim =
        new DifferentialDrivetrainSim(
            plant, motor, 1, kinematics.trackWidthMeters, Units.inchesToMeters(2), null);

    sim.setInputs(-12, -12);
    for (int i = 0; i < 10; i++) {
      sim.update(0.020);
    }
    assertTrue(sim.getCurrentDrawAmps() > 0);

    sim.setInputs(12, 12);
    for (int i = 0; i < 20; i++) {
      sim.update(0.020);
    }
    assertTrue(sim.getCurrentDrawAmps() > 0);

    sim.setInputs(-12, 12);
    for (int i = 0; i < 30; i++) {
      sim.update(0.020);
    }
    assertTrue(sim.getCurrentDrawAmps() > 0);
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
            kinematics.trackWidthMeters,
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
