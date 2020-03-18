package edu.wpi.first.wpilibj.estimator;

import edu.wpi.first.wpilibj.math.StateSpaceUtils;
import edu.wpi.first.wpiutil.math.*;
import edu.wpi.first.wpiutil.math.numbers.N1;
import org.ejml.dense.row.CommonOps_DDRM;
import org.ejml.simple.SimpleMatrix;

import java.util.function.BiFunction;


public class UnscentedKalmanFilter<S extends Num, I extends Num,
        O extends Num> {

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
  private final Nat<I> inputs;
  /**
   * The outputs of the system.
   */
  private final Nat<O> outputs;
  private Matrix<S, N1> m_xHat;
  private Matrix<S, S> m_P;
  private Matrix m_sigmasF;
  // TODO where is this initilized?
  private MerweScaledSigmaPoints<S> m_pts;

  public UnscentedKalmanFilter(Nat<S> states, Nat<I> inputs, Nat<O> outputs,
                               BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<S, N1>> f,
                               BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<O, N1>> h,
                               Matrix<S, N1> stateStdDevs,
                               Matrix<O, N1> measurementStdDevs) {
    this.states = states;
    this.inputs = inputs;
    this.outputs = outputs;

    m_f = f;
    m_h = h;
    m_Q = StateSpaceUtils.makeCovMatrix(states, stateStdDevs);
    m_R = StateSpaceUtils.makeCovMatrix(outputs, measurementStdDevs);

    reset();
  }

  @SuppressWarnings("ParameterName")
  public static <S extends Num, CovDim extends Num>
  SimpleMatrixUtils.Pair<Matrix<N1, CovDim>, Matrix<CovDim, CovDim>> unscentedTransform(
          S s, CovDim dim, Matrix sigmas, Matrix Wm, Matrix Wc, Matrix noiseCov
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
   * Resets the observer.
   */
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
//            var rungeKutta = RungeKuttaHelper.RungeKutta(m_f, x, u, dtSeconds).transpose();

//            Eigen::Matrix<double, States, 1> x =
//                    sigmas.template block<1, States>(i, 0).transpose();
//            m_sigmasF.template block<1, States>(i, 0) =
//            RungeKutta(m_f, x, u, dt).transpose();

    }

  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   * <p>
   * This is useful for when the measurements available during a timestep's
   * Correct() call vary. The h(x, u) passed to the constructor is used if one
   * is not provided (the two-argument version of this function).
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param h A vector-valued function of x and u that returns
   *          the measurement vector.
   * @param R Measurement noise covariance matrix.
   */
  public void correct(
          Matrix<I, N1> u,
          Matrix<O, N1> y,
          BiFunction<Matrix<S, N1>, Matrix<I, N1>, Matrix<O, N1>> h,
          Matrix<O, O> R) {

  }


}
