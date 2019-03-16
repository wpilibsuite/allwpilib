package edu.wpi.first.wpilibj.experimental.command;

import java.util.Set;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

import edu.wpi.first.wpilibj.experimental.controller.ControllerRunner;
import edu.wpi.first.wpilibj.experimental.controller.PIDController;

public class AsynchronousPIDCommand implements Command {

  private final PIDController m_controller;
  private DoubleSupplier m_reference;
  private final Set<Subsystem> m_requirements;
  private ControllerRunner m_runner;

  public AsynchronousPIDCommand(PIDController controller,
                               DoubleSupplier referenceSource,
                               DoubleConsumer useOutput,
                               Set<Subsystem> requirements) {
    m_controller = controller;
    m_reference = referenceSource;
    m_requirements = requirements;
    m_runner = new ControllerRunner(m_controller, useOutput::accept);
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
  public void end() {
    m_runner.disable();
  }

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

  @Override
  public Set<Subsystem> getRequirements() {
    return m_requirements;
  }
}
