package edu.wpi.first.wpilibj.control;

import edu.wpi.first.wpilibj.controller.StateSpaceLoop;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.Random;

public class StateSpaceLoopTest {
  private static final double kPositionStddev = 0.0001;
  private final StateSpaceLoop<N2, N1, N1> m_loop = ElevatorCoeffs.makeElevatorLoop();
  private final Random rand = new Random();

  private void setReferences(double position, double velocity) {
    Matrix<N2, N1> nextR = MatrixUtils.vec(Nat.N2()).fill(position, velocity);
    m_loop.setNextR(nextR);
  }

  private void update() {
    var y = m_loop.getPlant().updateY(m_loop.getXhat(), m_loop.getU()).plus(rand.nextGaussian() * kPositionStddev);
    m_loop.correct(y);
    m_loop.predict();
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

    var position = m_loop.getXhat(0);
    var velocity = m_loop.getXhat(1);
    // The system shouldn't move as it was never enabled
    Assertions.assertEquals(position, 0.0, 1E-9);
    Assertions.assertEquals(velocity, 0.0, 1E-9);
  }

  @Test
  void testEnabled() {
    setReferences(2.0, 0.0);

    m_loop.enable();

    for (int i = 0; i < 350; i++) {
      update();
      Assertions.assertTrue(m_loop.getU(0) >= -12.0 && m_loop.getU(0) <= 12.0, "Controller voltage not clamped to 0..12: " + m_loop.getU(0));
    }

    Assertions.assertEquals(m_loop.getXhat(0), 2.0, 0.05);
    Assertions.assertEquals(m_loop.getXhat(1), 0.0, 0.5);
  }
}
