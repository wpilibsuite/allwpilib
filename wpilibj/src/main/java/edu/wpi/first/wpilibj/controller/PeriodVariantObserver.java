/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import java.util.ArrayList;
import java.util.List;

import org.ejml.dense.row.CommonOps_DDRM;
import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.SimpleMatrixUtils;
import edu.wpi.first.wpiutil.math.numbers.N1;

/**
 * A Kalman filter that discretizes its model and covariance matrices after
 * every sample period.
 *
 * <p>Typical discrete state feedback implementations discretize with a nominal
 * sample period offline. If the real system doesn't maintain this period, this
 * nonlinearity can negatively affect the state estimate. This class discretizes
 * the continuous model after each measurement based on the measured sample
 * period.
 *
 * <p>Since the sample period changes during runtime, the process and measurement
 * noise covariance matrices as well as the true steady state error covariance
 * change as well. During runtime, the error covariance matrix is initialized
 * with the discrete steady state value, but is updated during runtime as well.
 *
 * <p>For more on the underlying math, read
 * https://file.tavsys.net/control/state-space-guide.pdf.
 */
@SuppressWarnings({"ClassTypeParameterName", "MemberName"})
public class PeriodVariantObserver<States extends Num, Inputs extends Num, Outputs extends Num> {
  private PeriodVariantPlant<States, Inputs, Outputs> m_plant;

  private double m_nominalSamplePeriod;

  /**
   * Internal state estimate.
   */
  private Matrix<States, N1> m_Xhat;

  /**
   * Internal error covariance estimate.
   */
  private Matrix<States, States> m_P;

  /**
   * Discretized process noise covariance matrix.
   */
  private Matrix<States, States> m_Q;
  /**
   * Discretized measurement noise covariance matrix.
   */
  private Matrix<Outputs, Outputs> m_R;

  @SuppressWarnings("LineLength")
  private final List<PeriodVariantObserverCoeffs<States, Inputs, Outputs>> m_coefficients = new ArrayList<>();
  private int m_index;

  /**
   * Rediscretizes Q and R with the provided timestep.
   *
   * @param dt Discretization timestep.
   */
  @SuppressWarnings({"LineLength", "LocalVariableName"})
  private void updateQR(double dt) {
    int states = m_plant.getStates().getNum();
    SimpleMatrix Qtemp = (getCoefficients().getQcontinuous().plus(getCoefficients().getQcontinuous().transpose()))
        .div(2.0).getStorage();
    final SimpleMatrix Rtemp = (getCoefficients().getRcontinuous().plus(getCoefficients().getRcontinuous().transpose()))
        .div(2.0).getStorage();

    Qtemp.concatRows(m_plant.getCoefficients().getAcontinuous().getStorage().transpose());

    SimpleMatrix Mgain = new SimpleMatrix(states * 2, states * 2);
    Mgain.concatColumns(m_plant.getCoefficients().getAcontinuous().getStorage().negative(), Qtemp);

    SimpleMatrix phi = SimpleMatrixUtils.expm(Mgain.scale(dt));
    SimpleMatrix phi12 = new SimpleMatrix(states, states);
    CommonOps_DDRM.extract(phi.getDDRM(), 0, states, phi12.getDDRM());
    SimpleMatrix phi22 = new SimpleMatrix(states, states);
    CommonOps_DDRM.extract(phi.getDDRM(), states, states, phi22.getDDRM());

    m_Q = new Matrix<>(phi22.transpose().mult(phi12));
    m_Q = (m_Q.plus(m_Q.transpose())).div(2.0);
    m_R = new Matrix<>(Rtemp.divide(dt));
  }

  /**
   * Constructs a Kalman filter with the given coefficients and plant.
   *
   * @param observerCoeffs      Observer coefficients.
   * @param plant               The plant used for the prediction step.
   * @param nominalSamplePeriod The nominal period at which the control loop
   *                            will run.
   */
  public PeriodVariantObserver(PeriodVariantObserverCoeffs<States, Inputs, Outputs> observerCoeffs,
                               PeriodVariantPlant<States, Inputs, Outputs> plant,
                               double nominalSamplePeriod) {
    addCoefficients(observerCoeffs);
    m_plant = plant;
    m_nominalSamplePeriod = nominalSamplePeriod;
  }

  /**
   * Constructs a Kalman filter with the given coefficients and plant,
   * with a nominal sample period of 0.005s.
   *
   * @param coeffs Observer coefficients.
   * @param plant  The plant used for the prediction step
   */
  public PeriodVariantObserver(PeriodVariantObserverCoeffs<States, Inputs, Outputs> coeffs,
                               PeriodVariantPlant<States, Inputs, Outputs> plant) {
    this(coeffs, plant, 0.005);
  }

  /**
   * Returns the discretized process noise covariance matrix.
   */
  public Matrix<States, States> getQ() {
    return m_Q;
  }

  /**
   * Returns an element of the discretized process noise covariance matrix.
   *
   * @param i Row in Q.
   * @param j Column in Q.
   */
  @SuppressWarnings("ParameterName")
  public double getQ(int i, int j) {
    return getQ().get(i, j);
  }

  /**
   * Returns the discretized measurement noise covariance matrix.
   */
  public Matrix<Outputs, Outputs> getR() {
    return m_R;
  }

  /**
   * Returns an element of the discretized measurement noise covariance matrix.
   *
   * @param i Row of R.
   * @param j Column of R.
   */
  @SuppressWarnings("ParameterName")
  public double getR(int i, int j) {
    return getR().get(i, j);
  }

  /**
   * Returns the discretized error covariance matrix.
   */
  public Matrix<States, States> getP() {
    return m_P;
  }

  /**
   * Returns an element of the discretized error covariance matrix.
   *
   * @param i Row of P.
   * @param j Column of P.
   */
  @SuppressWarnings("ParameterName")
  public double getP(int i, int j) {
    return getP().get(i, j);
  }

  /**
   * Returns the state estimate x-hat.
   */
  public Matrix<States, N1> getXhat() {
    return m_Xhat;
  }

  /**
   * Returns an element of the state estimate x-hat.
   *
   * @param i Row of x-hat.
   */
  @SuppressWarnings("ParameterName")
  public double getXhat(int i) {
    return getXhat().get(i, 0);
  }

  /**
   * Set initial state estimate x-hat.
   *
   * @param xHat The state estimate x-hat.
   */
  @SuppressWarnings("ParameterName")
  public void setXhat(Matrix<States, N1> xHat) {
    m_Xhat = xHat;
  }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param i     Row of x-hat.
   * @param value Value for element of x-hat.
   */
  @SuppressWarnings("ParameterName")
  public void setXhat(int i, double value) {
    m_Xhat.set(i, 0, value);
  }

  /**
   * Resets the observer.
   */
  public void reset() {
    m_Xhat = MatrixUtils.zeros(m_plant.getStates());
    m_P = getCoefficients().getPsteadyState();
    updateQR(m_nominalSamplePeriod);
  }

  /**
   * Project the model into the future with a new control input u.
   *
   * @param newU New control input from controller.
   * @param dt   Timestep for prediction.
   */
  public void predict(Matrix<Inputs, N1> newU, double dt) {
    m_Xhat = m_plant.updateX(getXhat(), newU, dt);

    updateQR(dt);

    m_P = m_plant.getA().times(getP()).times(m_plant.getA().transpose()).plus(m_Q);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param u Same control input used in the predict step.
   * @param y Measurement vector.
   */
  @SuppressWarnings({"ParameterName", "LocalVariableName", "LineLength"})
  public void correct(Matrix<Inputs, N1> u, Matrix<Outputs, N1> y) {
    Matrix<Outputs, N1> yBar = y.minus(m_plant.updateY(getXhat(), u));
    Matrix<Outputs, Outputs> S = m_plant.getC().times(getP()).times(m_plant.getC().transpose()).plus(getR());
    Matrix<States, Outputs> kalmanGain = getP().times(m_plant.getC().transpose()).times(S.inv());

    m_Xhat = getXhat().plus(kalmanGain.times(yBar));
    m_P = (MatrixUtils.eye(m_plant.getStates()).minus(kalmanGain.times(m_plant.getC()))).times(getP());
  }

  /**
   * Adds the given coefficients to the observer for gain scheduling.
   *
   * @param coefficients Observer coefficients.
   */
  public void addCoefficients(PeriodVariantObserverCoeffs<States, Inputs, Outputs> coefficients) {
    m_coefficients.add(coefficients);
  }

  /**
   * Returns the observer coefficients with the given index.
   *
   * @param index Index of coefficients.
   */
  public PeriodVariantObserverCoeffs<States, Inputs, Outputs> getCoefficients(int index) {
    return m_coefficients.get(index);
  }

  /**
   * Returns the current observer coefficients.
   */
  public PeriodVariantObserverCoeffs<States, Inputs, Outputs> getCoefficients() {
    return getCoefficients(m_index);
  }

  /**
   * Sets the current observer to be "index", clamped to be within range. This
   * can be used for gain scheduling.
   *
   * @param index The controller index.
   */
  public void setIndex(int index) {
    if (index < 0) {
      m_index = 0;
    } else if (index >= m_coefficients.size()) {
      m_index = m_coefficients.size() - 1;
    } else {
      m_index = index;
    }
  }

  /**
   * Returns the current observer index.
   */
  public int getIndex() {
    return m_index;
  }
}
