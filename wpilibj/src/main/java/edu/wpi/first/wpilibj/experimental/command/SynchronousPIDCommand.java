package edu.wpi.first.wpilibj.experimental.command;

import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;

import edu.wpi.first.wpilibj.experimental.controller.PIDController;

public class SynchronousPIDCommand implements Command {

  private PIDController m_controller;
  private DoubleSupplier m_reference;
  private DoubleConsumer m_useOutput;

  public SynchronousPIDCommand(PIDController controller,
                    DoubleSupplier referenceSource,
                    DoubleConsumer useOutput) {
    m_controller = controller;
    m_useOutput = useOutput;
    m_reference = referenceSource;
  }

  @Override
  public void initialize() {
  }

  @Override
  public void execute() {
    m_controller.setReference(m_reference.getAsDouble());
    m_useOutput.accept(m_controller.update());
  }

  @Override
  public void end() {
  }

  public final void setOutput(DoubleConsumer useOutput) {
    m_useOutput = useOutput;
  }

  public PIDController getController() {
    return m_controller;
  }
}
