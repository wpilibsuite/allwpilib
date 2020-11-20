/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.estimator;

import java.util.function.BiFunction;

import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.wpilibj.math.Discretization;
import edu.wpi.first.wpilibj.math.StateSpaceUtil;
import edu.wpi.first.wpilibj.system.NumericalJacobian;
import edu.wpi.first.wpilibj.system.RungeKutta;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.Pair;
import edu.wpi.first.wpiutil.math.numbers.N1;

/**
 * A Kalman filter combines predictions from a model and measurements to give an estimate of the
 * true ystem state. This is useful because many states cannot be measured directly as a result of
 * sensor noise, or because the state is "hidden".
 *
 * <p>The Unscented Kalman filter is similar to the {@link KalmanFilter Kalman filter}, except that
 * it propagates carefully chosen points called sigma points through the non-linear model to obtain
 * an estimate of the true covariance (as opposed to a linearized version of it). This means that
 * the UKF works with nonlinear systems.
 */
@SuppressWarnings({"MemberName", "ClassTypeParameterName", "PMD.TooManyFields"})
public class UnscentedKalmanFilter<States extends Num, Inputs extends Num,
      Outputs extends Num> implements KalmanTypeFilter<States, Inputs, Outputs> {

  private final Nat<States> m_states;
  private final Nat<Outputs> m_outputs;

  private final BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> m_f;
  private final BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> m_h;

  private BiFunction<Matrix<States, ?>, Matrix<?, N1>, Matrix<States, N1>> m_meanFuncX;
  private BiFunction<Matrix<Outputs, ?>, Matrix<?, N1>, Matrix<Outputs, N1>> m_meanFuncY;
  private BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> m_residualFuncX;
  private BiFunction<Matrix<Outputs, N1>, Matrix<Outputs, N1>, Matrix<Outputs, N1>> m_residualFuncY;
  private BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> m_addFuncX;

  private Matrix<States, N1> m_xHat;
  private Matrix<States, States> m_P;
  private final Matrix<States, States> m_contQ;
  private final Matrix<Outputs, Outputs> m_contR;
  private Matrix<States, ?> m_sigmasF;
  private double m_dtSeconds;

  private final MerweScaledSigmaPoints<States> m_pts;

  /**
   * Constructs an Unscented Kalman Filter.
   *
   * @param states             A Nat representing the number of states.
   * @param outputs            A Nat representing the number of outputs.
   * @param f                  A vector-valued function of x and u that returns
   *                           the derivative of the state vector.
   * @param h                  A vector-valued function of x and u that returns
   *                           the measurement vector.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param nominalDtSeconds   Nominal discretization timestep.
   */
  @SuppressWarnings("ParameterName")
  public UnscentedKalmanFilter(Nat<States> states, Nat<Outputs> outputs,
                               BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>,
                                   Matrix<States, N1>> f,
                               BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>,
                                   Matrix<Outputs, N1>> h,
                               Matrix<States, N1> stateStdDevs,
                               Matrix<Outputs, N1> measurementStdDevs,
                               double nominalDtSeconds) {
    this(
        states, outputs,
        f, h,
        stateStdDevs, measurementStdDevs,
        (sigmas, Wm) -> sigmas.times(Matrix.changeBoundsUnchecked(Wm)),
        (sigmas, Wm) -> sigmas.times(Matrix.changeBoundsUnchecked(Wm)),
        Matrix::minus,
        Matrix::minus,
        Matrix::plus,
        nominalDtSeconds
    );
  }

  /**
   * Constructs an unscented Kalman filter with custom mean, residual, and addition functions. Using
   * custom functions for arithmetic can be useful if you have angles in the state or measurements,
   * because they allow you to correctly account for the modular nature of angle arithmetic.
   *
   * @param states             A Nat representing the number of states.
   * @param outputs            A Nat representing the number of outputs.
   * @param f                  A vector-valued function of x and u that returns
   *                           the derivative of the state vector.
   * @param h                  A vector-valued function of x and u that returns
   *                           the measurement vector.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param meanFuncX          A function that computes the mean of 2 * States + 1 state vectors
   *                           using a given set of weights.
   * @param meanFuncY          A function that computes the mean of 2 * States + 1 measurement
   *                           vectors using a given set of weights.
   * @param residualFuncX      A function that computes the residual of two state vectors (i.e. it
   *                           subtracts them.)
   * @param residualFuncY      A function that computes the residual of two measurement vectors
   *                           (i.e. it subtracts them.)
   * @param addFuncX           A function that adds two state vectors.
   * @param nominalDtSeconds   Nominal discretization timestep.
   */
  @SuppressWarnings({"ParameterName", "PMD.ExcessiveParameterList"})
  public UnscentedKalmanFilter(
          Nat<States> states, Nat<Outputs> outputs,
          BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
          BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> h,
          Matrix<States, N1> stateStdDevs,
          Matrix<Outputs, N1> measurementStdDevs,
          BiFunction<Matrix<States, ?>, Matrix<?, N1>, Matrix<States, N1>> meanFuncX,
          BiFunction<Matrix<Outputs, ?>, Matrix<?, N1>, Matrix<Outputs, N1>> meanFuncY,
          BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> residualFuncX,
          BiFunction<Matrix<Outputs, N1>, Matrix<Outputs, N1>, Matrix<Outputs, N1>> residualFuncY,
          BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> addFuncX,
          double nominalDtSeconds
  ) {
    this.m_states = states;
    this.m_outputs = outputs;

    m_f = f;
    m_h = h;

    m_meanFuncX = meanFuncX;
    m_meanFuncY = meanFuncY;
    m_residualFuncX = residualFuncX;
    m_residualFuncY = residualFuncY;
    m_addFuncX = addFuncX;

    m_dtSeconds = nominalDtSeconds;

    m_contQ = StateSpaceUtil.makeCovarianceMatrix(states, stateStdDevs);
    m_contR = StateSpaceUtil.makeCovarianceMatrix(outputs, measurementStdDevs);

    m_pts = new MerweScaledSigmaPoints<>(states);

    reset();
  }

  @SuppressWarnings({"ParameterName", "LocalVariableName", "PMD.CyclomaticComplexity"})
  static <S extends Num, C extends Num>
       Pair<Matrix<C, N1>, Matrix<C, C>> unscentedTransform(
        Nat<S> s, Nat<C> dim, Matrix<C, ?> sigmas, Matrix<?, N1> Wm, Matrix<?, N1> Wc,
        BiFunction<Matrix<C, ?>, Matrix<?, N1>, Matrix<C, N1>> meanFunc,
        BiFunction<Matrix<C, N1>, Matrix<C, N1>, Matrix<C, N1>> residualFunc
  ) {
    if (sigmas.getNumRows() != dim.getNum() || sigmas.getNumCols() != 2 * s.getNum() + 1) {
      throw new IllegalArgumentException("Sigmas must be covDim by 2 * states + 1! Got "
            + sigmas.getNumRows() + " by " + sigmas.getNumCols());
    }

    if (Wm.getNumRows() != 2 * s.getNum() + 1 || Wm.getNumCols() != 1 ) {
      throw new IllegalArgumentException("Wm must be 2 * states + 1 by 1! Got "
            + Wm.getNumRows() + " by " + Wm.getNumCols());
    }

    if (Wc.getNumRows() != 2 * s.getNum() + 1 || Wc.getNumCols() != 1) {
      throw new IllegalArgumentException("Wc must be 2 * states + 1 by 1! Got "
            + Wc.getNumRows() + " by " + Wc.getNumCols());
    }

    // New mean is usually just the sum of the sigmas * weight:
    // dot = \Sigma^n_1 (W[k]*Xi[k])
    Matrix<C, N1> x = meanFunc.apply(sigmas, Wm);

    // New covariance is the sum of the outer product of the residuals times the
    // weights
    Matrix<C, ?> y = new Matrix<>(new SimpleMatrix(dim.getNum(), 2 * s.getNum() + 1));
    for (int i = 0; i < 2 * s.getNum() + 1; i++) {
      // y[:, i] = sigmas[:, i] - x
      y.setColumn(i, residualFunc.apply(sigmas.extractColumnVector(i), x));
    }
    Matrix<C, C> P = y.times(Matrix.changeBoundsUnchecked(Wc.diag()))
          .times(Matrix.changeBoundsUnchecked(y.transpose()));

    return new Pair<>(x, P);
  }

  /**
   * Returns the error covariance matrix P.
   *
   * @return the error covariance matrix P.
   */
  @Override
  public Matrix<States, States> getP() {
    return m_P;
  }

  /**
   * Returns an element of the error covariance matrix P.
   *
   * @param row Row of P.
   * @param col Column of P.
   * @return the value of the error covariance matrix P at (i, j).
   */
  @Override
  public double getP(int row, int col) {
    return m_P.get(row, col);
  }

  /**
   * Sets the entire error covariance matrix P.
   *
   * @param newP The new value of P to use.
   */
  @Override
  public void setP(Matrix<States, States> newP) {
    m_P = newP;
  }

  /**
   * Returns the state estimate x-hat.
   *
   * @return the state estimate x-hat.
   */
  @Override
  public Matrix<States, N1> getXhat() {
    return m_xHat;
  }

  /**
   * Returns an element of the state estimate x-hat.
   *
   * @param row Row of x-hat.
   * @return the value of the state estimate x-hat at i.
   */
  @Override
  public double getXhat(int row) {
    return m_xHat.get(row, 0);
  }


  /**
   * Set initial state estimate x-hat.
   *
   * @param xHat The state estimate x-hat.
   */
  @SuppressWarnings("ParameterName")
  @Override
  public void setXhat(Matrix<States, N1> xHat) {
    m_xHat = xHat;
  }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param row   Row of x-hat.
   * @param value Value for element of x-hat.
   */
  @Override
  public void setXhat(int row, double value) {
    m_xHat.set(row, 0, value);
  }

  /**
   * Resets the observer.
   */
  @Override
  public void reset() {
    m_xHat = new Matrix<>(m_states, Nat.N1());
    m_P = new Matrix<>(m_states, m_states);
    m_sigmasF = new Matrix<>(new SimpleMatrix(m_states.getNum(), 2 * m_states.getNum() + 1));
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u         New control input from controller.
   * @param dtSeconds Timestep for prediction.
   */
  @SuppressWarnings({"LocalVariableName", "ParameterName"})
  @Override
  public void predict(Matrix<Inputs, N1> u, double dtSeconds) {
    // Discretize Q before projecting mean and covariance forward
    Matrix<States, States> contA =
        NumericalJacobian.numericalJacobianX(m_states, m_states, m_f, m_xHat, u);
    var discQ =
        Discretization.discretizeAQTaylor(contA, m_contQ, dtSeconds).getSecond();

    var sigmas = m_pts.sigmaPoints(m_xHat, m_P);

    for (int i = 0; i < m_pts.getNumSigmas(); ++i) {
      Matrix<States, N1> x = sigmas.extractColumnVector(i);

      m_sigmasF.setColumn(i, RungeKutta.rungeKutta(m_f, x, u, dtSeconds));
    }

    var ret = unscentedTransform(m_states, m_states,
          m_sigmasF, m_pts.getWm(), m_pts.getWc(), m_meanFuncX, m_residualFuncX);

    m_xHat = ret.getFirst();
    m_P = ret.getSecond().plus(discQ);
    m_dtSeconds = dtSeconds;
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  @SuppressWarnings("ParameterName")
  @Override
  public void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y) {
    correct(m_outputs, u, y, m_h, m_contR,
            m_meanFuncY, m_residualFuncY, m_residualFuncX, m_addFuncX);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * <p>This is useful for when the measurements available during a timestep's
   * Correct() call vary. The h(x, u) passed to the constructor is used if one
   * is not provided (the two-argument version of this function).
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param h A vector-valued function of x and u that returns
   *          the measurement vector.
   * @param R Measurement noise covariance matrix.
   */
  @SuppressWarnings({"ParameterName", "LocalVariableName"})
  public <R extends Num> void correct(
        Nat<R> rows, Matrix<Inputs, N1> u,
        Matrix<R, N1> y,
        BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<R, N1>> h,
        Matrix<R, R> R,
        BiFunction<Matrix<R, ?>, Matrix<?, N1>, Matrix<R, N1>> meanFuncY,
        BiFunction<Matrix<R, N1>, Matrix<R, N1>, Matrix<R, N1>> residualFuncY,
        BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> residualFuncX,
        BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> addFuncX) {
    final var discR = Discretization.discretizeR(R, m_dtSeconds);

    // Transform sigma points into measurement space
    Matrix<R, ?> sigmasH = new Matrix<>(new SimpleMatrix(
          rows.getNum(), 2 * m_states.getNum() + 1));
    var sigmas = m_pts.sigmaPoints(m_xHat, m_P);
    for (int i = 0; i < m_pts.getNumSigmas(); i++) {
      Matrix<R, N1> hRet = h.apply(
            sigmas.extractColumnVector(i),
            u
      );
      sigmasH.setColumn(i, hRet);
    }

    // Mean and covariance of prediction passed through unscented transform
    var transRet = unscentedTransform(m_states, rows,
            sigmasH, m_pts.getWm(), m_pts.getWc(), meanFuncY, residualFuncY);
    var yHat = transRet.getFirst();
    var Py = transRet.getSecond().plus(discR);

    // Compute cross covariance of the state and the measurements
    Matrix<States, R> Pxy = new Matrix<>(m_states, rows);
    for (int i = 0; i < m_pts.getNumSigmas(); i++) {
      // Pxy += (sigmas_f[:, i] - xHat) * (sigmas_h[:, i] - yHat)^T * W_c[i]
      var dx = residualFuncX.apply(m_sigmasF.extractColumnVector(i), m_xHat);
      var dy = residualFuncY.apply(sigmasH.extractColumnVector(i), yHat).transpose();

      Pxy = Pxy.plus(dx.times(dy).times(m_pts.getWc(i)));
    }

    // K = P_{xy} Py^-1
    // K^T = P_y^T^-1 P_{xy}^T
    // P_y^T K^T = P_{xy}^T
    // K^T = P_y^T.solve(P_{xy}^T)
    // K = (P_y^T.solve(P_{xy}^T)^T
    Matrix<States, R> K = new Matrix<>(
          Py.transpose().solve(Pxy.transpose()).transpose()
    );

    // xHat + K * (y - yHat)
    m_xHat = addFuncX.apply(m_xHat, K.times(residualFuncY.apply(y, yHat)));
    m_P = m_P.minus(K.times(Py).times(K.transpose()));
  }
}
