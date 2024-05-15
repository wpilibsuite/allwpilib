// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.numbers.N1;
import org.ejml.simple.SimpleMatrix;

/**
 * Generates sigma points and weights according to Van der Merwe's 2004 dissertation[1] for the
 * UnscentedKalmanFilter class.
 *
 * <p>It parametrizes the sigma points using alpha, beta, kappa terms, and is the version seen in
 * most publications. Unless you know better, this should be your default choice.
 *
 * <p>States is the dimensionality of the state. 2*States+1 weights will be generated.
 *
 * <p>[1] R. Van der Merwe "Sigma-Point Kalman Filters for Probabilistic Inference in Dynamic
 * State-Space Models" (Doctoral dissertation)
 *
 * @param <S> The dimensionality of the state. 2 * States + 1 weights will be generated.
 */
public class MerweScaledSigmaPoints<S extends Num> {
  private final double m_alpha;
  private final int m_kappa;
  private final Nat<S> m_states;
  private Matrix<?, N1> m_wm;
  private Matrix<?, N1> m_wc;

  /**
   * Constructs a generator for Van der Merwe scaled sigma points.
   *
   * @param states an instance of Num that represents the number of states.
   * @param alpha Determines the spread of the sigma points around the mean. Usually a small
   *     positive value (1e-3).
   * @param beta Incorporates prior knowledge of the distribution of the mean. For Gaussian
   *     distributions, beta = 2 is optimal.
   * @param kappa Secondary scaling parameter usually set to 0 or 3 - States.
   */
  public MerweScaledSigmaPoints(Nat<S> states, double alpha, double beta, int kappa) {
    this.m_states = states;
    this.m_alpha = alpha;
    this.m_kappa = kappa;

    computeWeights(beta);
  }

  /**
   * Constructs a generator for Van der Merwe scaled sigma points with default values for alpha,
   * beta, and kappa.
   *
   * @param states an instance of Num that represents the number of states.
   */
  public MerweScaledSigmaPoints(Nat<S> states) {
    this(states, 1e-3, 2, 3 - states.getNum());
  }

  /**
   * Returns number of sigma points for each variable in the state x.
   *
   * @return The number of sigma points for each variable in the state x.
   */
  public int getNumSigmas() {
    return 2 * m_states.getNum() + 1;
  }

  /**
   * Computes the sigma points for an unscented Kalman filter given the mean (x) and covariance(P)
   * of the filter.
   *
   * @param x An array of the means.
   * @param s Square-root covariance of the filter.
   * @return Two-dimensional array of sigma points. Each column contains all the sigmas for one
   *     dimension in the problem space. Ordered by Xi_0, Xi_{1..n}, Xi_{n+1..2n}.
   */
  public Matrix<S, ?> squareRootSigmaPoints(Matrix<S, N1> x, Matrix<S, S> s) {
    double lambda = Math.pow(m_alpha, 2) * (m_states.getNum() + m_kappa) - m_states.getNum();
    double eta = Math.sqrt(lambda + m_states.getNum());

    Matrix<S, S> U = s.times(eta);

    // 2 * states + 1 by states
    Matrix<S, ?> sigmas =
        new Matrix<>(new SimpleMatrix(m_states.getNum(), 2 * m_states.getNum() + 1));
    sigmas.setColumn(0, x);
    for (int k = 0; k < m_states.getNum(); k++) {
      var xPlusU = x.plus(U.extractColumnVector(k));
      var xMinusU = x.minus(U.extractColumnVector(k));
      sigmas.setColumn(k + 1, xPlusU);
      sigmas.setColumn(m_states.getNum() + k + 1, xMinusU);
    }

    return new Matrix<>(sigmas);
  }

  /**
   * Computes the weights for the scaled unscented Kalman filter.
   *
   * @param beta Incorporates prior knowledge of the distribution of the mean.
   */
  private void computeWeights(double beta) {
    double lambda = Math.pow(m_alpha, 2) * (m_states.getNum() + m_kappa) - m_states.getNum();
    double c = 0.5 / (m_states.getNum() + lambda);

    Matrix<?, N1> wM = new Matrix<>(new SimpleMatrix(2 * m_states.getNum() + 1, 1));
    Matrix<?, N1> wC = new Matrix<>(new SimpleMatrix(2 * m_states.getNum() + 1, 1));
    wM.fill(c);
    wC.fill(c);

    wM.set(0, 0, lambda / (m_states.getNum() + lambda));
    wC.set(0, 0, lambda / (m_states.getNum() + lambda) + (1 - Math.pow(m_alpha, 2) + beta));

    this.m_wm = wM;
    this.m_wc = wC;
  }

  /**
   * Returns the weight for each sigma point for the mean.
   *
   * @return the weight for each sigma point for the mean.
   */
  public Matrix<?, N1> getWm() {
    return m_wm;
  }

  /**
   * Returns an element of the weight for each sigma point for the mean.
   *
   * @param element Element of vector to return.
   * @return the element i's weight for the mean.
   */
  public double getWm(int element) {
    return m_wm.get(element, 0);
  }

  /**
   * Returns the weight for each sigma point for the covariance.
   *
   * @return the weight for each sigma point for the covariance.
   */
  public Matrix<?, N1> getWc() {
    return m_wc;
  }

  /**
   * Returns an element of the weight for each sigma point for the covariance.
   *
   * @param element Element of vector to return.
   * @return The element I's weight for the covariance.
   */
  public double getWc(int element) {
    return m_wc.get(element, 0);
  }
}
