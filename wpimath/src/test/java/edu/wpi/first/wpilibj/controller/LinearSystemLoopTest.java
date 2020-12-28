// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.controller;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.estimator.KalmanFilter;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpilibj.system.LinearSystemLoop;
import edu.wpi.first.wpilibj.system.plant.DCMotor;
import edu.wpi.first.wpilibj.system.plant.LinearSystemId;
import edu.wpi.first.wpilibj.trajectory.TrapezoidProfile;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class LinearSystemLoopTest {
  public static final double kDt = 0.00505;
  private static final double kPositionStddev = 0.0001;
  private static final Random random = new Random();

  LinearSystem<N2, N1, N1> m_plant = LinearSystemId.createElevatorSystem(DCMotor.getVex775Pro(2), 5,
      0.0181864, 1.0);

  KalmanFilter<N2, N1, N1> m_observer = new KalmanFilter<>(Nat.N2(), Nat.N1(), m_plant,
      VecBuilder.fill(0.05, 1.0),
      VecBuilder.fill(0.0001), kDt);

  LinearQuadraticRegulator<N2, N1, N1> m_controller = new LinearQuadraticRegulator<>(
      m_plant, VecBuilder.fill(0.02, 0.4), VecBuilder.fill(12.0),
        0.00505);

  private final LinearSystemLoop<N2, N1, N1> m_loop =
      new LinearSystemLoop<>(m_plant, m_controller, m_observer, 12, 0.00505);

  @SuppressWarnings("LocalVariableName")
  private static void updateTwoState(LinearSystem<N2, N1, N1> plant, LinearSystemLoop<N2, N1, N1>
      loop, double noise) {
    Matrix<N1, N1> y = plant.calculateY(loop.getXHat(), loop.getU()).plus(
          VecBuilder.fill(noise)
    );

    loop.correct(y);
    loop.predict(kDt);
  }

  @Test
  @SuppressWarnings({"LocalVariableName", "PMD.AvoidInstantiatingObjectsInLoops"})
  public void testStateSpaceEnabled() {

    m_loop.reset(VecBuilder.fill(0, 0));
    Matrix<N2, N1> references = VecBuilder.fill(2.0, 0.0);
    var constraints = new TrapezoidProfile.Constraints(4, 3);
    m_loop.setNextR(references);

    TrapezoidProfile profile;
    TrapezoidProfile.State state;
    for (int i = 0; i < 1000; i++) {
      profile = new TrapezoidProfile(
            constraints, new TrapezoidProfile.State(m_loop.getXHat(0), m_loop.getXHat(1)),
            new TrapezoidProfile.State(references.get(0, 0), references.get(1, 0))
      );
      state = profile.calculate(kDt);
      m_loop.setNextR(VecBuilder.fill(state.position, state.velocity));

      updateTwoState(m_plant, m_loop, (random.nextGaussian()) * kPositionStddev);
      var u = m_loop.getU(0);

      assertTrue(u >= -12.1 && u <= 12.1, "U out of bounds! Got " + u);
    }

    assertEquals(2.0, m_loop.getXHat(0), 0.05);
    assertEquals(0.0, m_loop.getXHat(1), 0.5);

  }

  @Test
  @SuppressWarnings({"LocalVariableName", "PMD.AvoidInstantiatingObjectsInLoops"})
  public void testFlywheelEnabled() {

    LinearSystem<N1, N1, N1> plant = LinearSystemId.createFlywheelSystem(DCMotor.getNEO(2),
          0.00289, 1.0);
    KalmanFilter<N1, N1, N1> observer = new KalmanFilter<>(Nat.N1(), Nat.N1(), plant,
          VecBuilder.fill(1.0),
          VecBuilder.fill(kPositionStddev), kDt);

    var qElms = VecBuilder.fill(9.0);
    var rElms = VecBuilder.fill(12.0);

    var controller = new LinearQuadraticRegulator<>(
          plant, qElms, rElms, kDt);

    var feedforward = new LinearPlantInversionFeedforward<>(plant, kDt);

    var loop = new LinearSystemLoop<>(controller, feedforward, observer, 12);

    loop.reset(VecBuilder.fill(0.0));
    var references = VecBuilder.fill(3000 / 60d * 2 * Math.PI);

    loop.setNextR(references);

    List<Double> timeData = new ArrayList<>();
    List<Double> xHat = new ArrayList<>();
    List<Double> volt = new ArrayList<>();
    List<Double> reference = new ArrayList<>();
    List<Double> error = new ArrayList<>();

    var time = 0.0;
    while (time < 10) {

      if (time > 10) {
        break;
      }

      loop.setNextR(references);

      Matrix<N1, N1> y = plant.calculateY(loop.getXHat(), loop.getU()).plus(
            VecBuilder.fill(random.nextGaussian() * kPositionStddev)
      );

      loop.correct(y);
      loop.predict(kDt);
      var u = loop.getU(0);

      assertTrue(u >= -12.1 && u <= 12.1, "U out of bounds! Got " + u);

      xHat.add(loop.getXHat(0) / 2d / Math.PI * 60);
      volt.add(u);
      timeData.add(time);
      reference.add(references.get(0, 0) / 2d / Math.PI * 60);
      error.add(loop.getError(0) / 2d / Math.PI * 60);
      time += kDt;
    }

    //    XYChart bigChart = new XYChartBuilder().build();
    //    bigChart.addSeries("Xhat, RPM", timeData, xHat);
    //    bigChart.addSeries("Reference, RPM", timeData, reference);
    //    bigChart.addSeries("Error, RPM", timeData, error);

    //    XYChart smolChart = new XYChartBuilder().build();
    //    smolChart.addSeries("Volts, V", timeData, volt);

    //  try {
    //      new SwingWrapper<>(List.of(bigChart, smolChart)).displayChartMatrix();
    //      Thread.sleep(10000000);
    //    } catch (InterruptedException e) { e.printStackTrace(); }

    assertEquals(0.0, loop.getError(0), 0.1);
  }

}
