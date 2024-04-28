// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.controller;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.VecBuilder;
import edu.wpi.first.math.estimator.KalmanFilter;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.math.system.LinearSystem;
import edu.wpi.first.math.system.LinearSystemLoop;
import edu.wpi.first.math.system.plant.DCMotor;
import edu.wpi.first.math.system.plant.LinearSystemId;
import edu.wpi.first.math.trajectory.TrapezoidProfile;
import java.util.Random;
import org.junit.jupiter.api.Test;

class LinearSystemLoopTest {
  private static final double kDt = 0.00505;
  private static final double kPositionStddev = 0.0001;
  private static final Random random = new Random();

  LinearSystem<N2, N1, N2> m_plant =
      LinearSystemId.createElevatorSystem(DCMotor.getVex775Pro(2), 5, 0.0181864, 1.0);

  @SuppressWarnings("unchecked")
  KalmanFilter<N2, N1, N1> m_observer =
      new KalmanFilter<>(
          Nat.N2(),
          Nat.N1(),
          (LinearSystem<N2, N1, N1>) m_plant.slice(0),
          VecBuilder.fill(0.05, 1.0),
          VecBuilder.fill(0.0001),
          kDt);

  @SuppressWarnings("unchecked")
  LinearQuadraticRegulator<N2, N1, N1> m_controller =
      new LinearQuadraticRegulator<>(
          (LinearSystem<N2, N1, N1>) m_plant.slice(0),
          VecBuilder.fill(0.02, 0.4),
          VecBuilder.fill(12.0),
          0.00505);

  @SuppressWarnings("unchecked")
  private final LinearSystemLoop<N2, N1, N1> m_loop =
      new LinearSystemLoop<>(
          (LinearSystem<N2, N1, N1>) m_plant.slice(0), m_controller, m_observer, 12, 0.00505);

  private static void updateTwoState(
      LinearSystem<N2, N1, N1> plant, LinearSystemLoop<N2, N1, N1> loop, double noise) {
    Matrix<N1, N1> y = plant.calculateY(loop.getXHat(), loop.getU()).plus(VecBuilder.fill(noise));

    loop.correct(y);
    loop.predict(kDt);
  }

  @Test
  @SuppressWarnings("unchecked")
  void testStateSpaceEnabled() {
    m_loop.reset(VecBuilder.fill(0, 0));
    Matrix<N2, N1> references = VecBuilder.fill(2.0, 0.0);
    var constraints = new TrapezoidProfile.Constraints(4, 3);
    m_loop.setNextR(references);

    TrapezoidProfile profile;
    TrapezoidProfile.State state;
    for (int i = 0; i < 1000; i++) {
      profile = new TrapezoidProfile(constraints);
      state =
          profile.calculate(
              kDt,
              new TrapezoidProfile.State(m_loop.getXHat(0), m_loop.getXHat(1)),
              new TrapezoidProfile.State(references.get(0, 0), references.get(1, 0)));
      m_loop.setNextR(VecBuilder.fill(state.position, state.velocity));

      updateTwoState(
          (LinearSystem<N2, N1, N1>) m_plant.slice(0),
          m_loop,
          (random.nextGaussian()) * kPositionStddev);
      var u = m_loop.getU(0);

      assertTrue(u >= -12.1 && u <= 12.1, "U out of bounds! Got " + u);
    }

    assertEquals(2.0, m_loop.getXHat(0), 0.05);
    assertEquals(0.0, m_loop.getXHat(1), 0.5);
  }

  @Test
  void testFlywheelEnabled() {
    LinearSystem<N1, N1, N1> plant =
        LinearSystemId.createFlywheelSystem(DCMotor.getNEO(2), 0.00289, 1.0);
    KalmanFilter<N1, N1, N1> observer =
        new KalmanFilter<>(
            Nat.N1(), Nat.N1(), plant, VecBuilder.fill(1.0), VecBuilder.fill(kPositionStddev), kDt);

    var qElms = VecBuilder.fill(9.0);
    var rElms = VecBuilder.fill(12.0);

    var controller = new LinearQuadraticRegulator<>(plant, qElms, rElms, kDt);

    var feedforward = new LinearPlantInversionFeedforward<>(plant, kDt);

    var loop = new LinearSystemLoop<>(controller, feedforward, observer, 12);

    loop.reset(VecBuilder.fill(0.0));
    var references = VecBuilder.fill(3000 / 60d * 2 * Math.PI);

    loop.setNextR(references);

    var time = 0.0;
    while (time < 10) {
      if (time > 10) {
        break;
      }

      loop.setNextR(references);

      Matrix<N1, N1> y =
          plant
              .calculateY(loop.getXHat(), loop.getU())
              .plus(VecBuilder.fill(random.nextGaussian() * kPositionStddev));

      loop.correct(y);
      loop.predict(kDt);
      var u = loop.getU(0);

      assertTrue(u >= -12.1 && u <= 12.1, "U out of bounds! Got " + u);

      time += kDt;
    }

    assertEquals(0.0, loop.getError(0), 0.1);
  }
}
