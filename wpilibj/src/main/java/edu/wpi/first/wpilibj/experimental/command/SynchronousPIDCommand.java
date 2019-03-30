package edu.wpi.first.wpilibj.experimental.command;

import java.util.Set;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

import edu.wpi.first.wpilibj.experimental.controller.PIDController;

import static java.util.Objects.requireNonNull;

/**
 * A command that controls an output with a {@link PIDController}.  Runs forever by default - to add
 * exit conditions and/or other behavior, subclass this class.  The controller calculation and
 * output are performed synchronously in the command's execute() method.
 */
public class SynchronousPIDCommand extends SendableCommandBase {

  protected final PIDController m_controller;
  protected DoubleSupplier m_measurement;
  protected DoubleSupplier m_reference;
  protected DoubleConsumer m_useOutput;

  /**
   * Creates a new SynchronousPIDCommand, which controls the given output with a PIDController.
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param referenceSource   the controller's reference (aka setpoint)
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   */
  public SynchronousPIDCommand(PIDController controller,
                               DoubleSupplier measurementSource,
                               DoubleSupplier referenceSource,
                               DoubleConsumer useOutput,
                               Subsystem... requirements) {
    requireNonNull(controller);
    requireNonNull(measurementSource);
    requireNonNull(referenceSource);
    requireNonNull(useOutput);

    m_controller = controller;
    m_useOutput = useOutput;
    m_measurement = measurementSource;
    m_reference = referenceSource;
    m_requirements.addAll(Set.of(requirements));
  }

  /**
   * Creates a new SynchronousPIDCommand, which controls the given output with a PIDController. Sets
   * the reference to zero (constant).
   *
   * @param controller        the controller that controls the output.
   * @param measurementSource the measurement of the process variable
   * @param useOutput         the controller's output
   * @param requirements      the subsystems required by this command
   */
  public SynchronousPIDCommand(PIDController controller,
                               DoubleSupplier measurementSource,
                               DoubleConsumer useOutput,
                               Subsystem... requirements) {
    this(controller, measurementSource, () -> 0, useOutput, requirements);
  }

  @Override
  public void execute() {
    useOutput(m_controller.calculate(getReference(), getMeasurement()));
  }

  public final void setOutput(DoubleConsumer useOutput) {
    m_useOutput = useOutput;
  }

  public PIDController getController() {
    return m_controller;
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

  /**
   * Gets the reference for the controller.  Wraps the passed-in function for readability.
   *
   * @return the reference for the controller
   */
  private double getReference() {
    return m_reference.getAsDouble();
  }

  /**
   * Gets the measurement of the process variable. Wraps the passed-in function for readability.
   *
   * @return the measurement of the process variable
   */
  private double getMeasurement() {
    return m_measurement.getAsDouble();
  }

  /**
   * Uses the output of the controller.  Wraps the passed-in function for readability
   *
   * @param output the output value to use
   */
  private void useOutput(double output) {
    m_useOutput.accept(output);
  }
}
