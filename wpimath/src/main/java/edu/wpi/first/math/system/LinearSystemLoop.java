// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system;

import edu.wpi.first.math.Matrix;
import edu.wpi.first.math.Num;
import edu.wpi.first.math.StateSpaceUtil;
import edu.wpi.first.math.controller.LinearPlantInversionFeedforward;
import edu.wpi.first.math.controller.LinearQuadraticRegulator;
import edu.wpi.first.math.estimator.KalmanFilter;
import edu.wpi.first.math.numbers.N1;
import java.util.function.Function;
import org.ejml.MatrixDimensionException;
import org.ejml.simple.SimpleMatrix;

/**
 * Combines a controller, feedforward, and observer for controlling a mechanism with full state
 * feedback.
 *
 * <p>For everything in this file, "inputs" and "outputs" are defined from the perspective of the
 * plant. This means U is an input and Y is an output (because you give the plant U (powers) and it
 * gives you back a Y (sensor values)). This is the opposite of what they mean from the perspective
 * of the controller (U is an output because that's what goes to the motors and Y is an input
 * because that's what comes back from the sensors).
 *
 * <p>For more on the underlying math, read <a
 * href="https://file.tavsys.net/control/controls-engineering-in-frc.pdf">https://file.tavsys.net/control/controls-engineering-in-frc.pdf</a>.
 *
 * @param <States> Number of states.
 * @param <Inputs> Number of inputs.
 * @param <Outputs> Number of outputs.
 */
public class LinearSystemLoop<States extends Num, Inputs extends Num, Outputs extends Num> {
  private final LinearQuadraticRegulator<States, Inputs, Outputs> m_controller;
  private final LinearPlantInversionFeedforward<States, Inputs, Outputs> m_feedforward;
  private final KalmanFilter<States, Inputs, Outputs> m_observer;
  private Matrix<States, N1> m_nextR;
  private Function<Matrix<Inputs, N1>, Matrix<Inputs, N1>> m_clampFunction;

  /**
   * Constructs a state-space loop with the given plant, controller, and observer. By default, the
   * initial reference is all zeros. Users should call reset with the initial system state before
   * enabling the loop. This constructor assumes that the input(s) to this system are voltage.
   *
   * @param plant State-space plant.
   * @param controller State-space controller.
   * @param observer State-space observer.
   * @param maxVoltageVolts The maximum voltage that can be applied. Commonly 12.
   * @param dtSeconds The nominal timestep.
   */
  public LinearSystemLoop(
      LinearSystem<States, Inputs, Outputs> plant,
      LinearQuadraticRegulator<States, Inputs, Outputs> controller,
      KalmanFilter<States, Inputs, Outputs> observer,
      double maxVoltageVolts,
      double dtSeconds) {
    this(
        controller,
        new LinearPlantInversionFeedforward<>(plant, dtSeconds),
        observer,
        u -> StateSpaceUtil.desaturateInputVector(u, maxVoltageVolts));
  }

  /**
   * Constructs a state-space loop with the given plant, controller, and observer. By default, the
   * initial reference is all zeros. Users should call reset with the initial system state before
   * enabling the loop.
   *
   * @param plant State-space plant.
   * @param controller State-space controller.
   * @param observer State-space observer.
   * @param clampFunction The function used to clamp the input U.
   * @param dtSeconds The nominal timestep.
   */
  public LinearSystemLoop(
      LinearSystem<States, Inputs, Outputs> plant,
      LinearQuadraticRegulator<States, Inputs, Outputs> controller,
      KalmanFilter<States, Inputs, Outputs> observer,
      Function<Matrix<Inputs, N1>, Matrix<Inputs, N1>> clampFunction,
      double dtSeconds) {
    this(
        controller,
        new LinearPlantInversionFeedforward<>(plant, dtSeconds),
        observer,
        clampFunction);
  }

  /**
   * Constructs a state-space loop with the given controller, feedforward and observer. By default,
   * the initial reference is all zeros. Users should call reset with the initial system state
   * before enabling the loop.
   *
   * @param controller State-space controller.
   * @param feedforward Plant inversion feedforward.
   * @param observer State-space observer.
   * @param maxVoltageVolts The maximum voltage that can be applied. Assumes that the inputs are
   *     voltages.
   */
  public LinearSystemLoop(
      LinearQuadraticRegulator<States, Inputs, Outputs> controller,
      LinearPlantInversionFeedforward<States, Inputs, Outputs> feedforward,
      KalmanFilter<States, Inputs, Outputs> observer,
      double maxVoltageVolts) {
    this(
        controller,
        feedforward,
        observer,
        u -> StateSpaceUtil.desaturateInputVector(u, maxVoltageVolts));
  }

  /**
   * Constructs a state-space loop with the given controller, feedforward, and observer. By default,
   * the initial reference is all zeros. Users should call reset with the initial system state
   * before enabling the loop.
   *
   * @param controller State-space controller.
   * @param feedforward Plant inversion feedforward.
   * @param observer State-space observer.
   * @param clampFunction The function used to clamp the input U.
   */
  public LinearSystemLoop(
      LinearQuadraticRegulator<States, Inputs, Outputs> controller,
      LinearPlantInversionFeedforward<States, Inputs, Outputs> feedforward,
      KalmanFilter<States, Inputs, Outputs> observer,
      Function<Matrix<Inputs, N1>, Matrix<Inputs, N1>> clampFunction) {
    this.m_controller = controller;
    this.m_feedforward = feedforward;
    this.m_observer = observer;
    this.m_clampFunction = clampFunction;

    m_nextR = new Matrix<>(new SimpleMatrix(controller.getK().getNumCols(), 1));
    reset(m_nextR);
  }

  /**
   * Returns the observer's state estimate x-hat.
   *
   * @return the observer's state estimate x-hat.
   */
  public Matrix<States, N1> getXHat() {
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
  public void setXHat(Matrix<States, N1> xhat) {
    getObserver().setXhat(xhat);
  }

  /**
   * Set an element of the initial state estimate x-hat.
   *
   * @param row Row of x-hat.
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
  public Matrix<States, N1> getNextR() {
    return m_nextR;
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
   * Set the next reference r.
   *
   * @param nextR Next reference.
   */
  public void setNextR(double... nextR) {
    if (nextR.length != m_nextR.getNumRows()) {
      throw new MatrixDimensionException(
          String.format(
              "The next reference does not have the "
                  + "correct number of entries! Expected %s, but got %s.",
              m_nextR.getNumRows(), nextR.length));
    }
    m_nextR = new Matrix<>(new SimpleMatrix(m_nextR.getNumRows(), 1, true, nextR));
  }

  /**
   * Returns the controller's calculated control input u plus the calculated feedforward u_ff.
   *
   * @return the calculated control input u.
   */
  public Matrix<Inputs, N1> getU() {
    return clampInput(m_controller.getU().plus(m_feedforward.getUff()));
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
   * Return the controller used internally.
   *
   * @return the controller used internally.
   */
  public LinearQuadraticRegulator<States, Inputs, Outputs> getController() {
    return m_controller;
  }

  /**
   * Return the feedforward used internally.
   *
   * @return the feedforward used internally.
   */
  public LinearPlantInversionFeedforward<States, Inputs, Outputs> getFeedforward() {
    return m_feedforward;
  }

  /**
   * Return the observer used internally.
   *
   * @return the observer used internally.
   */
  public KalmanFilter<States, Inputs, Outputs> getObserver() {
    return m_observer;
  }

  /**
   * Zeroes reference r and controller output u. The previous reference of the
   * PlantInversionFeedforward and the initial state estimate of the KalmanFilter are set to the
   * initial state provided.
   *
   * @param initialState The initial state.
   */
  public final void reset(Matrix<States, N1> initialState) {
    m_nextR.fill(0.0);
    m_controller.reset();
    m_feedforward.reset(initialState);
    m_observer.setXhat(initialState);
  }

  /**
   * Returns difference between reference r and current state x-hat.
   *
   * @return The state error matrix.
   */
  public Matrix<States, N1> getError() {
    return getController().getR().minus(m_observer.getXhat());
  }

  /**
   * Returns difference between reference r and current state x-hat.
   *
   * @param index The index of the error matrix to return.
   * @return The error at that index.
   */
  public double getError(int index) {
    return getController().getR().minus(m_observer.getXhat()).get(index, 0);
  }

  /**
   * Get the function used to clamp the input u.
   *
   * @return The clamping function.
   */
  public Function<Matrix<Inputs, N1>, Matrix<Inputs, N1>> getClampFunction() {
    return m_clampFunction;
  }

  /**
   * Set the clamping function used to clamp inputs.
   *
   * @param clampFunction The clamping function.
   */
  public void setClampFunction(Function<Matrix<Inputs, N1>, Matrix<Inputs, N1>> clampFunction) {
    this.m_clampFunction = clampFunction;
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param y Measurement vector.
   */
  public void correct(Matrix<Outputs, N1> y) {
    getObserver().correct(getU(), y);
  }

  /**
   * Sets new controller output, projects model forward, and runs observer prediction.
   *
   * <p>After calling this, the user should send the elements of u to the actuators.
   *
   * @param dtSeconds Timestep for model update.
   */
  public void predict(double dtSeconds) {
    var u =
        clampInput(
            m_controller
                .calculate(getObserver().getXhat(), m_nextR)
                .plus(m_feedforward.calculate(m_nextR)));
    getObserver().predict(u, dtSeconds);
  }

  /**
   * Clamp the input u to the min and max.
   *
   * @param unclampedU The input to clamp.
   * @return The clamped input.
   */
  public Matrix<Inputs, N1> clampInput(Matrix<Inputs, N1> unclampedU) {
    return m_clampFunction.apply(unclampedU);
  }
}
