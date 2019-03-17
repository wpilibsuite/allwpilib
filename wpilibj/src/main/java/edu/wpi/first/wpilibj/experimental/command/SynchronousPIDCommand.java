package edu.wpi.first.wpilibj.experimental.command;

import java.util.Set;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

import edu.wpi.first.wpilibj.experimental.controller.PIDController;

/**
 * A command that controls an output with a PIDController.  Runs forever by default - to add
 * exit conditions and/or other behavior, subclass this class.  The controller calculation and
 * output are performed synchronously in the command's execute() method.
 */
public class SynchronousPIDCommand extends SendableCommandBase {

  private final PIDController m_controller;
  private DoubleSupplier m_reference;
  private DoubleConsumer m_useOutput;

  /**
   * Creates a new SynchronousPIDCommand, which controls the given output with a PIDController.
   *
   * @param controller      the controller that controls the output.
   * @param referenceSource the controller's reference (aka setpoint)
   * @param useOutput       the controller's output
   * @param requirements    the subsystems required by this command
   */
  public SynchronousPIDCommand(PIDController controller,
                               DoubleSupplier referenceSource,
                               DoubleConsumer useOutput,
                               Subsystem... requirements) {
    m_controller = controller;
    m_useOutput = useOutput;
    m_reference = referenceSource;
    m_requirements.addAll(Set.of(requirements));
  }

  @Override
  public void execute() {
    m_controller.setReference(m_reference.getAsDouble());
    m_useOutput.accept(m_controller.update());
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

  @Override
  public Set<Subsystem> getRequirements() {
    return m_requirements;
  }
}
