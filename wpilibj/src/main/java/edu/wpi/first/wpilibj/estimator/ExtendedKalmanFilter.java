package edu.wpi.first.wpilibj.estimator;

import java.util.function.BiFunction;

import edu.wpi.first.wpilibj.math.StateSpaceUtils;
import edu.wpi.first.wpilibj.system.NumericalJacobian;
import edu.wpi.first.wpilibj.system.RungeKutta;
import edu.wpi.first.wpiutil.math.Drake;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.SimpleMatrixUtils;
import edu.wpi.first.wpiutil.math.numbers.N1;

public class ExtendedKalmanFilter<S extends Num, I extends Num, O extends Num>
        implements KalmanTypeFilter<S, I, O> {
  private final Nat<S> m_states;
  private final Nat<O> m_outputs;

  @SuppressWarnings("MemberName")
  private final BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<S, N1>> m_f;
  @SuppressWarnings("MemberName")
  private final BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<O, N1>> m_h;
  private final Matrix<S, S> m_contQ;
  private final Matrix<O, O> m_discR;
  private final Matrix<S, S> m_initP;
  @SuppressWarnings("MemberName")
  private Matrix<S, N1> m_xHat;
  @SuppressWarnings("MemberName")
  private Matrix<S, S> m_P;

  /**
   * Constructs an extended Kalman filter.
   *
   * @param states             a Nat representing the number of states.
   * @param inputs             a Nat representing the number of inputs.
   * @param outputs            a Nat representing the number of outputs.
   * @param f                  A vector-valued function of x and u that returns
   *                           the derivative of the state vector.
   * @param h                  A vector-valued function of x and u that returns
   *                           the measurement vector.
   * @param stateStdDevs       Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param dtSeconds          Nominal discretization timestep.
   */
  @SuppressWarnings("ParameterName")
  public ExtendedKalmanFilter(
          Nat<S> states,
          Nat<I> inputs,
          Nat<O> outputs,
          BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<S, N1>> f,
          BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<O, N1>> h,
          Matrix<S, N1> stateStdDevs,
          Matrix<O, N1> measurementStdDevs,
          double dtSeconds
  ) {
    m_states = states;
    m_outputs = outputs;

    m_f = f;
    m_h = h;

    reset();

    m_contQ = StateSpaceUtils.makeCovMatrix(states, stateStdDevs);
    var contR = StateSpaceUtils.makeCovMatrix(outputs, measurementStdDevs);

    final var contA = NumericalJacobian
            .numericalJacobianX(states, states, f, m_xHat, MatrixUtils.zeros(inputs));
    final var C = NumericalJacobian
            .numericalJacobianX(outputs, states, h, m_xHat, MatrixUtils.zeros(inputs));

    final var discPair = StateSpaceUtils.discretizeAQTaylor(contA, m_contQ, dtSeconds);
    final var discA = discPair.getFirst();
    final var discQ = discPair.getSecond();

    m_discR = StateSpaceUtils.discretizeR(contR, dtSeconds);

    if (StateSpaceUtils.isStabilizable(
            discA.transpose(), C.transpose()) && outputs.getNum() <= states.getNum()) {
      m_initP = new Matrix<>(Drake.discreteAlgebraicRiccatiEquation(
              discA.transpose(), C.transpose(), discQ, m_discR));
    } else {
      m_initP = MatrixUtils.zeros(states, states);
    }

    m_P = m_initP;
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
  public void setP(Matrix<S, S> newP) {
    m_P = newP;
  }

  /**
   * Returns the state estimate x-hat.
   *
   * @return the state estimate x-hat.
   */
  @Override
  public Matrix<S, N1> getXhat() {
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
  public void setXhat(Matrix<S, N1> xHat) {
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

  @Override
  public void reset() {
    m_xHat = MatrixUtils.zeros(m_states);
    m_P = m_initP;
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
    predict(u, m_f, dtSeconds);
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u         New control input from controller.
   * @param f         The function used to linearlize the model.
   * @param dtSeconds Timestep for prediction.
   */
  @SuppressWarnings("ParameterName")
  public void predict(
          Matrix<I, N1> u, BiFunction<Matrix<S, N1>,
          Matrix<I, N1>, Matrix<S, N1>> f,
          double dtSeconds
  ) {
    // Find continuous A
    final var contA = NumericalJacobian.numericalJacobianX(m_states, m_states, f, m_xHat, u);

    // Find discrete A and Q
    final var discPair = StateSpaceUtils.discretizeAQTaylor(contA, m_contQ, dtSeconds);
    final var discA = discPair.getFirst();
    final var discQ = discPair.getSecond();

    m_xHat = RungeKutta.rungeKutta(f, m_xHat, u, dtSeconds);

    m_P = discA.times(m_P).times(discA.transpose()).plus(discQ);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  @SuppressWarnings("ParameterName")
  @Override
  public void correct(Matrix<I, N1> u, Matrix<O, N1> y) {
    correct(m_outputs, u, y, m_h, m_discR);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * <p>This is useful for when the measurements available during a timestep's
   * Correct() call vary. The h(x, u) passed to the constructor is used if one is
   * not provided (the two-argument version of this function).
   *
   * @param <R>  Number of rows in the result of f(x, u).
   * @param rows Number of rows in the result of f(x, u).
   * @param u    Same control input used in the predict step.
   * @param y    Measurement vector.
   * @param h    A vector-valued function of x and u that returns the measurement
   *             vector.
   * @param R    Discrete measurement noise covariance matrix.
   */
  @SuppressWarnings("ParameterName")
  public <R extends Num> void correct(
          Nat<R> rows, Matrix<I, N1> u,
          Matrix<R, N1> y,
          BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<R, N1>> h,
          Matrix<R, R> R
  ) {
    final var C = NumericalJacobian.numericalJacobianX(rows, m_states, h, m_xHat, u);

    final var S = C.times(m_P).times(C.transpose()).plus(R);

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
    //
    // Now we have the optimal Kalman gain
    final var K = new Matrix<S, R>(SimpleMatrixUtils.lltDecompose(S.transpose().getStorage())
            .solve(C.times(m_P.transpose()).getStorage()).transpose());

    m_xHat = m_xHat.plus(K.times(y.minus(h.apply(m_xHat, u))));
    m_P = MatrixUtils.eye(m_states).minus(K.times(C)).times(m_P);
  }
}
