/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.MatrixUtils;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;

/**
 * Combines a plant, controller, and observer for controlling a mechanism with
 * full state feedback.
 *
 * <p>For everything in this file, "inputs" and "outputs" are defined from the
 * perspective of the plant. This means U is an input and Y is an output
 * (because you give the plant U (powers) and it gives you back a Y (sensor
 * values). This is the opposite of what they mean from the perspective of the
 * controller (U is an output because that's what goes to the motors and Y is an
 * input because that's what comes back from the sensors).
 *
 * <p>For more on the underlying math, read
 * https://file.tavsys.net/control/state-space-guide.pdf.
 */
@SuppressWarnings({"ClassTypeParameterName", "MemberName"})
public class PeriodVariantLoop<States extends Num, Inputs extends Num, Outputs extends Num> {
  /**
   * The number of states, inputs, and outputs for this system.
   */
  protected final Nat<States> kStates;
  protected final Nat<Inputs> kInputs;
  protected final Nat<Outputs> kOutputs;

  protected PeriodVariantPlant<States, Inputs, Outputs> m_plant;
  protected PeriodVariantController<States, Inputs, Outputs> m_controller;
  protected PeriodVariantObserver<States, Inputs, Outputs> m_observer;

  // Reference to go to in the next cycle (used by feedforward controller).
  protected Matrix<States, N1> m_nextR;

  /**
   * Constructs a period-variant loop with the given plant, controller, and
   * observer.
   *
   * @param plant      Period-variant plant.
   * @param controller Period-variant controller.
   * @param observer   Period-variant observer.
   */
  public PeriodVariantLoop(PeriodVariantPlant<States, Inputs, Outputs> plant,
                           PeriodVariantController<States, Inputs, Outputs> controller,
                           PeriodVariantObserver<States, Inputs, Outputs> observer) {
    m_plant = plant;
    m_controller = controller;
    m_observer = observer;
    kStates = m_plant.getStates();
    kInputs = m_plant.getInputs();
    kOutputs = m_plant.getOutputs();
  }

  /**
   * A convenience constructor for constructing a period-variant loop with the
   * given plant, controller, and observer coefficients.
   *
   * @param plantCoeffs      Period-variant plant coefficients.
   * @param controllerCoeffs Period-variant controller coefficients.
   * @param observerCoeffs   Period-variant observer coefficients.
   */
  public PeriodVariantLoop(PeriodVariantPlantCoeffs<States, Inputs, Outputs> plantCoeffs,
                           StateSpaceControllerCoeffs<States, Inputs, Outputs> controllerCoeffs,
                           PeriodVariantObserverCoeffs<States, Inputs, Outputs> observerCoeffs) {
    m_plant = new PeriodVariantPlant<>(plantCoeffs);
    m_controller = new PeriodVariantController<>(controllerCoeffs, m_plant);
    m_observer = new PeriodVariantObserver<>(observerCoeffs, m_plant);
    kStates = m_plant.getStates();
    kInputs = m_plant.getInputs();
    kOutputs = m_plant.getOutputs();
  }

  /**
   * Enables the controller.
   */
  public void enable() {
    m_controller.enable();
  }

  /**
   * Disables the controller and zeros the control input.
   */
  public void disable() {
    m_controller.disable();
  }

  /**
   * Returns the observer's state estimate x-hat.
   */
  public Matrix<States, N1> getXhat() {
    return m_observer.getXhat();
  }

  /**
   * Returns an element of the observer's state estimate x-hat.
   *
   * @param i Row of x-hat.
   */
  @SuppressWarnings("ParameterName")
  public double getXhat(int i) {
    return m_observer.getXhat(i);
  }

  /**
   * Returns the controller's next reference r.
   */
  public Matrix<States, N1> getNextR() {
    return m_nextR;
  }

  /**
   * Returns an element of the controller's next reference r.
   *
   * @param i Row of r.
   */
  @SuppressWarnings("ParameterName")
  public double getNextR(int i) {
    return getNextR().get(i, 0);
  }

  /**
   * Returns the controller's calculated control input u.
   */
  public Matrix<Inputs, N1> getU() {
    return m_controller.getU();
  }

  /**
   * Returns an element of the controller's calculated control input u.
   *
   * @param i Row of u.
   */
  @SuppressWarnings("ParameterName")
  public double getU(int i) {
    return m_controller.getU(i);
  }

  /**
   * Set the initial state estimate x-hat.
   *
   * @param xHat The initial state estimate x-hat.
   */
  @SuppressWarnings("ParameterName")
  public void setXhat(Matrix<States, N1> xHat) {
    m_observer.setXhat(xHat);
  }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param i     Row of x-hat.
   * @param value Value for element of x-hat.
   */
  @SuppressWarnings("ParameterName")
  public void setXhat(int i, double value) {
    m_observer.setXhat(i, value);
  }

  /**
   * Set the next reference r.
   *
   * @param nextR Next reference.
   */
  public void setNextR(Matrix<States, N1> nextR) {
    m_nextR = nextR;
  }

  /**
   * Return the plant used internally.
   */
  public PeriodVariantPlant<States, Inputs, Outputs> getPlant() {
    return m_plant;
  }

  /**
   * Return the controller used internally.
   */
  public PeriodVariantController<States, Inputs, Outputs> getController() {
    return m_controller;
  }

  /**
   * Return the observer used internally.
   */
  public PeriodVariantObserver<States, Inputs, Outputs> getObserver() {
    return m_observer;
  }

  /**
   * Zeroes reference r, controller output u, plant output y, and state estimate
   * x-hat.
   */
  public void reset() {
    m_plant.reset();
    m_controller.reset();
    m_observer.reset();
    m_nextR = MatrixUtils.zeros(kStates);
  }

  /**
   * Returns difference between reference r and x-hat.
   */
  public Matrix<States, N1> getError() {
    return m_controller.getR().minus(m_observer.getXhat());
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param y Measurement vector.
   */
  @SuppressWarnings("ParameterName")
  public void correct(Matrix<Outputs, N1> y) {
    m_observer.correct(m_controller.getU(), y);
  }

  /**
   * Sets new controller output, projects model forward, and runs observer
   * prediction.
   *
   * <p>After calling this, the user should send the elements of u to the
   * actuators.
   *
   * @param dt Timestep for prediction. (in seconds)
   */
  public void predict(double dt) {
    m_controller.update(getXhat(), m_nextR);

    m_observer.predict(m_controller.getU(), dt);
  }

  /**
   * Sets the current controller to be "index". This can be used for gain
   * scheduling.
   *
   * @param index The controller index.
   */
  public void setIndex(int index) {
    m_plant.setIndex(index);
    m_controller.setIndex(index);
    m_observer.setIndex(index);
  }

  /**
   * Returns the current controller index.
   */
  public int getIndex() {
    return m_plant.getIndex();
  }

  /**
   * Returns whether this loop is enabled.
   *
   * @return Whether the output of the loop is enabled.
   */
  public boolean isEnabled() {
    return m_controller.isEnabled();
  }
}
