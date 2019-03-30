package edu.wpi.first.wpilibj.experimental.command;

import edu.wpi.first.wpilibj.experimental.controller.AsynchronousControllerRunner;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;

import static java.util.Objects.requireNonNull;

/**
 * A subsystem uses a {@link PIDController} to control an output.  The controller is run
 * asynchronously by a separate thread with the period specified by the controller.
 *
 * <p>Care should be taken to ensure the implementation of this class is thread-safe; if you are not
 * familiar with thread safety, consider using {@link SynchronousPIDSubsystem} instead.
 */
public abstract class AsynchronousPIDSubsystem extends SendableSubsystemBase {

  protected final PIDController m_controller;
  protected final AsynchronousControllerRunner m_runner;

  /**
   * Creates a new AsynchronousPIDSubsystem.
   *
   * @param controller the controller to use
   */
  public AsynchronousPIDSubsystem(PIDController controller) {
    requireNonNull(controller);
    m_controller = controller;
    m_runner = new AsynchronousControllerRunner(m_controller, this::getReference,
        this::getMeasurement, this::useOutput);
  }

  /**
   * Uses the output from the PIDController.  Should be synchronized to remain threadsafe.
   *
   * @param output the output of the PIDController
   */
  public abstract void useOutput(double output);

  /**
   * Returns the reference(setpoint) used by the PIDController.  Should be synchronized to remain
   * threadsafe.
   *
   * @return the reference (setpoint) to be used by the controller
   */
  public abstract double getReference();

  /**
   * Returns the measurement of the process variable used by the PIDController.  Should be
   * synchronized to remain threadsafe.
   *
   * @return the measurement of the process variable
   */
  public abstract double getMeasurement();

  /**
   * Enables the PID control.
   */
  public void enable() {
    m_runner.enable();
  }

  /**
   * Disables the PID control.  Sets the output to zero.
   */
  public void disable() {
    m_runner.disable();
  }
}
