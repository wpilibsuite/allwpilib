// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.math.geometry.Pose2d;
import edu.wpi.first.math.geometry.Rotation2d;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.numbers.N2;
import edu.wpi.first.wpilibj.UtilityClassTest;
import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.Test;

class StateSpaceUtilTest extends UtilityClassTest<StateSpaceUtil> {
  StateSpaceUtilTest() {
    super(StateSpaceUtil.class);
  }

  @Test
  void testCostArray() {
    var mat = StateSpaceUtil.makeCostMatrix(VecBuilder.fill(1.0, 2.0, 3.0));

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
  void testCovArray() {
    var mat = StateSpaceUtil.makeCovarianceMatrix(Nat.N3(), VecBuilder.fill(1.0, 2.0, 3.0));

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
  void testIsStabilizable() {
    Matrix<N2, N2> A;
    Matrix<N2, N1> B = VecBuilder.fill(0, 1);

    // First eigenvalue is uncontrollable and unstable.
    // Second eigenvalue is controllable and stable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.2, 0, 0, 0.5);
    assertFalse(StateSpaceUtil.isStabilizable(A, B));

    // First eigenvalue is uncontrollable and marginally stable.
    // Second eigenvalue is controllable and stable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 0.5);
    assertFalse(StateSpaceUtil.isStabilizable(A, B));

    // First eigenvalue is uncontrollable and stable.
    // Second eigenvalue is controllable and stable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.2, 0, 0, 0.5);
    assertTrue(StateSpaceUtil.isStabilizable(A, B));

    // First eigenvalue is uncontrollable and stable.
    // Second eigenvalue is controllable and unstable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.2, 0, 0, 1.2);
    assertTrue(StateSpaceUtil.isStabilizable(A, B));
  }

  @Test
  void testIsDetectable() {
    Matrix<N2, N2> A;
    Matrix<N1, N2> C = MatBuilder.fill(Nat.N1(), Nat.N2(), 0, 1);

    // First eigenvalue is unobservable and unstable.
    // Second eigenvalue is observable and stable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 1.2, 0, 0, 0.5);
    assertFalse(StateSpaceUtil.isDetectable(A, C));

    // First eigenvalue is unobservable and marginally stable.
    // Second eigenvalue is observable and stable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 0, 0, 0.5);
    assertFalse(StateSpaceUtil.isDetectable(A, C));

    // First eigenvalue is unobservable and stable.
    // Second eigenvalue is observable and stable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.2, 0, 0, 0.5);
    assertTrue(StateSpaceUtil.isDetectable(A, C));

    // First eigenvalue is unobservable and stable.
    // Second eigenvalue is observable and unstable.
    A = MatBuilder.fill(Nat.N2(), Nat.N2(), 0.2, 0, 0, 1.2);
    assertTrue(StateSpaceUtil.isDetectable(A, C));
  }

  @Test
  void testMakeWhiteNoiseVector() {
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
  void testMatrixExp() {
    Matrix<N2, N2> wrappedMatrix = Matrix.eye(Nat.N2());
    var wrappedResult = wrappedMatrix.exp();

    assertTrue(
        wrappedResult.isEqual(MatBuilder.fill(Nat.N2(), Nat.N2(), Math.E, 0, 0, Math.E), 1E-9));

    var matrix = MatBuilder.fill(Nat.N2(), Nat.N2(), 1, 2, 3, 4);
    wrappedResult = matrix.times(0.01).exp();

    assertTrue(
        wrappedResult.isEqual(
            MatBuilder.fill(Nat.N2(), Nat.N2(), 1.01035625, 0.02050912, 0.03076368, 1.04111993),
            1E-8));
  }

  @Test
  @SuppressWarnings("removal")
  void testPoseToVector() {
    Pose2d pose = new Pose2d(1, 2, new Rotation2d(3));
    var vector = StateSpaceUtil.poseToVector(pose);
    assertEquals(pose.getTranslation().getX(), vector.get(0, 0), 1e-6);
    assertEquals(pose.getTranslation().getY(), vector.get(1, 0), 1e-6);
    assertEquals(pose.getRotation().getRadians(), vector.get(2, 0), 1e-6);
  }
}
