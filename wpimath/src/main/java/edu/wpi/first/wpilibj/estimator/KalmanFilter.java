/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.estimator;

import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.math.Drake;
import edu.wpi.first.math.MathSharedStore;
import edu.wpi.first.wpilibj.math.Discretization;
import edu.wpi.first.wpilibj.math.StateSpaceUtil;
import edu.wpi.first.wpilibj.system.LinearSystem;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

/**
 * A Kalman filter combines predictions from a model and measurements to give an estimate of the
 * true system state. This is useful because many states cannot be measured directly as a result of
 * sensor noise, or because the state is "hidden".
 *
 * <p>Kalman filters use a K gain matrix to determine whether to trust the model or measurements
 * more. Kalman filter theory uses statistics to compute an optimal K gain which minimizes the sum
 * of squares error in the state estimate. This K gain is used to correct the state estimate by
 * some amount of the difference between the actual measurements and the measurements predicted by
 * the model.
 *
 * <p>For more on the underlying math, read
 * https://file.tavsys.net/control/controls-engineering-in-frc.pdf chapter 9.
 */
public class KalmanFilter<S extends Num, I extends Num,
      O extends Num> implements KalmanTypeFilter<S, I, O> {

  private final Nat<S> m_states;

  private final LinearSystem<S, I, O> m_plant;

  private final Matrix<S, N1> m_stateStdDevs;
  private final Matrix<O, N1> m_measurementStdDevs;

  /**
   * Error covariance matrix.
   */
  @SuppressWarnings("MemberName")
  private Matrix<S, S> m_P;

  /**
   * Continuous process noise covariance matrix.
   */
  private final Matrix<S, S> m_contQ;

  /**
   * Continuous measurement noise covariance matrix.
   */
  private final Matrix<O, O> m_contR;

  /**
   * Discrete measurement noise covariance matrix.
   */
  private Matrix<O, O> m_discR;

  /**
   * The current state estimate x-hat.
   */
  @SuppressWarnings("MemberName")
  private Matrix<S, N1> m_xHat;

  /**
   * Constructs a state-space observer with the given plant.
   *
   * @param states             A Nat representing the states of the system.
   * @param outputs            A Nat representing the outputs of the system.
   * @param plant              The plant used for the prediction step.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dtSeconds          Nominal discretization timestep.
   */
  public KalmanFilter(
        Nat<S> states, Nat<O> outputs,
        LinearSystem<S, I, O> plant,
        Matrix<S, N1> stateStdDevs,
        Matrix<O, N1> measurementStdDevs,
        double dtSeconds
  ) {
    this.m_states = states;

    this.m_plant = plant;

    this.m_contQ = StateSpaceUtil.makeCovarianceMatrix(states, stateStdDevs);
    this.m_contR = StateSpaceUtil.makeCovarianceMatrix(outputs, measurementStdDevs);

    this.m_stateStdDevs = stateStdDevs;
    this.m_measurementStdDevs = measurementStdDevs;

    var pair = Discretization.discretizeAQTaylor(plant.getA(), m_contQ, dtSeconds);
    var discA = pair.getFirst();
    var discQ = pair.getSecond();

    m_discR = Discretization.discretizeR(m_contR, dtSeconds);

    // IsStabilizable(A^T, C^T) will tell us if the system is observable.
    var isObservable = StateSpaceUtil.isStabilizable(discA.transpose(), plant.getC().transpose());
    if (isObservable) {
      if (outputs.getNum() <= states.getNum()) {
        m_P = new Matrix<>(Drake.discreteAlgebraicRiccatiEquation(
            discA.transpose(), plant.getC().transpose(), discQ, m_discR));
      } else {
        m_P = new Matrix<>(new SimpleMatrix(states.getNum(), states.getNum()));
      }
    } else {
      MathSharedStore.reportError("The system passed to the Kalman Filter is not observable!",
          Thread.currentThread().getStackTrace());
      throw new IllegalArgumentException(
        "The system passed to the Kalman Filter is not observable!");
    }

    reset();
  }

  @Override
  public void reset() {
    m_xHat = new Matrix<>(m_states, Nat.N1());
  }

  /**
   * Returns the error covariance matrix P.
   *
   * @return the error covariance matrix P.
   */
  @Override
  public Matrix<S, S> getP() {
    return m_P;
  }

  /**
   * Returns an element of the error covariance matrix P.
   *
   * @param row Row of P.
   * @param col Column of P.
   * @return the element (i, j) of the error covariance matrix P.
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
  public void setP(Matrix<S, S> newP) {
    m_P = newP;
  }

  /**
   * Set initial state estimate x-hat.
   *
   * @param xhat The state estimate x-hat.
   */
  @Override
  public void setXhat(Matrix<S, N1> xhat) {
    this.m_xHat = xhat;
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
   * Returns the state estimate x-hat.
   *
   * @return The state estimate x-hat.
   */
  @Override
  public Matrix<S, N1> getXhat() {
    return m_xHat;
  }

  /**
   * Returns an element of the state estimate x-hat.
   *
   * @param row Row of x-hat.
   * @return the state estimate x-hat at i.
   */
  @Override
  public double getXhat(int row) {
    return m_xHat.get(row, 0);
  }

  /**
   * Returns the state standard deviations used to make Q.
   */
  public Matrix<S, N1> getStateStdDevs() {
    return m_stateStdDevs;
  }

  /**
   * Returns the measurement standard deviations used to make R.
   */
  public Matrix<O, N1> getMeasurementStdDevs() {
    return m_measurementStdDevs;
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u         New control input from controller.
   * @param dtSeconds Timestep for prediction.
   */
  @SuppressWarnings("ParameterName")
  @Override
  public void predict(Matrix<I, N1> u, double dtSeconds) {
    this.m_xHat = m_plant.calculateX(m_xHat, u, dtSeconds);

    var pair = Discretization.discretizeAQTaylor(m_plant.getA(), m_contQ, dtSeconds);
    var discA = pair.getFirst();
    var discQ = pair.getSecond();

    m_P = discA.times(m_P).times(discA.transpose()).plus(discQ);
    m_discR = Discretization.discretizeR(m_contR, dtSeconds);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the last predict step.
   * @param y Measurement vector.
   */
  @SuppressWarnings("ParameterName")
  @Override
  public void correct(Matrix<I, N1> u, Matrix<O, N1> y) {
    correct(u, y, m_plant.getC(), m_plant.getD(), m_discR);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * <p>This is useful for when the measurements available during a timestep's
   * Correct() call vary. The C matrix passed to the constructor is used if one
   * is not provided (the two-argument version of this function).
   *
   * @param <R> Number of rows in the result of f(x, u).
   * @param u   Same control input used in the predict step.
   * @param y   Measurement vector.
   * @param C   Output matrix.
   * @param r   Discrete measurement noise covariance matrix.
   */
  @SuppressWarnings({"ParameterName", "LocalVariableName"})
  public <R extends Num> void correct(
        Matrix<I, N1> u,
        Matrix<R, N1> y,
        Matrix<R, S> C,
        Matrix<R, I> D,
        Matrix<R, R> r) {
    var S = C.times(m_P).times(C.transpose()).plus(r);

    // We want to put K = PC^T S^-1 into Ax = b form so we can solve it more
    // efficiently.
    //
    // K = PC^T S^-1
    // KS = PC^T
    // (KS)^T = (PC^T)^T
    // S^T K^T = CP^T
    //
    // The solution of Ax = b can be found via x = A.solve(b).
    //
    // K^T = S^T.solve(CP^T)
    // K = (S^T.solve(CP^T))^T
    Matrix<S, R> K = S.transpose().solve(C.times(m_P.transpose())).transpose();

    m_xHat = m_xHat.plus(K.times(y.minus(C.times(m_xHat).plus(D.times(u)))));
    m_P = Matrix.eye(m_states).minus(K.times(C)).times(m_P);
  }

}
