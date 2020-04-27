/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.system;

import java.util.function.Function;

import org.ejml.MatrixDimensionException;
import org.ejml.simple.SimpleMatrix;

import edu.wpi.first.wpilibj.controller.LinearPlantInversionFeedforward;
import edu.wpi.first.wpilibj.controller.LinearQuadraticRegulator;
import edu.wpi.first.wpilibj.estimator.KalmanFilter;
import edu.wpi.first.wpilibj.math.StateSpaceUtil;
import edu.wpi.first.wpiutil.math.Matrix;
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
public class LinearSystemLoop<S extends Num, I extends Num,
        O extends Num> {

  private final LinearSystem<S, I, O> m_plant;
  private final LinearQuadraticRegulator<S, I, O> m_controller;
  private final LinearPlantInversionFeedforward<S, I, O> m_feedforward;
  private final KalmanFilter<S, I, O> m_observer;
  private Matrix<S, N1> m_nextR;
  private Function<Matrix<I, N1>, Matrix<I, N1>> m_clampFunction;

  /**
   * Constructs a state-space loop with the given plant, controller, and
   * observer. By default, the initial reference is all zeros. Users should
   * call reset with the initial system state before enabling the loop. This
   * constructor assumes that the input(s) to this system are voltage.
   *
   * @param plant      State-space plant.
   * @param controller State-space controller.
   * @param observer   State-space observer.
   * @param maxVoltageVolts The maximum voltage that can be applied. Commonly 12.
   * @param dtSeconds The nominal timestep.
   */
  public LinearSystemLoop(LinearSystem<S, I, O> plant,
                          LinearQuadraticRegulator<S, I, O> controller,
                          KalmanFilter<S, I, O> observer,
                          double maxVoltageVolts,
                          double dtSeconds) {
    this(plant, controller,
        new LinearPlantInversionFeedforward<>(plant, dtSeconds), observer,
        u -> StateSpaceUtil.normalizeInputVector(u, maxVoltageVolts));
  }

  /**
   * Constructs a state-space loop with the given plant, controller, and
   * observer. By default, the initial reference is all zeros. Users should
   * call reset with the initial system state before enabling the loop.
   *
   * @param plant      State-space plant.
   * @param controller State-space controller.
   * @param observer   State-space observer.
   * @param clampFunction The function used to clamp the input U.
   * @param dtSeconds The nominal timestep.
   */
  public LinearSystemLoop(LinearSystem<S, I, O> plant,
                          LinearQuadraticRegulator<S, I, O> controller,
                          KalmanFilter<S, I, O> observer,
                          Function<Matrix<I, N1>, Matrix<I, N1>> clampFunction,
                          double dtSeconds) {
    this(plant, controller, new LinearPlantInversionFeedforward<>(plant, dtSeconds),
          observer, clampFunction);
  }

  /**
   * Constructs a state-space loop with the given plant, controller, and
   * observer. By default, the initial reference is all zeros. Users should
   * call reset with the initial system state before enabling the loop.
   *
   * @param plant      State-space plant.
   * @param controller State-space controller.
   * @param feedforward Plant inversion feedforward.
   * @param observer   State-space observer.
   * @param maxVoltageVolts The maximum voltage that can be applied. Assumes that the
   *                        inputs are voltages.
   */
  public LinearSystemLoop(LinearSystem<S, I, O> plant,
                          LinearQuadraticRegulator<S, I, O> controller,
                          LinearPlantInversionFeedforward<S, I, O> feedforward,
                          KalmanFilter<S, I, O> observer,
                          double maxVoltageVolts
  ) {
    this(plant, controller, feedforward,
          observer, u -> StateSpaceUtil.normalizeInputVector(u, maxVoltageVolts));
  }

  /**
   * Constructs a state-space loop with the given plant, controller, and
   * observer. By default, the initial reference is all zeros. Users should
   * call reset with the initial system state before enabling the loop.
   *
   * @param plant      State-space plant.
   * @param controller State-space controller.
   * @param feedforward Plant inversion feedforward.
   * @param observer   State-space observer.
   * @param clampFunction The function used to clamp the input U.
   */
  public LinearSystemLoop(LinearSystem<S, I, O> plant,
                          LinearQuadraticRegulator<S, I, O> controller,
                          LinearPlantInversionFeedforward<S, I, O> feedforward,
                          KalmanFilter<S, I, O> observer,
                          Function<Matrix<I, N1>, Matrix<I, N1>> clampFunction) {
    this.m_plant = plant;
    this.m_controller = controller;
    this.m_feedforward = feedforward;
    this.m_observer = observer;
    this.m_clampFunction = clampFunction;

    m_nextR = new Matrix<>(new SimpleMatrix(controller.getK().getNumCols(), 0));
    reset(m_nextR);
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
   * @param row   Row of x-hat.
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
   * Set the next reference r.
   *
   * @param nextR Next reference.
   */
  public void setNextR(double... nextR) {
    if (nextR.length != m_nextR.getNumRows()) {
      throw new MatrixDimensionException(String.format("The next reference does not have the "
                      + "correct number of entries! Expected %s, but got %s.",
              m_nextR.getNumRows(),
              nextR.length));
    }
    m_nextR = new Matrix<>(new SimpleMatrix(m_nextR.getNumRows(), 1, true, nextR));
  }

  /**
   * Returns the controller's calculated control input u plus the calculated feedforward u_ff.
   *
   * @return the calculated control input u.
   */
  public Matrix<I, N1> getU() {
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
   * Return the feedforward used internally.
   *
   * @return the feedforward used internally.
   */
  public LinearPlantInversionFeedforward<S, I, O> getFeedforward() {
    return m_feedforward;
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
   * Zeroes reference r, controller output u and plant output y.
   * The previous reference for PlantInversionFeedforward is set to the
   * initial reference.
   * @param initialReference The initial reference.
   */
  public void reset(Matrix<S, N1> initialReference) {
    m_controller.reset();
    m_feedforward.reset(initialReference);
    m_observer.reset();
    m_nextR.fill(0.0);
  }

  /**
   * Returns difference between reoid predict(double dtSference r and x-hat.
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
   * Get the function used to clamp the input u.
   * @return The clamping function.
   */
  public Function<Matrix<I, N1>, Matrix<I, N1>> getClampFunction() {
    return m_clampFunction;
  }

  /**
   * Set the clamping function used to clamp inputs.
   */
  public void setClampFunction(Function<Matrix<I, N1>, Matrix<I, N1>> clampFunction) {
    this.m_clampFunction = clampFunction;
  }

  /**
   * Correct the state estimate x-hat using the measurements in y.
   *
   * @param y Measurement vector.
   */
  @SuppressWarnings("ParameterName")
  public void correct(Matrix<O, N1> y) {
    getObserver().correct(getU(), y);
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
  @SuppressWarnings("LocalVariableName")
  public void predict(double dtSeconds) {
    var u = clampInput(m_controller.calculate(getObserver().getXhat(), m_nextR)
          .plus(m_feedforward.calculate(m_nextR)));
    getObserver().predict(u, dtSeconds);
  }

  /**
   * Clamp the input u to the min and max.
   *
   * @param unclampedU The input to clamp.
   * @return           The clamped input.
   */
  public Matrix<I, N1> clampInput(Matrix<I, N1> unclampedU) {
    return m_clampFunction.apply(unclampedU);
  }

}
