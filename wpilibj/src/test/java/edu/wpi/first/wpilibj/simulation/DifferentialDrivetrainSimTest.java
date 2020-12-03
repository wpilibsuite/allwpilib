/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.wpilibj.controller.LinearPlantInversionFeedforward;
import edu.wpi.first.wpilibj.controller.RamseteController;
import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpilibj.kinematics.DifferentialDriveKinematics;
import edu.wpi.first.wpilibj.system.RungeKutta;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.trajectory.TrajectoryConfig;
import edu.wpi.first.wpilibj.trajectory.TrajectoryGenerator;
import edu.wpi.first.wpilibj.trajectory.constraint.DifferentialDriveKinematicsConstraint;
import edu.wpi.first.wpilibj.util.Units;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.Vector;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N7;

import org.junit.jupiter.api.Test;

import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DifferentialDrivetrainSimTest {
  @Test
  public void testConvergence() {
    var motor = DCMotor.getNEO(2);
    var plant = LinearSystemId.createDrivetrainVelocitySystem(
        motor,
        50,
        Units.inchesToMeters(2),
        Units.inchesToMeters(12),
        0.5,
        1.0);

    var kinematics = new DifferentialDriveKinematics(Units.inchesToMeters(24));
    var sim = new DifferentialDrivetrainSim(plant,
        motor, 1, kinematics.trackWidthMeters, Units.inchesToMeters(2), VecBuilder.fill(0, 0, 0.0001, 0.1, 0.1, 0.005, 0.005));

    var feedforward = new LinearPlantInversionFeedforward<>(plant, 0.020);
    var ramsete = new RamseteController();
    feedforward.reset(VecBuilder.fill(0, 0));

    // ground truth
    Matrix<N7, N1> groundTruthX = new Vector<>(Nat.N7());

    var traj = TrajectoryGenerator.generateTrajectory(
        new Pose2d(),
        List.of(),
        new Pose2d(2, 2, new Rotation2d()),
        new TrajectoryConfig(1, 1)
            .addConstraint(new DifferentialDriveKinematicsConstraint(kinematics, 1)));

    for (double t = 0; t < traj.getTotalTimeSeconds(); t += 0.020) {
      var state = traj.sample(0.020);
      var ramseteOut = ramsete.calculate(sim.getPose(), state);

      var wheelSpeeds = kinematics.toWheelSpeeds(ramseteOut);

      var voltages = feedforward.calculate(
          VecBuilder.fill(wheelSpeeds.leftMetersPerSecond, wheelSpeeds.rightMetersPerSecond));

      // Sim periodic code
      sim.setInputs(voltages.get(0, 0), voltages.get(1, 0));
      sim.update(0.020);

      // Update our ground truth
      groundTruthX = RungeKutta.rungeKutta(sim::getDynamics, groundTruthX, voltages, 0.020);
    }

    assertEquals(groundTruthX.get(DifferentialDrivetrainSim.State.kX.value, 0),
        sim.getState(DifferentialDrivetrainSim.State.kX));
    assertEquals(groundTruthX.get(DifferentialDrivetrainSim.State.kY.value, 0),
        sim.getState(DifferentialDrivetrainSim.State.kY));
    assertEquals(groundTruthX.get(DifferentialDrivetrainSim.State.kHeading.value, 0),
        sim.getState(DifferentialDrivetrainSim.State.kHeading));
  }

  @Test
  public void testCurrent() {
    var motor = DCMotor.getNEO(2);
    var plant = LinearSystemId.createDrivetrainVelocitySystem(
        motor,
        50,
        Units.inchesToMeters(2),
        Units.inchesToMeters(12),
        0.5,
        1.0);
    var kinematics = new DifferentialDriveKinematics(Units.inchesToMeters(24));
    var sim = new DifferentialDrivetrainSim(plant, motor, 1, kinematics.trackWidthMeters, Units.inchesToMeters(2), null);

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
  public void testModelStability() {
    var motor = DCMotor.getNEO(2);
    var plant = LinearSystemId.createDrivetrainVelocitySystem(
        motor,
        50,
        Units.inchesToMeters(2),
        Units.inchesToMeters(12),
        2.0,
        5.0);

    var kinematics = new DifferentialDriveKinematics(Units.inchesToMeters(24));
    var sim = new DifferentialDrivetrainSim(plant, motor, 5, kinematics.trackWidthMeters, Units.inchesToMeters(2), VecBuilder.fill(0, 0, 0.0001, 0.1, 0.1, 0.005, 0.005));

    sim.setInputs(2, 4);

    // 10 seconds should be enough time to verify stability
    for (int i = 0; i < 500; i++) {
      sim.update(0.020);
    }

    assertTrue(Math.abs(sim.getPose().getTranslation().getNorm()) < 100);
  }
}
