// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.estimator;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Nat;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.Pair;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.numbers.N1;
import edu.wpi.first.math.system.Discretization;
import edu.wpi.first.math.system.NumericalIntegration;
import edu.wpi.first.math.system.NumericalJacobian;
import java.util.function.BiFunction;
import org.ejml.dense.row.decomposition.qr.QRDecompositionHouseholder_DDRM;
import org.ejml.simple.SimpleMatrix;

/**
 * A Kalman filter combines predictions from a model and measurements to give an estimate of the
 * true system state. This is useful because many states cannot be measured directly as a result of
 * sensor noise, or because the state is "hidden".
 *
 * <p>Kalman filters use a K gain matrix to determine whether to trust the model or measurements
 * more. Kalman filter theory uses statistics to compute an optimal K gain which minimizes the sum
 * of squares error in the state estimate. This K gain is used to correct the state estimate by some
 * amount of the difference between the actual measurements and the measurements predicted by the
 * model.
 *
 * <p>An unscented Kalman filter uses nonlinear state and measurement models. It propagates the
 * error covariance using sigma points chosen to approximate the true probability distribution.
 *
 * <p>For more on the underlying math, read <a
 * href="https://file.tavsys.net/control/controls-engineering-in-frc.pdf">https://file.tavsys.net/control/controls-engineering-in-frc.pdf</a>
 * chapter 9 "Stochastic control theory".
 *
 * <p>This class implements a square-root-form unscented Kalman filter (SR-UKF). The main reason for
 * this is to guarantee that the covariance matrix remains positive definite. For more information
 * about the SR-UKF, see https://www.researchgate.net/publication/3908304.
 *
 * @param <States> Number of states.
 * @param <Inputs> Number of inputs.
 * @param <Outputs> Number of outputs.
 */
public class UnscentedKalmanFilter<States extends Num, Inputs extends Num, Outputs extends Num>
    implements KalmanTypeFilter<States, Inputs, Outputs> {
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
  private Matrix<States, States> m_S;
  private final Matrix<States, States> m_contQ;
  private final Matrix<Outputs, Outputs> m_contR;
  private Matrix<States, ?> m_sigmasF;
  private double m_dtSeconds;

  private final MerweScaledSigmaPoints<States> m_pts;

  /**
   * Constructs an Unscented Kalman Filter.
   *
   * <p>See <a
   * href="https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices">https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices</a>
   * for how to select the standard deviations.
   *
   * @param states A Nat representing the number of states.
   * @param outputs A Nat representing the number of outputs.
   * @param f A vector-valued function of x and u that returns the derivative of the state vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param stateStdDevs Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param nominalDtSeconds Nominal discretization timestep.
   */
  public UnscentedKalmanFilter(
      Nat<States> states,
      Nat<Outputs> outputs,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> h,
      Matrix<States, N1> stateStdDevs,
      Matrix<Outputs, N1> measurementStdDevs,
      double nominalDtSeconds) {
    this(
        states,
        outputs,
        f,
        h,
        stateStdDevs,
        measurementStdDevs,
        (sigmas, Wm) -> sigmas.times(Matrix.changeBoundsUnchecked(Wm)),
        (sigmas, Wm) -> sigmas.times(Matrix.changeBoundsUnchecked(Wm)),
        Matrix::minus,
        Matrix::minus,
        Matrix::plus,
        nominalDtSeconds);
  }

  /**
   * Constructs an Unscented Kalman filter with custom mean, residual, and addition functions. Using
   * custom functions for arithmetic can be useful if you have angles in the state or measurements,
   * because they allow you to correctly account for the modular nature of angle arithmetic.
   *
   * <p>See <a
   * href="https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices">https://docs.wpilib.org/en/stable/docs/software/advanced-controls/state-space/state-space-observers.html#process-and-measurement-noise-covariance-matrices</a>
   * for how to select the standard deviations.
   *
   * @param states A Nat representing the number of states.
   * @param outputs A Nat representing the number of outputs.
   * @param f A vector-valued function of x and u that returns the derivative of the state vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param stateStdDevs Standard deviations of model states.
   * @param measurementStdDevs Standard deviations of measurements.
   * @param meanFuncX A function that computes the mean of 2 * States + 1 state vectors using a
   *     given set of weights.
   * @param meanFuncY A function that computes the mean of 2 * States + 1 measurement vectors using
   *     a given set of weights.
   * @param residualFuncX A function that computes the residual of two state vectors (i.e. it
   *     subtracts them.)
   * @param residualFuncY A function that computes the residual of two measurement vectors (i.e. it
   *     subtracts them.)
   * @param addFuncX A function that adds two state vectors.
   * @param nominalDtSeconds Nominal discretization timestep.
   */
  public UnscentedKalmanFilter(
      Nat<States> states,
      Nat<Outputs> outputs,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<States, N1>> f,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<Outputs, N1>> h,
      Matrix<States, N1> stateStdDevs,
      Matrix<Outputs, N1> measurementStdDevs,
      BiFunction<Matrix<States, ?>, Matrix<?, N1>, Matrix<States, N1>> meanFuncX,
      BiFunction<Matrix<Outputs, ?>, Matrix<?, N1>, Matrix<Outputs, N1>> meanFuncY,
      BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> residualFuncX,
      BiFunction<Matrix<Outputs, N1>, Matrix<Outputs, N1>, Matrix<Outputs, N1>> residualFuncY,
      BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> addFuncX,
      double nominalDtSeconds) {
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

  static <S extends Num, C extends Num>
      Pair<Matrix<C, N1>, Matrix<C, C>> squareRootUnscentedTransform(
          Nat<S> s,
          Nat<C> dim,
          Matrix<C, ?> sigmas,
          Matrix<?, N1> Wm,
          Matrix<?, N1> Wc,
          BiFunction<Matrix<C, ?>, Matrix<?, N1>, Matrix<C, N1>> meanFunc,
          BiFunction<Matrix<C, N1>, Matrix<C, N1>, Matrix<C, N1>> residualFunc,
          Matrix<C, C> squareRootR) {
    if (sigmas.getNumRows() != dim.getNum() || sigmas.getNumCols() != 2 * s.getNum() + 1) {
      throw new IllegalArgumentException(
          "Sigmas must be covDim by 2 * states + 1! Got "
              + sigmas.getNumRows()
              + " by "
              + sigmas.getNumCols());
    }

    if (Wm.getNumRows() != 2 * s.getNum() + 1 || Wm.getNumCols() != 1) {
      throw new IllegalArgumentException(
          "Wm must be 2 * states + 1 by 1! Got " + Wm.getNumRows() + " by " + Wm.getNumCols());
    }

    if (Wc.getNumRows() != 2 * s.getNum() + 1 || Wc.getNumCols() != 1) {
      throw new IllegalArgumentException(
          "Wc must be 2 * states + 1 by 1! Got " + Wc.getNumRows() + " by " + Wc.getNumCols());
    }

    // New mean is usually just the sum of the sigmas * weights:
    //
    //      2n
    //   x̂ = Σ Wᵢ⁽ᵐ⁾𝒳ᵢ
    //      i=0
    //
    // equations (19) and (23) in the paper show this,
    // but we allow a custom function, usually for angle wrapping
    Matrix<C, N1> x = meanFunc.apply(sigmas, Wm);

    // Form an intermediate matrix S⁻ as:
    //
    //   [√{W₁⁽ᶜ⁾}(𝒳_{1:2L} - x̂) √{Rᵛ}]
    //
    // the part of equations (20) and (24) within the "qr{}"
    Matrix<C, ?> Sbar = new Matrix<>(new SimpleMatrix(dim.getNum(), 2 * s.getNum() + dim.getNum()));
    for (int i = 0; i < 2 * s.getNum(); i++) {
      Sbar.setColumn(
          i,
          residualFunc.apply(sigmas.extractColumnVector(1 + i), x).times(Math.sqrt(Wc.get(1, 0))));
    }
    Sbar.assignBlock(0, 2 * s.getNum(), squareRootR);

    QRDecompositionHouseholder_DDRM qr = new QRDecompositionHouseholder_DDRM();
    var qrStorage = Sbar.transpose().getStorage();

    if (!qr.decompose(qrStorage.getDDRM())) {
      throw new RuntimeException("QR decomposition failed! Input matrix:\n" + qrStorage);
    }

    // Compute the square-root covariance of the sigma points
    //
    // We transpose S⁻ first because we formed it by horizontally
    // concatenating each part; it should be vertical so we can take
    // the QR decomposition as defined in the "QR Decomposition" passage
    // of section 3. "EFFICIENT SQUARE-ROOT IMPLEMENTATION"
    //
    // The resulting matrix R is the square-root covariance S, but it
    // is upper triangular, so we need to transpose it.
    //
    // equations (20) and (24)
    Matrix<C, C> newS = new Matrix<>(new SimpleMatrix(qr.getR(null, true)).transpose());

    // Update or downdate the square-root covariance with (𝒳₀-x̂)
    // depending on whether its weight (W₀⁽ᶜ⁾) is positive or negative.
    //
    // equations (21) and (25)
    newS.rankUpdate(residualFunc.apply(sigmas.extractColumnVector(0), x), Wc.get(0, 0), true);

    return new Pair<>(x, newS);
  }

  /**
   * Returns the square-root error covariance matrix S.
   *
   * @return the square-root error covariance matrix S.
   */
  public Matrix<States, States> getS() {
    return m_S;
  }

  /**
   * Returns an element of the square-root error covariance matrix S.
   *
   * @param row Row of S.
   * @param col Column of S.
   * @return the value of the square-root error covariance matrix S at (i, j).
   */
  public double getS(int row, int col) {
    return m_S.get(row, col);
  }

  /**
   * Sets the entire square-root error covariance matrix S.
   *
   * @param newS The new value of S to use.
   */
  public void setS(Matrix<States, States> newS) {
    m_S = newS;
  }

  /**
   * Returns the reconstructed error covariance matrix P.
   *
   * @return the error covariance matrix P.
   */
  @Override
  public Matrix<States, States> getP() {
    return m_S.times(m_S.transpose());
  }

  /**
   * Returns an element of the error covariance matrix P.
   *
   * @param row Row of P.
   * @param col Column of P.
   * @return the value of the error covariance matrix P at (i, j).
   * @throws UnsupportedOperationException indexing into the reconstructed P matrix is not supported
   */
  @Override
  public double getP(int row, int col) {
    throw new UnsupportedOperationException(
        "indexing into the reconstructed P matrix is not supported");
  }

  /**
   * Sets the entire error covariance matrix P.
   *
   * @param newP The new value of P to use.
   */
  @Override
  public void setP(Matrix<States, States> newP) {
    m_S = newP.lltDecompose(true);
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
   * @return the value of the state estimate x-hat at 'i'.
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
  @Override
  public void setXhat(Matrix<States, N1> xHat) {
    m_xHat = xHat;
  }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param row Row of x-hat.
   * @param value Value for element of x-hat.
   */
  @Override
  public void setXhat(int row, double value) {
    m_xHat.set(row, 0, value);
  }

  /** Resets the observer. */
  @Override
  public final void reset() {
    m_xHat = new Matrix<>(m_states, Nat.N1());
    m_S = new Matrix<>(m_states, m_states);
    m_sigmasF = new Matrix<>(new SimpleMatrix(m_states.getNum(), 2 * m_states.getNum() + 1));
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param u New control input from controller.
   * @param dtSeconds Timestep for prediction.
   */
  @Override
  public void predict(Matrix<Inputs, N1> u, double dtSeconds) {
    // Discretize Q before projecting mean and covariance forward
    Matrix<States, States> contA =
        NumericalJacobian.numericalJacobianX(m_states, m_states, m_f, m_xHat, u);
    var discQ = Discretization.discretizeAQ(contA, m_contQ, dtSeconds).getSecond();
    var squareRootDiscQ = discQ.lltDecompose(true);

    // Generate sigma points around the state mean
    //
    // equation (17)
    var sigmas = m_pts.squareRootSigmaPoints(m_xHat, m_S);

    // Project each sigma point forward in time according to the
    // dynamics f(x, u)
    //
    //   sigmas  = 𝒳ₖ₋₁
    //   sigmasF = 𝒳ₖ,ₖ₋₁ or just 𝒳 for readability
    //
    // equation (18)
    for (int i = 0; i < m_pts.getNumSigmas(); ++i) {
      Matrix<States, N1> x = sigmas.extractColumnVector(i);

      m_sigmasF.setColumn(i, NumericalIntegration.rk4(m_f, x, u, dtSeconds));
    }

    // Pass the predicted sigmas (𝒳) through the Unscented Transform
    // to compute the prior state mean and covariance
    //
    // equations (18) (19) and (20)
    var ret =
        squareRootUnscentedTransform(
            m_states,
            m_states,
            m_sigmasF,
            m_pts.getWm(),
            m_pts.getWc(),
            m_meanFuncX,
            m_residualFuncX,
            squareRootDiscQ);

    m_xHat = ret.getFirst();
    m_S = ret.getSecond();
    m_dtSeconds = dtSeconds;
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  @Override
  public void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y) {
    correct(
        m_outputs, u, y, m_h, m_contR, m_meanFuncY, m_residualFuncY, m_residualFuncX, m_addFuncX);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * <p>This is useful for when the measurement noise covariances vary.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param R Continuous measurement noise covariance matrix.
   */
  public void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y, Matrix<Outputs, Outputs> R) {
    correct(m_outputs, u, y, m_h, R, m_meanFuncY, m_residualFuncY, m_residualFuncX, m_addFuncX);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * <p>This is useful for when the measurements available during a timestep's Correct() call vary.
   * The h(x, u) passed to the constructor is used if one is not provided (the two-argument version
   * of this function).
   *
   * @param <R> Number of measurements in y.
   * @param rows Number of rows in y.
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param R Continuous measurement noise covariance matrix.
   */
  public <R extends Num> void correct(
      Nat<R> rows,
      Matrix<Inputs, N1> u,
      Matrix<R, N1> y,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<R, N1>> h,
      Matrix<R, R> R) {
    BiFunction<Matrix<R, ?>, Matrix<?, N1>, Matrix<R, N1>> meanFuncY =
        (sigmas, Wm) -> sigmas.times(Matrix.changeBoundsUnchecked(Wm));
    BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> residualFuncX =
        Matrix::minus;
    BiFunction<Matrix<R, N1>, Matrix<R, N1>, Matrix<R, N1>> residualFuncY = Matrix::minus;
    BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> addFuncX = Matrix::plus;
    correct(rows, u, y, h, R, meanFuncY, residualFuncY, residualFuncX, addFuncX);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * <p>This is useful for when the measurements available during a timestep's Correct() call vary.
   * The h(x, u) passed to the constructor is used if one is not provided (the two-argument version
   * of this function).
   *
   * @param <R> Number of measurements in y.
   * @param rows Number of rows in y.
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   * @param h A vector-valued function of x and u that returns the measurement vector.
   * @param R Continuous measurement noise covariance matrix.
   * @param meanFuncY A function that computes the mean of 2 * States + 1 measurement vectors using
   *     a given set of weights.
   * @param residualFuncY A function that computes the residual of two measurement vectors (i.e. it
   *     subtracts them.)
   * @param residualFuncX A function that computes the residual of two state vectors (i.e. it
   *     subtracts them.)
   * @param addFuncX A function that adds two state vectors.
   */
  public <R extends Num> void correct(
      Nat<R> rows,
      Matrix<Inputs, N1> u,
      Matrix<R, N1> y,
      BiFunction<Matrix<States, N1>, Matrix<Inputs, N1>, Matrix<R, N1>> h,
      Matrix<R, R> R,
      BiFunction<Matrix<R, ?>, Matrix<?, N1>, Matrix<R, N1>> meanFuncY,
      BiFunction<Matrix<R, N1>, Matrix<R, N1>, Matrix<R, N1>> residualFuncY,
      BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> residualFuncX,
      BiFunction<Matrix<States, N1>, Matrix<States, N1>, Matrix<States, N1>> addFuncX) {
    final var discR = Discretization.discretizeR(R, m_dtSeconds);
    final var squareRootDiscR = discR.lltDecompose(true);

    // Generate new sigma points from the prior mean and covariance
    // and transform them into measurement space using h(x, u)
    //
    //   sigmas  = 𝒳
    //   sigmasH = 𝒴
    //
    // This differs from equation (22) which uses
    // the prior sigma points, regenerating them allows
    // multiple measurement updates per time update
    Matrix<R, ?> sigmasH = new Matrix<>(new SimpleMatrix(rows.getNum(), 2 * m_states.getNum() + 1));
    var sigmas = m_pts.squareRootSigmaPoints(m_xHat, m_S);
    for (int i = 0; i < m_pts.getNumSigmas(); i++) {
      Matrix<R, N1> hRet = h.apply(sigmas.extractColumnVector(i), u);
      sigmasH.setColumn(i, hRet);
    }

    // Pass the predicted measurement sigmas through the Unscented Transform
    // to compute the mean predicted measurement and square-root innovation
    // covariance.
    //
    // equations (23) (24) and (25)
    var transRet =
        squareRootUnscentedTransform(
            m_states,
            rows,
            sigmasH,
            m_pts.getWm(),
            m_pts.getWc(),
            meanFuncY,
            residualFuncY,
            squareRootDiscR);
    var yHat = transRet.getFirst();
    var Sy = transRet.getSecond();

    // Compute cross covariance of the predicted state and measurement sigma
    // points given as:
    //
    //           2n
    //   P_{xy} = Σ Wᵢ⁽ᶜ⁾[𝒳ᵢ - x̂][𝒴ᵢ - ŷ⁻]ᵀ
    //           i=0
    //
    // equation (26)
    Matrix<States, R> Pxy = new Matrix<>(m_states, rows);
    for (int i = 0; i < m_pts.getNumSigmas(); i++) {
      var dx = residualFuncX.apply(m_sigmasF.extractColumnVector(i), m_xHat);
      var dy = residualFuncY.apply(sigmasH.extractColumnVector(i), yHat).transpose();

      Pxy = Pxy.plus(dx.times(dy).times(m_pts.getWc(i)));
    }

    // Compute the Kalman gain. We use Eigen's QR decomposition to solve. This
    // is equivalent to MATLAB's \ operator, so we need to rearrange to use
    // that.
    //
    //   K = (P_{xy} / S_{y}ᵀ) / S_{y}
    //   K = (S_{y} \ P_{xy})ᵀ / S_{y}
    //   K = (S_{y}ᵀ \ (S_{y} \ P_{xy}ᵀ))ᵀ
    //
    // equation (27)
    Matrix<States, R> K =
        Sy.transpose()
            .solveFullPivHouseholderQr(Sy.solveFullPivHouseholderQr(Pxy.transpose()))
            .transpose();

    // Compute the posterior state mean
    //
    // x̂ = x̂⁻ + K(y − ŷ⁻)
    //
    // second part of equation (27)
    m_xHat = addFuncX.apply(m_xHat, K.times(residualFuncY.apply(y, yHat)));

    // Compute the intermediate matrix U for downdating
    // the square-root covariance
    //
    // equation (28)
    Matrix<States, R> U = K.times(Sy);

    // Downdate the posterior square-root state covariance
    //
    // equation (29)
    for (int i = 0; i < rows.getNum(); i++) {
      m_S.rankUpdate(U.extractColumnVector(i), -1, true);
    }
  }
}
