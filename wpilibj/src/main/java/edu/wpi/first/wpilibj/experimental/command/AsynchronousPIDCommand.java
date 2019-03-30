package edu.wpi.first.wpilibj.experimental.command;

import java.util.Set;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

import edu.wpi.first.wpilibj.experimental.controller.AsynchronousControllerRunner;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;

import static java.util.Objects.requireNonNull;

/**
 * A command that controls an output with a {@link PIDController}.  Runs forever by default - to add
 * exit conditions and/or other behavior, subclass this class.  The controller calculation and
 * output are performed asynchronously by a separate thread with the period specified by the
 * controller.
 *
 * <p>While this class does more than {@link AsynchronousPIDSubsystem} to ensure
 * thread-safety (as it is a fully usable implementation rather than a base for a user
 * implementation), care should still be taken when using this class to ensure code remains
 * thread-safe.  If you are unfamiliar with thread-safety, consider using
 * {@link SynchronousPIDSubsystem}.
 */
public class AsynchronousPIDCommand extends SendableCommandBase {

  protected final PIDController m_controller;
  protected DoubleSupplier m_measurement;
  protected DoubleSupplier m_reference;
  protected DoubleConsumer m_useOutput;
  protected AsynchronousControllerRunner m_runner;

  /**
   * Creates a new AsynchronousPIDCommand, which controls the given output with a PIDController.
   *
   * @param controller        the controller that controls the output
   * @param measurementSource the measurement of the process variable
   * @param referenceSource   the controller's reference (aka setpoint)
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   */
  public AsynchronousPIDCommand(PIDController controller,
                                DoubleSupplier measurementSource,
                                DoubleSupplier referenceSource,
                                DoubleConsumer useOutput,
                                Subsystem... requirements) {
    requireNonNull(controller);
    requireNonNull(measurementSource);
    requireNonNull(referenceSource);
    requireNonNull(useOutput);

    m_controller = controller;
    m_measurement = measurementSource;
    m_reference = referenceSource;
    m_useOutput = useOutput;
    m_requirements.addAll(Set.of(requirements));
    m_runner = new AsynchronousControllerRunner(m_controller, this::getReference,
        this::getMeasurement, this::useOutput);
  }

  /**
   * Creates a new AsynchronousPIDCommand, which controls the given output with a PIDController.
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param useOutput         the controller's output; should be a synchronized method to remain
   *                          threadsafe
   * @param requirements      the subsystems required by this command
   */
  public AsynchronousPIDCommand(PIDController controller,
                                DoubleSupplier measurementSource,
                                DoubleConsumer useOutput,
                                Subsystem... requirements) {
    this(controller, measurementSource, () -> 0, useOutput, requirements);
  }

  @Override
  public void initialize() {
    m_runner.enable();
  }

  @Override
  public void execute() {
    m_controller.setReference(m_reference.getAsDouble());
  }

  @Override
  public void end(boolean interrupted) {
    m_runner.disable();
  }

  public PIDController getController() {
    return m_controller;
  }

  /**
   * Sets the reference of the PIDController to a constant value.
   *
   * @param reference the reference for the PIDController
   */
  public synchronized void setReference(double reference) {
    m_reference = () -> reference;
  }

  /**
   * Sets the reference of the PIDController to the current value plus a specified value.  The set
   * reference will be constant.
   *
   * @param relativeReference the amount by which to increase the reference
   */
  public synchronized void setReferenceRelative(double relativeReference) {
    setReference(m_controller.getReference() + relativeReference);
  }

  /**
   * Gets the reference for the controller.  Wraps the passed-in function so that changes to the
   * function by a subclass are seen by the runner.
   *
   * @return the reference for the controller
   */
  private synchronized double getReference() {
    return m_reference.getAsDouble();
  }

  /**
   * Gets the measurement of the process variable.  Wraps the passed-in function so that changes to
   * the function by a subclass are seen by the runner.
   *
   * @return the measurement of the process variable
   */
  private synchronized double getMeasurement() {
    return m_measurement.getAsDouble();
  }

  /**
   * Uses the output of the controller.  Wraps the passed-in function so that changes to the
   * function by a subclass are seen by the runner.
   *
   * @param output the output to use
   */
  private synchronized void useOutput(double output) {
    m_useOutput.accept(output);
  }
}
