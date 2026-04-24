// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.List;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Pose2d;
import org.wpilib.math.geometry.Rotation2d;
import org.wpilib.math.linalg.MatBuilder;
import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.linalg.VecBuilder;
import org.wpilib.math.random.Normal;
import org.wpilib.math.system.LinearSystem;
import org.wpilib.math.trajectory.TrajectoryConfig;
import org.wpilib.math.trajectory.TrajectoryGenerator;
import org.wpilib.math.util.Nat;

class KalmanFilterTest {
  @Test
  void testSwerveStationary() {
    final var dt = 0.02;

    // Swerve drive with x = [x, y, θ, v_x, v_y, ω]ᵀ, u = [a_x, a_y, α]ᵀ,
    // y = [x, y, θ]ᵀ
    var plant =
        new LinearSystem<>(
            MatBuilder.fill(
                Nat.N6(),
                Nat.N6(),
                // spotless:off
                0, 0, 0, 1, 0, 0,
                0, 0, 0, 0, 1, 0,
                0, 0, 0, 0, 0, 1,
                0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0),
            // spotless:on
            MatBuilder.fill(
                Nat.N6(),
                Nat.N3(),
                // spotless:off
                0, 0, 0,
                0, 0, 0,
                0, 0, 0,
                1, 0, 0,
                0, 1, 0,
                0, 0, 1),
            // spotless:on
            MatBuilder.fill(
                Nat.N3(),
                Nat.N6(),
                // spotless:off
                1, 0, 0, 0, 0, 0,
                0, 1, 0, 0, 0, 0,
                0, 0, 1, 0, 0, 0),
            // spotless:on
            new Matrix<>(Nat.N3(), Nat.N3()));

    var filter =
        new KalmanFilter<>(
            Nat.N6(),
            Nat.N3(),
            plant,
            VecBuilder.fill(0.1, 0.1, 0.1, 0.1, 0.1, 0.1),
            VecBuilder.fill(1.0, 1.0, 1.0),
            dt);

    var u = VecBuilder.fill(0.0, 0.0, 0.0);

    for (int i = 0; i < 100; ++i) {
      var y = Normal.normal(VecBuilder.fill(1.0, 1.0, 1.0));

      filter.correct(u, y);
      filter.predict(u, dt);
    }

    assertEquals(0.0, filter.getXhat(0), 0.3);
    assertEquals(0.0, filter.getXhat(1), 0.3);
  }

  @Test
  void testSwerveBadInitialPose() {
    final var dt = 0.02;

    // Swerve drive with x = [x, y, θ, v_x, v_y, ω]ᵀ, u = [a_x, a_y, α]ᵀ,
    // y = [x, y, θ]ᵀ
    var plant =
        new LinearSystem<>(
            MatBuilder.fill(
                Nat.N6(),
                Nat.N6(),
                // spotless:off
                0, 0, 0, 1, 0, 0,
                0, 0, 0, 0, 1, 0,
                0, 0, 0, 0, 0, 1,
                0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0),
            // spotless:on
            MatBuilder.fill(
                Nat.N6(),
                Nat.N3(),
                // spotless:off
                0, 0, 0,
                0, 0, 0,
                0, 0, 0,
                1, 0, 0,
                0, 1, 0,
                0, 0, 1),
            // spotless:on
            MatBuilder.fill(
                Nat.N3(),
                Nat.N6(),
                // spotless:off
                1, 0, 0, 0, 0, 0,
                0, 1, 0, 0, 0, 0,
                0, 0, 1, 0, 0, 0),
            // spotless:on
            new Matrix<>(Nat.N3(), Nat.N3()));

    var filter =
        new KalmanFilter<>(
            Nat.N6(),
            Nat.N3(),
            plant,
            VecBuilder.fill(0.1, 0.1, 0.1, 0.1, 0.1, 0.1),
            VecBuilder.fill(0.1, 0.1, 0.25),
            dt);

    // Set nonzero position
    filter.setXhat(0, 0.5);
    filter.setXhat(1, 0.5);

    var u = VecBuilder.fill(0.0, 0.0, 0.0);

    // Let filter converge to zero position
    for (int i = 0; i < 300; ++i) {
      var y = Normal.normal(VecBuilder.fill(0.1, 0.1, 0.25));

      filter.correct(u, y);
      filter.predict(u, dt);
    }

    assertEquals(0.0, filter.getXhat(0), 0.2);
    assertEquals(0.0, filter.getXhat(1), 0.2);
  }

  @Test
  void testSwerveMovingOverTrajectory() {
    final var dt = 0.02;

    // Swerve drive with x = [x, y, θ, v_x, v_y, ω]ᵀ, u = [a_x, a_y, α]ᵀ,
    // y = [x, y, θ]ᵀ
    var plant =
        new LinearSystem<>(
            MatBuilder.fill(
                Nat.N6(),
                Nat.N6(),
                // spotless:off
                0, 0, 0, 1, 0, 0,
                0, 0, 0, 0, 1, 0,
                0, 0, 0, 0, 0, 1,
                0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0),
            // spotless:on
            MatBuilder.fill(
                Nat.N6(),
                Nat.N3(),
                // spotless:off
                0, 0, 0,
                0, 0, 0,
                0, 0, 0,
                1, 0, 0,
                0, 1, 0,
                0, 0, 1),
            // spotless:on
            MatBuilder.fill(
                Nat.N3(),
                Nat.N6(),
                // spotless:off
                1, 0, 0, 0, 0, 0,
                0, 1, 0, 0, 0, 0,
                0, 0, 1, 0, 0, 0),
            // spotless:on
            new Matrix<>(Nat.N3(), Nat.N3()));

    var filter =
        new KalmanFilter<>(
            Nat.N6(),
            Nat.N3(),
            plant,
            VecBuilder.fill(0.1, 0.1, 0.1, 0.1, 0.1, 0.1),
            VecBuilder.fill(0.2, 0.2, 1.0 / 3.0),
            dt);

    var trajectory =
        TrajectoryGenerator.generateTrajectory(
            List.of(new Pose2d(0.0, 0.0, Rotation2d.kZero), new Pose2d(5.0, 5.0, Rotation2d.kZero)),
            new TrajectoryConfig(2.0, 2.0));

    var lastVelocity = VecBuilder.fill(0.0, 0.0, 0.0);

    for (var t = 0.0; t < trajectory.getTotalTime(); t += dt) {
      var sample = trajectory.sample(t);

      var y =
          VecBuilder.fill(
              sample.pose.getTranslation().getX(),
              sample.pose.getTranslation().getY(),
              sample.pose.getRotation().getRadians());
      var noise = Normal.normal(VecBuilder.fill(0.2, 0.2, 1.0 / 3.0));
      y.set(0, 0, y.get(0, 0) + noise.get(0, 0));
      y.set(1, 0, y.get(1, 0) + noise.get(1, 0));
      y.set(2, 0, y.get(2, 0) + noise.get(2, 0));

      var velocity =
          VecBuilder.fill(
              sample.velocity * sample.pose.getRotation().getCos(),
              sample.velocity * sample.pose.getRotation().getSin(),
              sample.curvature * sample.velocity);
      var u = velocity.minus(lastVelocity).div(dt);

      filter.correct(u, y);
      filter.predict(u, dt);

      lastVelocity = velocity;
    }

    assertEquals(
        trajectory.sample(trajectory.getTotalTime()).pose.getTranslation().getX(),
        filter.getXhat(0),
        0.2);
    assertEquals(
        trajectory.sample(trajectory.getTotalTime()).pose.getTranslation().getY(),
        filter.getXhat(1),
        0.2);
  }
}
