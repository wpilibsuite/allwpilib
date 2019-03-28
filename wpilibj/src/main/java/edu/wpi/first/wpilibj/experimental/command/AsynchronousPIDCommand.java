package edu.wpi.first.wpilibj.experimental.command;

import java.util.Set;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

import edu.wpi.first.wpilibj.experimental.controller.ControllerRunner;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;

import static java.util.Objects.requireNonNull;

/**
 * A command that controls an output with a PIDController.  Runs forever by default - to add
 * exit conditions and/or other behavior, subclass this class.  The controller calculation and
 * output are performed asynchronously by a separate thread with the period specified by
 * the controller.
 */
public class AsynchronousPIDCommand extends SendableCommandBase {

  private final PIDController m_controller;
  private DoubleSupplier m_reference;
  private ControllerRunner m_runner;

  /**
   * Creates a new AsynchronousPIDCommand, which controls the given output with a PIDController.
   *
   * @param controller      the controller that controls the output.
   * @param referenceSource the controller's reference (aka setpoint)
   * @param useOutput       the controller's output; should be a synchronized method to remain
   *                        threadsafe
   * @param requirements    the subsystems required by this command
   */
  public AsynchronousPIDCommand(PIDController controller,
                                DoubleSupplier referenceSource,
                                DoubleConsumer useOutput,
                                Subsystem... requirements) {
    requireNonNull(controller);
    requireNonNull(referenceSource);
    requireNonNull(useOutput);

    m_controller = controller;
    m_reference = referenceSource;
    m_requirements.addAll(Set.of(requirements));
    m_runner = new ControllerRunner(m_controller, useOutput::accept);
  }

  /**
   * Creates a new AsynchronousPIDCommand, which controls the given output with a PIDController.
   *
   * @param controller      the controller that controls the output.
   * @param useOutput       the controller's output; should be a synchronized method to remain
   *                        threadsafe
   * @param requirements    the subsystems required by this command
   */
  public AsynchronousPIDCommand(PIDController controller,
                                DoubleConsumer useOutput,
                                Subsystem... requirements) {
    this(controller, () -> 0, useOutput, requirements);
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

  /**
   * Sets the output to be used by the PIDController.
   *
   * @param useOutput the output to be used; should be a synchronized method to remain threadsafe
   */
  public final void setOutput(DoubleConsumer useOutput) {
    boolean enabled = m_runner.isEnabled();
    m_runner.disable();
    m_runner = new ControllerRunner(m_controller, useOutput::accept);
    if (enabled) {
      m_runner.enable();
    }
  }

  public PIDController getController() {
    return m_controller;
  }

  public ControllerRunner getRunner() {
    return m_runner;
  }

  public void setReference(DoubleSupplier referenceSource) {
    m_reference = referenceSource;
  }

  public void setReference(double reference) {
    setReference(() -> reference);
  }

  public void setReferenceRelative(double relativeReference) {
    setReference(m_controller.getReference() + relativeReference);
  }

  @Override
  public Set<Subsystem> getRequirements() {
    return m_requirements;
  }
}
