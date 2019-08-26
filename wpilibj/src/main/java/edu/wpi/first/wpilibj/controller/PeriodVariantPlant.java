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
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.SimpleMatrixUtils;
import edu.wpi.first.wpiutil.math.numbers.N1;

@SuppressWarnings({"ClassTypeParameterName", "MemberName"})
public class PeriodVariantPlant<States extends Num, Inputs extends Num, Outputs extends Num> {
  private final double m_nominalSamplePeriod;

  // The number of states, inputs, and outputs for this plant
  private final Nat<States> kStates;
  private final Nat<Inputs> kInputs;
  private final Nat<Outputs> kOutputs;

  /**
   * Current state.
   */
  private Matrix<States, N1> m_x;

  /**
   * Current output.
   */
  private Matrix<Outputs, N1> m_y;

  /**
   * Discrete system matrix.
   */
  private Matrix<States, States> m_A;

  /**
   * Discrete input matrix.
   */
  private Matrix<States, Inputs> m_B;

  /**
   * Delayed U since predict and correct steps are run in reverse.
   */
  private Matrix<Inputs, N1> m_delayedU;

  @SuppressWarnings("LineLength")
  private final List<PeriodVariantPlantCoeffs<States, Inputs, Outputs>> m_coefficients = new ArrayList<>();
  private int m_index;

  /**
   * Constructs a plant with the given coefficients.
   *
   * @param plantCoeffs         Plant coefficients.
   * @param nominalSamplePeriod The nominal period at which the control loop
   *                            will run.
   */
  public PeriodVariantPlant(PeriodVariantPlantCoeffs<States, Inputs, Outputs> plantCoeffs,
                            double nominalSamplePeriod) {
    m_nominalSamplePeriod = nominalSamplePeriod;
    kStates = plantCoeffs.getStates();
    kInputs = plantCoeffs.getInputs();
    kOutputs = plantCoeffs.getOutputs();

    addCoefficients(plantCoeffs);
    reset();
  }

  /**
   * Constructs a plant with the given coefficients and a nominal sample period of 0.005s
   *
   * @param coeffs Plant coefficients.
   */
  public PeriodVariantPlant(PeriodVariantPlantCoeffs<States, Inputs, Outputs> coeffs) {
    this(coeffs, 0.005);
  }

  /**
   * Returns the system matrix A.
   */
  public Matrix<States, States> getA() {
    return m_A;
  }

  /**
   * Returns an element of the system matrix A.
   *
   * @param i Row of A.
   * @param j Column of A.
   */
  @SuppressWarnings("ParameterName")
  public double getA(int i, int j) {
    return getA().get(i, j);
  }

  /**
   * Returns the input matrix B.
   */
  public Matrix<States, Inputs> getB() {
    return m_B;
  }

  /**
   * Returns an element of the input matrix B.
   *
   * @param i Row of B.
   * @param j Column of B.
   */
  @SuppressWarnings("ParameterName")
  public double getB(int i, int j) {
    return getB().get(i, j);
  }

  /**
   * Returns the output matrix C.
   */
  public Matrix<Outputs, States> getC() {
    return getCoefficients().getC();
  }

  /**
   * Returns an element of the output matrix C.
   *
   * @param i Row of C.
   * @param j Column of C.
   */
  @SuppressWarnings("ParameterName")
  public double getC(int i, int j) {
    return getC().get(i, j);
  }

  /**
   * Returns the feedthrough matrix D.
   */
  public Matrix<Outputs, Inputs> getD() {
    return getCoefficients().getD();
  }

  /**
   * Returns an element of the feedthrough matrix D.
   *
   * @param i Row of D.
   * @param j Column of D.
   */
  @SuppressWarnings("ParameterName")
  public double getD(int i, int j) {
    return getD().get(i, j);
  }

  /**
   * Returns the current state X.
   */
  public Matrix<States, N1> getX() {
    return m_x;
  }

  /**
   * Returns an element of the current state x.
   *
   * @param i Row of x.
   */
  @SuppressWarnings("ParameterName")
  public double getX(int i) {
    return getX().get(i, 0);
  }

  /**
   * Returns the current measurement vector y.
   */
  public Matrix<Outputs, N1> getY() {
    return m_y;
  }

  /**
   * Returns an element of the current measurement vector y.
   *
   * @param i Row of y.
   */
  @SuppressWarnings("ParameterName")
  public double getY(int i) {
    return getY().get(i, 0);
  }

  /**
   * Set the initial state x.
   *
   * @param x The initial state.
   */
  @SuppressWarnings("ParameterName")
  public void setX(Matrix<States, N1> x) {
    m_x = x;
  }

  /**
   * Set an element of the initial state x.
   *
   * @param i     Row of x.
   * @param value Value of element of x.
   */
  @SuppressWarnings("ParameterName")
  public void setX(int i, double value) {
    m_x.set(i, 0, value);
  }

  /**
   * Set the current measurement y.
   *
   * @param y The current measurement.
   */
  @SuppressWarnings("ParameterName")
  public void setY(Matrix<Outputs, N1> y) {
    m_y = y;
  }

  /**
   * Set an element of the current measurement y.
   *
   * @param i     Row of y.
   * @param value Value of element of y.
   */
  @SuppressWarnings("ParameterName")
  public void setY(int i, double value) {
    m_y.set(i, 0, value);
  }

  /**
   * Adds the given coefficients to the plant for gain scheduling.
   *
   * @param coefficients Plant coefficients.
   */
  public void addCoefficients(PeriodVariantPlantCoeffs<States, Inputs, Outputs> coefficients) {
    m_coefficients.add(coefficients);
  }

  /**
   * Returns the plant coefficients with the given index.
   *
   * @param index Index of coefficients.
   */
  public PeriodVariantPlantCoeffs<States, Inputs, Outputs> getCoefficients(int index) {
    return m_coefficients.get(index);
  }

  /**
   * Returns the current plant coefficients.
   */
  public PeriodVariantPlantCoeffs<States, Inputs, Outputs> getCoefficients() {
    return getCoefficients(getIndex());
  }

  /**
   * Sets the current plant to be "index", clamped to be within range. This can
   * be used for gain scheduling to make the current model match changed plant
   * behavior.
   *
   * @param index The plant index.
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
   * Returns the current plant index.
   */
  public int getIndex() {
    return m_index;
  }

  /**
   * Resets the plant.
   */
  public void reset() {
    m_x = MatrixUtils.zeros(kStates);
    m_y = MatrixUtils.zeros(kOutputs);
    m_A = MatrixUtils.zeros(kStates, kStates);
    m_B = MatrixUtils.zeros(kStates, kInputs);
    m_delayedU = MatrixUtils.zeros(kInputs);
    updateAB(m_nominalSamplePeriod);
  }

  /**
   * Computes the new x and y given the control input.
   *
   * @param u  The control input.
   * @param dt The timestep.
   */
  @SuppressWarnings("ParameterName")
  public void update(Matrix<Inputs, N1> u, double dt) {
    m_x = updateX(getX(), m_delayedU, dt);
    m_y = updateY(getX(), m_delayedU);
    m_delayedU = u;
  }

  /**
   * Computes the new x given the old x and the control input.
   *
   * <p>This is used by state observers directly to run updates based on state
   * estimate.
   *
   * @param x  The current state.
   * @param u  The control input.
   * @param dt The timestep.
   */
  @SuppressWarnings("ParameterName")
  public Matrix<States, N1> updateX(Matrix<States, N1> x, Matrix<Inputs, N1> u, double dt) {
    updateAB(dt);

    return getA().times(x).plus(getB().times(u));
  }

  /**
   * Computes the new y given the control input.
   *
   * <p>This is used by state observers directly to run updates based on state
   * estimate.
   *
   * @param x The current state.
   * @param u The control input.
   */
  @SuppressWarnings("ParameterName")
  public Matrix<Outputs, N1> updateY(Matrix<States, N1> x, Matrix<Inputs, N1> u) {
    return getC().times(x).plus(getD().times(u));
  }

  /**
   * Rediscretize the plant with the given timestep.
   *
   * @param dt Timestep.
   */
  @SuppressWarnings({"LineLength", "LocalVariableName"})
  private void updateAB(double dt) {
    SimpleMatrix MstateContinuous = new SimpleMatrix(0, 0);

    MstateContinuous = MstateContinuous.concatColumns(getCoefficients().getAcontinuous().times(dt).getStorage());
    MstateContinuous = MstateContinuous.concatColumns(getCoefficients().getBcontinuous().times(dt).getStorage());
    MstateContinuous = MstateContinuous.concatRows(new SimpleMatrix(1, 3));

    SimpleMatrix Mstate = SimpleMatrixUtils.expm(MstateContinuous);
    CommonOps_DDRM.extract(Mstate.getDDRM(), 0, 0, m_A.getStorage().getDDRM());
    CommonOps_DDRM.extract(Mstate.getDDRM(), 0, kStates.getNum(), m_B.getStorage().getDDRM());
  }

  public Nat<States> getStates() {
    return kStates;
  }

  public Nat<Inputs> getInputs() {
    return kInputs;
  }

  public Nat<Outputs> getOutputs() {
    return kOutputs;
  }
}
