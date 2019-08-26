/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.control;

import java.util.Random;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.control.periodvariant.ElevatorCoeffs;
import edu.wpi.first.wpilibj.controller.PeriodVariantLoop;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class PeriodVariantLoopTest {
  private static final double kPositionStddev = 0.0001;
  private static final double kNominalDt = 0.005;
  private final PeriodVariantLoop<N2, N1, N1> m_loop = ElevatorCoeffs.makeElevatorLoop();
  private final Random m_rand = new Random();

  private void setReferences(double position, double velocity) {
    Matrix<N2, N1> nextR = MatrixUtils.vec(Nat.N2()).fill(position, velocity);
    m_loop.setNextR(nextR);
  }

  @SuppressWarnings("LocalVariableName")
  private void update() {
    var dt = Math.abs(kNominalDt + (m_rand.nextGaussian() * 0.02));

    var y = m_loop.getPlant().updateY(m_loop.getXhat(), m_loop.getU());
    if (m_loop.isEnabled()) {
      y = y.plus(m_rand.nextGaussian() * (kPositionStddev / kNominalDt) * dt);
    }
    m_loop.correct(y);

    m_loop.predict(dt);
  }

  @BeforeEach
  void resetLoop() {
    m_loop.disable();
    m_loop.reset();
  }

  @Test
  void testDisabled() {
    setReferences(2.0, 0.0);

    for (int i = 0; i < 100; i++) {
      update();
    }

    double position = m_loop.getXhat(0);
    double velocity = m_loop.getXhat(1);
    // The system shouldn't move as it was never enabled
    assertEquals(position, 0.0, 1E-9);
    assertEquals(velocity, 0.0, 1E-9);
  }

  @Test
  void testEnabled() {
    setReferences(2.0, 0.0);

    m_loop.enable();

    for (int i = 0; i < 350; i++) {
      update();
      assertTrue(m_loop.getU(0) >= -12.0 && m_loop.getU(0) <= 12.0,
          "Controller voltage not clamped to -12..12");
    }

    assertEquals(m_loop.getXhat(0), 2.0, 0.05);
    assertEquals(m_loop.getXhat(1), 0.0, 0.5);
  }
}
