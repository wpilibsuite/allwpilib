package edu.wpi.first.wpilibj.estimator;

import java.util.function.BiFunction;

import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.wpilibj.system.RungeKutta;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.SimpleMatrixUtils;
import edu.wpi.first.wpilibj.math.StateSpaceUtils;
import edu.wpi.first.wpiutil.math.numbers.N1;


@SuppressWarnings("MemberName")
public class UnscentedKalmanFilter<S extends Num, I extends Num,
        O extends Num> implements KalmanTypeFilter<S, I, O> {

  private final BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<S, N1>> m_f;
  private final BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<O, N1>> m_h;
  private final Matrix<S, S> m_Q;
  private final Matrix<O, O> m_R;
  /**
   * The states of the system.
   */
  private final Nat<S> m_states;
  /**
   * The outputs of the system.
   */
  private final Nat<O> m_outputs;
  private Matrix<S, N1> m_xHat;
  private Matrix<S, S> m_P;
  private Matrix<?, S> m_sigmasF;
  private MerweScaledSigmaPoints<S> m_pts;

  /**
   * Constructs an Unscented Kalman Filter.
   *
   * @param states             TODO
   * @param inputs             TODO
   * @param outputs            TODO
   * @param f                  TODO
   * @param h                  TODO
   * @param stateStdDevs       TODO
   * @param measurementStdDevs TODO
   */
  @SuppressWarnings("ParameterName")
  public UnscentedKalmanFilter(Nat<S> states, Nat<I> inputs, Nat<O> outputs,
                               BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<S, N1>> f,
                               BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<O, N1>> h,
                               Matrix<S, N1> stateStdDevs,
                               Matrix<O, N1> measurementStdDevs,
                               double nominalDt) {
    this.m_states = states;
    this.m_outputs = outputs;

    m_pts = new MerweScaledSigmaPoints<>(states);

    m_f = f;
    m_h = h;
    m_Q = StateSpaceUtils.makeCovMatrix(states, stateStdDevs);
    m_R = StateSpaceUtils.makeCovMatrix(outputs, measurementStdDevs);

    reset();
  }

  @SuppressWarnings({"ParameterName", "LocalVariableName"})
  private static <S extends Num, C extends Num>
  SimpleMatrixUtils.Pair<Matrix<C, N1>, Matrix<C, C>> unscentedTransform(
          Nat<S> s, Nat<C> dim, Matrix sigmas, Matrix Wm, Matrix Wc, Matrix<C, C> noiseCov
  ) {
    if (sigmas.getNumRows() != 2 * s.getNum() + 1 || sigmas.getNumCols() != dim.getNum()) {
      throw new IllegalArgumentException("Sigmas must be 2 * states + 1 by covDim! Got "
              + sigmas.getNumRows() + " by " + sigmas.getNumCols());
    }

    if (Wm.getNumRows() != 1 || Wm.getNumCols() != 2 * s.getNum() + 1) {
      throw new IllegalArgumentException("Wm must be 1 by 2 * states + 1! Got "
              + Wm.getNumRows() + " by " + Wm.getNumCols());
    }

    if (Wc.getNumRows() != 1 || Wc.getNumCols() != 2 * s.getNum() + 1) {
      throw new IllegalArgumentException("Wc must be 1 by 2 * states + 1! Got "
              + Wc.getNumRows() + " by " + Wc.getNumCols());
    }

    if (noiseCov.getNumRows() != dim.getNum() || noiseCov.getNumCols() != dim.getNum()) {
      throw new IllegalArgumentException("noiseCov must be covDim by covDim! Got "
              + noiseCov.getNumRows() + " by " + noiseCov.getNumCols());
    }

    // New mean is just the sum of the sigmas * weight
    // dot = \Sigma^n_1 (W[k]*Xi[k])
    Matrix<N1, C> x = Wm.times(sigmas);

    // New covariance is the sum of the outer product of the residuals times the
    // weights
    Matrix<?, C> y = new Matrix<>(new SimpleMatrix(2 * s.getNum() + 1, dim.getNum()));
    for (int i = 0; i < 2 * s.getNum() + 1; i++) {
      Matrix<N1, C> rowBlock = y.extractRowVector(i);
      rowBlock = rowBlock.minus(x);
      for (int j = 0; j < dim.getNum(); j++) {
        y.set(i, j, rowBlock.get(0, j));
      }
    }
    Matrix<C, C> P = y.transpose().times(Wc.diag()).times(y);

    P = P.plus(noiseCov);

    return new SimpleMatrixUtils.Pair<>(x.transpose(), P);
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

  /**
   * Resets the observer.
   */
  @Override
  public void reset() {
    m_xHat = new Matrix<>(new SimpleMatrix(m_states.getNum(), 1));
    m_P = new Matrix<>(new SimpleMatrix(m_states.getNum(), m_states.getNum()));
    m_sigmasF = new Matrix<>(new SimpleMatrix(2 * m_states.getNum() + 1, m_states.getNum()));
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u         New control input from controller.
   * @param dtSeconds Timestep for prediction.
   */
  @SuppressWarnings({"LocalVariableName", "ParameterName"})
  public void predict(Matrix<I, N1> u, double dtSeconds) {
    var sigmas = m_pts.sigmaPoints(m_xHat, m_P);

    for (int i = 0; i < m_pts.getNumSigmas(); ++i) {
      Matrix<S, N1> x =
              m_sigmasF.extractRowVector(i).transpose();

      var deltaXT = RungeKutta.rungeKutta(m_f, x, u, dtSeconds).transpose();
      for (int j = 0; j < m_states.getNum(); j++) {
        m_sigmasF.set(i, j, deltaXT.get(0, j));
      }

      var ret = unscentedTransform(m_states, m_states,
              m_sigmasF, m_pts.getWm(), m_pts.getWc(), m_Q);

      m_xHat = ret.getFirst();
      m_P = ret.getSecond();
    }

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
    correct(m_outputs, u, y, m_h, m_R);
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
          Nat<R> rows, Matrix<I, N1> u,
          Matrix<R, N1> y,
          BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<R, N1>> h,
          Matrix<R, R> R) {
    // Transform sigma points into measurement space
    Matrix<?, R> sigmasH = new Matrix<>(new SimpleMatrix(2 * m_states.getNum() + 1, rows.getNum()));
    for (int i = 0; i < m_pts.getNumSigmas(); i++) {
      Matrix<R, N1> hRet = h.apply(
              m_sigmasF.extractRowVector(i).transpose(),
              u
      );
      for (int j = 0; j < rows.getNum(); j++) {
        sigmasH.set(i, j, hRet.get(j, 0));
      }
    }

    // Mean and covariance of prediction passed through unscented transform
    var transRet = unscentedTransform(m_states, rows, sigmasH, m_pts.getWm(), m_pts.getWc(), R);
    var yHat = transRet.getFirst();
    var Py = transRet.getSecond();

    // Compute cross covariance of the state and the measurements
    Matrix<S, R> Pxy = MatrixUtils.zeros(m_states, rows);
    for (int i = 0; i < m_pts.getNumSigmas(); i++) {
      var temp =
              m_sigmasF.extractRowVector(i).minus(m_xHat.transpose())
                      .transpose()
                      .times(
                              sigmasH.extractRowVector(i).minus(yHat.transpose())
                      );

      Pxy = Pxy.plus(temp.times(m_pts.getWc(i)));
    }

    // K = P_{xy} Py^-1
    // K^T = P_y^T^-1 P_{xy}^T
    // P_y^T K^T = P_{xy}^T
    // K^T = P_y^T.solve(P_{xy}^T)
    // K = (P_y^T.solve(P_{xy}^T)^T
    Matrix<S, R> K = new Matrix<>(
            SimpleMatrixUtils.lltDecompose(Py.transpose().getStorage())
                    .solve(Pxy.transpose().getStorage())
                    .transpose()
    );

    m_xHat = m_xHat.plus(K.times(y.minus(yHat)));
    m_P = m_P.minus(K.times(Py).times(K.transpose()));
  }


}
