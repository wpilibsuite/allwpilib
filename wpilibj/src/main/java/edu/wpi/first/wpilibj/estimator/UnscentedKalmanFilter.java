package edu.wpi.first.wpilibj.estimator;

import java.util.function.BiFunction;

import org.ejml.dense.row.CommonOps_DDRM;
import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.wpilibj.math.StateSpaceUtils;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.SimpleMatrixUtils;
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
  private final Nat<S> states;
  /**
   * The inputs of the system.
   */
  private final Nat<I> m_inputs;
  /**
   * The outputs of the system.
   */
  private final Nat<O> m_outputs;
  private Matrix<S, N1> m_xHat;
  private Matrix<S, S> m_P;
  private Matrix m_sigmasF;
  // TODO where is this initialized?
  private MerweScaledSigmaPoints<S> m_pts;

  /**
   * Constructs an Unscented Kalman Filter.
   *
   * @param states TODO
   * @param inputs TODO
   * @param outputs TODO
   * @param f TODO
   * @param h TODO
   * @param stateStdDevs TODO
   * @param measurementStdDevs TODO
   */
  @SuppressWarnings("ParameterName")
  public UnscentedKalmanFilter(Nat<S> states, Nat<I> inputs, Nat<O> outputs,
                               BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<S, N1>> f,
                               BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<O, N1>> h,
                               Matrix<S, N1> stateStdDevs,
                               Matrix<O, N1> measurementStdDevs) {
    this.states = states;
    this.m_inputs = inputs;
    this.m_outputs = outputs;

    m_f = f;
    m_h = h;
    m_Q = StateSpaceUtils.makeCovMatrix(states, stateStdDevs);
    m_R = StateSpaceUtils.makeCovMatrix(outputs, measurementStdDevs);

    reset();
  }

  @SuppressWarnings("ParameterName")
  private static <S extends Num, C extends Num>
  SimpleMatrixUtils.Pair<Matrix<N1, C>, Matrix<C, C>> unscentedTransform(
          S s, C dim, Matrix sigmas, Matrix Wm, Matrix Wc, Matrix noiseCov
  ) {
    if (sigmas.getNumCols() != 2 * s.getNum() + 1 || sigmas.getNumCols() != dim.getNum()) {
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

    return null;
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
    m_xHat = new Matrix<>(new SimpleMatrix(states.getNum(), 1));
    m_P = new Matrix<>(new SimpleMatrix(states.getNum(), states.getNum()));
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u         New control input from controller.
   * @param dtSeconds Timestep for prediction.
   */
  public void predict(Matrix<I, N1> u, double dtSeconds) {
    var sigmas = m_pts.sigmaPoints(m_xHat, m_P);

    for (int i = 0; i < m_pts.getNumSigns(); ++i) {

      var temp = new SimpleMatrix(1, states.getNum());
      CommonOps_DDRM.extract(sigmas.getStorage().getDDRM(), i, 0, temp.getDDRM());
      var x = new Matrix<S, N1>(temp.transpose());

      // set the block from i, 0 to i+1, States to the result of RungeKutta
      //var rungeKutta = RungeKuttaHelper.RungeKutta(m_f, x, u, dtSeconds).transpose();

      //Eigen::Matrix<double, States, 1> x =
      //        sigmas.template block<1, States>(i, 0).transpose();
      //m_sigmasF.template block<1, States>(i, 0) =
      //RungeKutta(m_f, x, u, dt).transpose();

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
    correct(u, y, m_h, m_R);
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
  @SuppressWarnings("ParameterName")
  public void correct(
          Matrix<I, N1> u,
          Matrix<O, N1> y,
          BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<O, N1>> h,
          Matrix<O, O> R) {

  }


}
