// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.estimator;

import org.wpilib.math.linalg.Matrix;
import org.wpilib.math.Nat;
import org.wpilib.math.util.Num;
import org.wpilib.math.numbers.N1;
import org.ejml.simple.SimpleMatrix;

/**
 * Generates sigma points and weights according to Papakonstantinou's paper[1] for the
 * UnscentedKalmanFilter class.
 *
 * <p>It parameterizes the sigma points using alpha and beta terms. Unless you know better, this
 * should be your default choice due to its high accuracy and performance.
 *
 * <p>[1] K. Papakonstantinou "A Scaled Spherical Simplex Filter (S3F) with a decreased n + 2 sigma
 * points set size and equivalent 2n + 1 Unscented Kalman Filter (UKF) accuracy"
 *
 * @param <States> The dimenstionality of the state. States + 2 weights will be generated.
 */
public class S3SigmaPoints<States extends Num> implements SigmaPoints<States> {
  private final Nat<States> m_states;
  private final double m_alpha;
  private Matrix<?, N1> m_wm;
  private Matrix<?, N1> m_wc;

  /**
   * Constructs a generator for Papakonstantinou sigma points.
   *
   * @param states an instance of Num that represents the number of states.
   * @param alpha Determines the spread of the sigma points around the mean. Usually a small
   *     positive value (1e-3).
   * @param beta Incorporates prior knowledge of the distribution of the mean. For Gaussian
   *     distributions, beta = 2 is optimal.
   */
  @SuppressWarnings("this-escape")
  public S3SigmaPoints(Nat<States> states, double alpha, double beta) {
    m_states = states;
    m_alpha = alpha;

    computeWeights(beta);
  }

  /**
   * Constructs a generator for Papakonstantinou sigma points with default values for alpha, beta,
   * and kappa.
   *
   * @param states an instance of Num that represents the number of states.
   */
  public S3SigmaPoints(Nat<States> states) {
    this(states, 1e-3, 2);
  }

  /**
   * Returns number of sigma points for each variable in the state x.
   *
   * @return The number of sigma points for each variable in the state x.
   */
  @Override
  public int getNumSigmas() {
    return m_states.getNum() + 2;
  }

  /**
   * Computes the sigma points for an unscented Kalman filter given the mean (x) and square-root
   * covariance (s) of the filter.
   *
   * @param x An array of the means.
   * @param s Square-root covariance of the filter.
   * @return Two-dimensional array of sigma points. Each column contains all the sigmas for one
   *     dimension in the problem space. Ordered by Xi_0, Xi_{1..n}, Xi_{n+1..2n}.
   */
  @Override
  public Matrix<States, ?> squareRootSigmaPoints(Matrix<States, N1> x, Matrix<States, States> s) {
    // table (1), equation (12)
    double[] q = new double[m_states.getNum()];
    for (int t = 1; t <= m_states.getNum(); ++t) {
      q[t - 1] = m_alpha * Math.sqrt(t * (m_states.getNum() + 1) / (double) (t + 1));
    }

    Matrix<States, ?> C = new Matrix<>(new SimpleMatrix(m_states.getNum(), getNumSigmas()));
    for (int row = 0; row < m_states.getNum(); ++row) {
      C.set(row, 0, 0.0);
    }
    for (int col = 1; col < getNumSigmas(); ++col) {
      for (int row = 0; row < m_states.getNum(); ++row) {
        if (row < col - 2) {
          C.set(row, col, 0.0);
        } else if (row == col - 2) {
          C.set(row, col, q[row]);
        } else {
          C.set(row, col, -q[row] / (row + 1));
        }
      }
    }

    Matrix<States, ?> sigmas = new Matrix<>(new SimpleMatrix(m_states.getNum(), getNumSigmas()));
    for (int col = 0; col < getNumSigmas(); ++col) {
      sigmas.setColumn(col, x.plus(s.times(C.extractColumnVector(col))));
    }

    return sigmas;
  }

  /**
   * Computes the weights for the scaled unscented Kalman filter.
   *
   * @param beta Incorporates prior knowledge of the distribution of the mean.
   */
  private void computeWeights(double beta) {
    double alpha_sq = m_alpha * m_alpha;

    double c = 1.0 / (alpha_sq * (m_states.getNum() + 1));

    Matrix<?, N1> wM = new Matrix<>(new SimpleMatrix(getNumSigmas(), 1));
    Matrix<?, N1> wC = new Matrix<>(new SimpleMatrix(getNumSigmas(), 1));
    wM.fill(c);
    wC.fill(c);

    wM.set(0, 0, 1.0 - 1.0 / alpha_sq);
    wC.set(0, 0, 1.0 - 1.0 / alpha_sq + (1 - alpha_sq + beta));

    this.m_wm = wM;
    this.m_wc = wC;
  }

  /**
   * Returns the weight for each sigma point for the mean.
   *
   * @return the weight for each sigma point for the mean.
   */
  @Override
  public Matrix<?, N1> getWm() {
    return m_wm;
  }

  /**
   * Returns an element of the weight for each sigma point for the mean.
   *
   * @param element Element of vector to return.
   * @return the element i's weight for the mean.
   */
  @Override
  public double getWm(int element) {
    return m_wm.get(element, 0);
  }

  /**
   * Returns the weight for each sigma point for the covariance.
   *
   * @return the weight for each sigma point for the covariance.
   */
  @Override
  public Matrix<?, N1> getWc() {
    return m_wc;
  }

  /**
   * Returns an element of the weight for each sigma point for the covariance.
   *
   * @param element Element of vector to return.
   * @return The element I's weight for the covariance.
   */
  @Override
  public double getWc(int element) {
    return m_wc.get(element, 0);
  }
}
