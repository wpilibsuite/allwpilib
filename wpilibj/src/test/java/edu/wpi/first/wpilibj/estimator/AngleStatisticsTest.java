package edu.wpi.first.wpilibj.estimator;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.VecBuilder;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class AngleStatisticsTest {
  @Test
  public void testMean() {
    // 3 states, 2 sigmas
    var sigmas = Matrix.mat(Nat.N3(), Nat.N2()).fill(
            1, 1.2,
            Math.toRadians(359), Math.toRadians(3),
            1, 2
    );
    // Weights can just be 1
    var Wc = VecBuilder.fill(1.0/sigmas.getNumCols(), 1.0/sigmas.getNumCols());

    assertTrue(AngleStatistics.angleMean(sigmas, Wc, 1).isEqual(VecBuilder.fill(1.1, Math.toRadians(1), 1.5), 1e-6));
  }

  @Test
  public void testResidual() {
    var a = VecBuilder.fill(1, Math.toRadians(1), 2);
    var b = VecBuilder.fill(1, Math.toRadians(359), 1);

//    System.out.println(AngleStatistics.angleResidual(a, b, 1));
    assertTrue(AngleStatistics.angleResidual(a, b, 1).isEqual(VecBuilder.fill(0, Math.toRadians(2), 1), 1e-6));
  }
}
