/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.math;

import java.util.ArrayList;
import java.util.List;

import org.ejml.dense.row.MatrixFeatures_DDRM;
import org.ejml.simple.SimpleMatrix;
import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.geometry.Pose2d;
import edu.wpi.first.wpilibj.geometry.Rotation2d;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.SimpleMatrixUtils;
import edu.wpi.first.wpiutil.math.VecBuilder;
import edu.wpi.first.wpiutil.math.numbers.N1;
import edu.wpi.first.wpiutil.math.numbers.N2;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class StateSpaceUtilTest {
  @Test
  public void testCostArray() {
    var mat = StateSpaceUtil.makeCostMatrix(
          VecBuilder.fill(1.0, 2.0, 3.0));

    assertEquals(1.0, mat.get(0, 0), 1e-3);
    assertEquals(0.0, mat.get(0, 1), 1e-3);
    assertEquals(0.0, mat.get(0, 2), 1e-3);
    assertEquals(0.0, mat.get(1, 0), 1e-3);
    assertEquals(1.0 / 4.0, mat.get(1, 1), 1e-3);
    assertEquals(0.0, mat.get(1, 2), 1e-3);
    assertEquals(0.0, mat.get(0, 2), 1e-3);
    assertEquals(0.0, mat.get(1, 2), 1e-3);
    assertEquals(1.0 / 9.0, mat.get(2, 2), 1e-3);
  }

  @Test
  public void testCovArray() {
    var mat = StateSpaceUtil.makeCovarianceMatrix(Nat.N3(),
          VecBuilder.fill(1.0, 2.0, 3.0));

    assertEquals(1.0, mat.get(0, 0), 1e-3);
    assertEquals(0.0, mat.get(0, 1), 1e-3);
    assertEquals(0.0, mat.get(0, 2), 1e-3);
    assertEquals(0.0, mat.get(1, 0), 1e-3);
    assertEquals(4.0, mat.get(1, 1), 1e-3);
    assertEquals(0.0, mat.get(1, 2), 1e-3);
    assertEquals(0.0, mat.get(0, 2), 1e-3);
    assertEquals(0.0, mat.get(1, 2), 1e-3);
    assertEquals(9.0, mat.get(2, 2), 1e-3);
  }

  @Test
  @SuppressWarnings("LocalVariableName")
  public void testIsStabilizable() {
    Matrix<N2, N2> A;
    Matrix<N2, N1> B = VecBuilder.fill(0, 1);

    // First eigenvalue is uncontrollable and unstable.
    // Second eigenvalue is controllable and stable.
    A = Matrix.mat(Nat.N2(), Nat.N2()).fill(1.2, 0, 0, 0.5);
    assertFalse(StateSpaceUtil.isStabilizable(A, B));

    // First eigenvalue is uncontrollable and marginally stable.
    // Second eigenvalue is controllable and stable.
    A = Matrix.mat(Nat.N2(), Nat.N2()).fill(1, 0, 0, 0.5);
    assertFalse(StateSpaceUtil.isStabilizable(A, B));

    // First eigenvalue is uncontrollable and stable.
    // Second eigenvalue is controllable and stable.
    A = Matrix.mat(Nat.N2(), Nat.N2()).fill(0.2, 0, 0, 0.5);
    assertTrue(StateSpaceUtil.isStabilizable(A, B));

    // First eigenvalue is uncontrollable and stable.
    // Second eigenvalue is controllable and unstable.
    A = Matrix.mat(Nat.N2(), Nat.N2()).fill(0.2, 0, 0, 1.2);
    assertTrue(StateSpaceUtil.isStabilizable(A, B));
  }

  @Test
  public void testMakeWhiteNoiseVector() {
    var firstData = new ArrayList<Double>();
    var secondData = new ArrayList<Double>();
    for (int i = 0; i < 1000; i++) {
      var noiseVec = StateSpaceUtil.makeWhiteNoiseVector(VecBuilder.fill(1.0, 2.0));
      firstData.add(noiseVec.get(0, 0));
      secondData.add(noiseVec.get(1, 0));
    }
    assertEquals(1.0, calculateStandardDeviation(firstData), 0.2);
    assertEquals(2.0, calculateStandardDeviation(secondData), 0.2);
  }

  private double calculateStandardDeviation(List<Double> numArray) {
    double sum = 0.0;
    double standardDeviation = 0.0;
    int length = numArray.size();

    for (double num : numArray) {
      sum += num;
    }

    double mean = sum / length;

    for (double num : numArray) {
      standardDeviation += Math.pow(num - mean, 2);
    }

    return Math.sqrt(standardDeviation / length);
  }

  @Test
  public void testDiscretizeA() {
    var contA = Matrix.mat(Nat.N2(), Nat.N2()).fill(0, 1, 0, 0);
    var x0 = VecBuilder.fill(1, 1);
    var discA = Discretization.discretizeA(contA, 1.0);
    var x1Discrete = discA.times(x0);

    // We now have pos = vel = 1 and accel = 0, which should give us:
    var x1Truth = VecBuilder.fill(x0.get(0, 0) + 1.0 * x0.get(1, 0),
          x0.get(1, 0));
    assertTrue(x1Truth.isEqual(x1Discrete, 1E-4));
  }

  @SuppressWarnings("LocalVariableName")
  @Test
  public void testDiscretizeAB() {
    var contA = Matrix.mat(Nat.N2(), Nat.N2()).fill(0, 1, 0, 0);
    var contB = VecBuilder.fill(0, 1);
    var x0 = VecBuilder.fill(1, 1);
    var u = VecBuilder.fill(1);

    var abPair = Discretization.discretizeAB(contA, contB, 1.0);

    var x1Discrete = abPair.getFirst().times(x0).plus(abPair.getSecond().times(u));

    // We now have pos = vel = accel = 1, which should give us:
    var x1Truth = VecBuilder.fill(x0.get(0, 0) + x0.get(1, 0) + 0.5 * u.get(0, 0), x0.get(0, 0)
          + u.get(0, 0));

    assertTrue(x1Truth.isEqual(x1Discrete, 1E-4));
  }

  @Test
  public void testMatrixExp() {
    Matrix<N2, N2> wrappedMatrix = Matrix.eye(Nat.N2());
    var wrappedResult = wrappedMatrix.exp();

    assertTrue(wrappedResult.isEqual(
        Matrix.mat(Nat.N2(), Nat.N2()).fill(Math.E, 0, 0, Math.E), 1E-9));

    var matrix = Matrix.mat(Nat.N2(), Nat.N2()).fill(1, 2, 3, 4);
    wrappedResult = matrix.times(0.01).exp();

    assertTrue(wrappedResult.isEqual(Matrix.mat(Nat.N2(), Nat.N2()).fill(1.01035625, 0.02050912,
              0.03076368, 1.04111993), 1E-8));
  }

  @Test
  public void testSimpleMatrixExp() {
    SimpleMatrix matrix = SimpleMatrixUtils.eye(2);
    var result = SimpleMatrixUtils.exp(matrix);

    assertTrue(MatrixFeatures_DDRM.isIdentical(
          result.getDDRM(),
          new SimpleMatrix(2, 2, true, new double[]{Math.E, 0, 0, Math.E}).getDDRM(),
          1E-9
    ));

    matrix = new SimpleMatrix(2, 2, true, new double[]{1, 2, 3, 4});
    result = SimpleMatrixUtils.exp(matrix.scale(0.01));

    assertTrue(MatrixFeatures_DDRM.isIdentical(
          result.getDDRM(),
          new SimpleMatrix(2, 2, true, new double[]{1.01035625, 0.02050912,
              0.03076368, 1.04111993}).getDDRM(),
          1E-8
    ));
  }

  @Test
  public void testPoseToVector() {
    Pose2d pose = new Pose2d(1, 2, new Rotation2d(3));
    var vector = StateSpaceUtil.poseToVector(pose);
    assertEquals(pose.getTranslation().getX(), vector.get(0, 0), 1e-6);
    assertEquals(pose.getTranslation().getY(), vector.get(1, 0), 1e-6);
    assertEquals(pose.getRotation().getRadians(), vector.get(2, 0), 1e-6);
  }

}
