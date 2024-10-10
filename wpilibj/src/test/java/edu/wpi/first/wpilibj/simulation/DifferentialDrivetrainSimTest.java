// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static edu.wpi.first.units.Units.Inches;
import static edu.wpi.first.units.Units.KilogramSquareMeters;
import static edu.wpi.first.units.Units.Kilograms;
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
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.numbers.N7;
import edu.wpi.first.math.system.NumericalIntegration;
import edu.wpi.first.math.system.plant.DCMotorType;
import edu.wpi.first.math.system.plant.Gearbox;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.system.plant.Wheel;
import edu.wpi.first.math.trajectory.TrajectoryConfig;
import edu.wpi.first.math.trajectory.TrajectoryGenerator;
import edu.wpi.first.math.trajectory.constraint.DifferentialDriveKinematicsConstraint;
import edu.wpi.first.math.util.Units;
import java.util.List;
import java.util.function.BiFunction;
import org.junit.jupiter.api.Test;

class DifferentialDrivetrainSimTest {
  @Test
  void testConvergence() {
    var gearbox = new Gearbox(2, DCMotorType.NEO);
    var wheel = new Wheel(gearbox, Inches.of(2.0));
    var plant = LinearSystemId.createDrivetrainSystem(wheel, 50, 0.5, 24);
    var trackRadius = 12.0;
    var kinematics = new DifferentialDriveKinematics(Units.inchesToMeters(24));
    var sim =
        new DifferentialDrivetrainSim(
            wheel,
            KilogramSquareMeters.of(0.5),
            Kilograms.of(50.0),
            Inches.of(24),
            0,
            0,
            0.0001,
            0.1,
            0.1,
            0.005,
            0.005);

    var feedforward = new LinearPlantInversionFeedforward<>(plant, 0.020);
    var feedback = new LTVUnicycleController(0.020);
    feedforward.reset(VecBuilder.fill(0, 0, 0, 0, 0, 0, 0));

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
              VecBuilder.fill(
                  state.poseMeters.getX(),
                  state.poseMeters.getY(),
                  state.poseMeters.getRotation().getRadians(),
                  wheelSpeeds.leftMetersPerSecond,
                  wheelSpeeds.rightMetersPerSecond,
                  0,
                  0));

      // Sim periodic code
      sim.setInputs(voltages.get(0, 0), voltages.get(1, 0));
      sim.update(0.020);

      BiFunction<Matrix<N7, N1>, Matrix<N2, N1>, Matrix<N7, N1>> dynamics =
          (x, u) -> {
            var B = new Matrix<>(Nat.N4(), Nat.N2());
            B.assignBlock(0, 0, plant.getB().block(4, 2, 3, 0));

            var A = new Matrix<>(Nat.N4(), Nat.N4());
            A.assignBlock(0, 0, plant.getA().block(4, 4, 3, 3));

            A.assignBlock(2, 0, Matrix.eye(Nat.N2()));

            var v = (x.get(3, 0) + x.get(4, 0)) / 2.0;

            var xdot = new Matrix<>(Nat.N7(), Nat.N1());
            xdot.set(0, 0, v * Math.cos(x.get(2, 0)));
            xdot.set(1, 0, v * Math.sin(x.get(2, 0)));
            xdot.set(2, 0, (x.get(4, 0) - x.get(3, 0)) / (2.0 * trackRadius));
            xdot.assignBlock(3, 0, A.times(x.block(Nat.N4(), Nat.N1(), 3, 0)).plus(B.times(u)));

            return xdot;
          };
      System.out.println("groundX: " + groundTruthX.get(0, 0));
      System.out.println("simX: " + sim.getPose().getX());

      // Update our ground truth
      groundTruthX = NumericalIntegration.rkdp(dynamics, groundTruthX, voltages, 0.020);
    }

    // 2 inch tolerance is OK since our ground truth is an approximation of the
    // ODE solution using RKDP anyway
    assertEquals(groundTruthX.get(0, 0), sim.getPose().getX(), 0.05);
    assertEquals(groundTruthX.get(1, 0), sim.getPose().getY(), 0.05);
    assertEquals(groundTruthX.get(2, 0), sim.getPose().getRotation().getRadians(), 0.01);
  }

  @Test
  void testCurrent() {
    var wheel = new Wheel(new Gearbox(2, DCMotorType.NEO), Inches.of(2.0));
    var sim =
        new DifferentialDrivetrainSim(
            wheel, KilogramSquareMeters.of(0.5), Kilograms.of(50.0), Inches.of(24.0));

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
    var gearbox = new Gearbox(2, DCMotorType.NEO, 5.0);
    var wheel = new Wheel(gearbox, Inches.of(2.0));

    var sim =
        new DifferentialDrivetrainSim(
            wheel,
            KilogramSquareMeters.of(2.0),
            Kilograms.of(50.0),
            Inches.of(24),
            0,
            0,
            0.0001,
            0.1,
            0.1,
            0.005,
            0.005);

    sim.setInputs(2, 4);

    // 10 seconds should be enough time to verify stability
    for (int i = 0; i < 500; i++) {
      sim.update(0.020);
    }

    assertTrue(Math.abs(sim.getPose().getTranslation().getNorm()) < 100);
  }
}
