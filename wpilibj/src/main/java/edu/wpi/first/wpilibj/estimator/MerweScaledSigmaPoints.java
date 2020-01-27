/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.estimator;

import edu.wpi.first.wpiutil.math.*;
import edu.wpi.first.wpiutil.math.numbers.N1;
import org.ejml.data.DMatrixRMaj;
import org.ejml.dense.row.factory.DecompositionFactory_DDRM;
import org.ejml.interfaces.decomposition.CholeskyDecomposition_F64;
import org.ejml.simple.SimpleMatrix;

/**
 * Generates sigma points and weights according to Van der Merwe's 2004
 * dissertation[1] for the UnscentedKalmanFilter class.
 *
 * It parametizes the sigma points using alpha, beta, kappa terms, and is the
 * version seen in most publications. Unless you know better, this should be
 * your default choice.
 *
 * States is the dimensionality of the state. 2*States+1 weights will be
 *                generated.
 *
 * [1] R. Van der Merwe "Sigma-Point Kalman Filters for Probabilitic
 *     Inference in Dynamic State-Space Models" (Doctoral dissertation)
 */
public class MerweScaledSigmaPoints<States extends Num> {

    private final double m_alpha;
    private final int m_kappa;
    private final States m_states;
    private Matrix m_Wm;
    private Matrix m_Wc;

    /**
     * Constructs a generator for Van der Merwe scaled sigma points.
     *
     * @param states an instance of Num that represents the number of states.
     * @param alpha Determines the spread of the sigma points around the mean.
     *              Usually a small positive value (1e-3).
     * @param beta Incorporates prior knowledge of the distribution of the mean.
     *             For Gaussian distributions, beta = 2 is optimal.
     * @param kappa Secondary scaling parameter usually set to 0 or 3 - States.
     */
    public MerweScaledSigmaPoints(States states, double alpha, double beta, int kappa) {
        this.m_states = states;
        this.m_alpha = alpha;
        this.m_kappa = kappa;

        computeWeights(beta);
    }

    public MerweScaledSigmaPoints(States states) {
        this(states, 1e-3, 2, 3 - states.getNum());
    }

    /**
     * Returns number of sigma points for each variable in the state x.
     */
    public int getNumSigns() {
        return 2 * m_states.getNum() + 1;
    }

    /**
     * Computes the sigma points for an unscented Kalman filter given the mean
     * (x) and covariance(P) of the filter.
     *
     * @param x An array of the means.
     * @param P Covariance of the filter.
     *
     * @return Two dimensional array of sigma points. Each column contains all of
     *         the sigmas for one dimension in the problem space. Ordered by
     *         Xi_0, Xi_{1..n}, Xi_{n+1..2n}.
     *
     */
    public Matrix sigmaPoints(
            Vector<States> x,
            Matrix<States, States> P) {

        double lambda = Math.pow(m_alpha, 2) * (m_states.getNum() + m_kappa) - m_states.getNum();
        int states = m_states.getNum();

        var intermediate = P.times(lambda + m_states.getNum()).getStorage();//.llt().matrixL();
        var U = SimpleMatrixUtils.lltDecompose(intermediate);

        // 2 * states + 1 by states
        SimpleMatrix sigmas = new SimpleMatrix(2 * m_states.getNum() + 1, m_states.getNum());
        for(int i = 0; i < states; i++) {
            sigmas.set(0, i, x.get(i, 0));
        }
        for (int k = 0; k < m_states.getNum(); ++k) {
            for(int i = 0; i < states; i++) {
                sigmas.set(k + 1 + i, 0, x.get(i, 0) + U.get(k + i, 0));
                sigmas.set(states + k + 1 + i, 0, x.get(i, 0) - U.get(k + i, 0));
            }
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
        var wC = new SimpleMatrix(1, 2 * m_states.getNum() + 1);
        var wM = new SimpleMatrix(1, 2 * m_states.getNum() + 1);
        for (int i = 0; i < 2 * m_states.getNum() + 1; ++i) {
            m_Wc.set(0, i,
                    lambda / (m_states.getNum() + lambda)
                            + (1 - Math.pow(m_alpha, 2) + beta));
            m_Wm.set(0, i, lambda / (m_states.getNum() + lambda));
        }
        this.m_Wc = new Matrix(wC);
        this.m_Wm = new Matrix(wM);
    }

    /**
     * Returns the weight for each sigma point for the mean.
     */
    public Matrix getWm() { return m_Wm; }

    /**
     * Returns an element of the weight for each sigma point for the mean.
     *
     * @param i Element of vector to return.
     */
    public double getWm(int i) { return m_Wm.get(0, i); }

    /**
     * Returns the weight for each sigma point for the covariance.
     */
    public Matrix getWc() { return m_Wc; }

    /**
     * Returns an element of the weight for each sigma point for the covariance.
     *
     * @param i Element of vector to return.
     */
    public double getWc(int i) { return m_Wc.get(0, i); }

}
