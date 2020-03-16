package edu.wpi.first.wpilibj.system;

import edu.wpi.first.wpilibj.controller.LinearQuadraticRegulator;
import edu.wpi.first.wpilibj.estimator.KalmanFilter;
import edu.wpi.first.wpiutil.math.Matrix;
import edu.wpi.first.wpiutil.math.Nat;
import edu.wpi.first.wpiutil.math.Num;
import edu.wpi.first.wpiutil.math.numbers.N1;
import org.ejml.simple.SimpleMatrix;

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
public class LinearSystemLoop<S extends Num, I extends Num,
        O extends Num> {

  private final Nat<S> m_states;
  private final Nat<I> m_inputs;
  private final Nat<O> m_outputs;
  private LinearSystem<S, I, O> m_plant;
  private LinearQuadraticRegulator<S, I, O> m_controller;
  private KalmanFilter<S, I, O> m_observer;
  private Matrix<S, N1> m_nextR;

  /**
   * Constructs a state-space loop with the given plant, controller, and
   * observer.
   *
   * @param states     Nat representing the states of the system.
   * @param inputs     Nat representing the inputs to the system.
   * @param outputs    Nat representing the outputs of the system.
   * @param plant      State-space plant.
   * @param controller State-space controller.
   * @param observer   State-space observer.
   */
  public LinearSystemLoop(Nat<S> states, Nat<I> inputs, Nat<O> outputs,
                          LinearSystem<S, I, O> plant,
                          LinearQuadraticRegulator<S, I, O> controller,
                          KalmanFilter<S, I, O> observer) {

    this.m_states = states;
    this.m_inputs = inputs;
    this.m_outputs = outputs;
    this.m_plant = plant;
    this.m_controller = controller;
    this.m_observer = observer;

    reset();
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
   * If the controller is enabled.
   *
   * @return if the controller is enabled.
   */
  public boolean isEnabled() {
    return getController().isEnabled();
  }

  /**
   * Returns the observer's state estimate x-hat.
   *
   * @return the observer's state estimate x-hat.
   */
  public Matrix<S, N1> getXHat() {
    return getObserver().getXhat();
  }

  /**
   * Returns an element of the observer's state estimate x-hat.
   *
   * @param row Row of x-hat.
   * @return the i-th element of the observer's state estimate x-hat.
   */
  public double getXHat(int row) {
    return getObserver().getXhat(row);
  }

  /**
   * Set the initial state estimate x-hat.
   *
   * @param xhat The initial state estimate x-hat.
   */
  public void setXHat(Matrix<S, N1> xhat) {
    getObserver().setXhat(xhat);
  }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param row     Row of x-hat.
   * @param value Value for element of x-hat.
   */
  public void setXHat(int row, double value) {
    getObserver().setXhat(row, value);
  }

  /**
   * Returns an element of the controller's next reference r.
   *
   * @param row Row of r.
   * @return the element i of the controller's next reference r.
   */
  public double getNextR(int row) {
    return getNextR().get(row, 0);
  }

  /**
   * Returns the controller's next reference r.
   *
   * @return the controller's next reference r.
   */
  public Matrix<S, N1> getNextR() {
    return m_nextR;
  }

  /**
   * Set the next reference r.
   *
   * @param nextR Next reference.
   */
  public void setNextR(Matrix<S, N1> nextR) {
    m_nextR = nextR;
  }

  /**
   * Returns the controller's calculated control input u.
   *
   * @return the calculated control input u.
   */
  public Matrix<I, N1> getU() {
    return m_plant.clampInput(m_controller.getU());
  }

  /**
   * Returns an element of the controller's calculated control input u.
   *
   * @param row Row of u.
   * @return the calculated control input u at the row i.
   */
  public double getU(int row) {
    return getU().get(row, 0);
  }

  /**
   * Return the plant used internally.
   *
   * @return the plant used internally.
   */
  public LinearSystem<S, I, O> getPlant() {
    return m_plant;
  }

  /**
   * Return the controller used internally.
   *
   * @return the controller used internally.
   */
  public LinearQuadraticRegulator<S, I, O> getController() {
    return m_controller;
  }

  /**
   * Return the observer used internally.
   *
   * @return the observer used internally.
   */
  public KalmanFilter<S, I, O> getObserver() {
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
    m_nextR = new Matrix<>(new SimpleMatrix(m_states.getNum(), 1));
  }

  /**
   * Returns difference between reference r and x-hat.
   *
   * @return the
   */
  public Matrix<S, N1> getError() {
    return getController().getR().minus(m_observer.getXhat());
  }

  /**
   * Returns difference between reference r and x-hat.
   *
   * @param index The index of the error matrix to return.
   * @return The error at that index.
   */
  public double getError(int index) {
    return (getController().getR().minus(m_observer.getXhat())).get(index, 0);
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param y Measurement vector.
   */
  @SuppressWarnings("ParameterName")
  public void correct(Matrix<O, N1> y) {
    getObserver().correct(getController().getU(), y);
  }

  /**
   * Sets new controller output, projects model forward, and runs observer
   * prediction.
   *
   * <p>After calling this, the user should send the elements of u to the
   * actuators.
   *
   * @param dtSeconds Timestep for model update.
   */
  public void predict(double dtSeconds) {
    getController().update(getObserver().getXhat(), m_nextR);
    getObserver().predict(getController().getU(), dtSeconds);
  }
}
