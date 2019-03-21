package edu.wpi.first.wpilibj.experimental.command;

import java.util.function.BooleanSupplier;
import java.util.function.DoubleConsumer;

/**
 * A command that runs a method that takes a double (such as a motor set method) at a specified
 * value, and then sets it to zero when it ends.  Useful for running motors.  Has no end
 * condition as-is; either subclass it or use {@link Command#withTimeout(double)} or
 * {@link Command#withInterruptCondition(BooleanSupplier)} to give it one.
 */
public class RunOutputCommand extends SendableCommandBase {

  protected final DoubleConsumer m_useOutput;
  protected final double m_output;

  /**
   * Creates a new RunMotorCommand.
   *
   * @param useOutput    the method that uses the output
   * @param output       the value to send to the output
   * @param requirements the subsystems required by this command
   */
  public RunOutputCommand(DoubleConsumer useOutput, double output, Subsystem... requirements) {
    m_useOutput = useOutput;
    m_output = output;
    addRequirements(requirements);
  }

  @Override
  public void initialize() {
    m_useOutput.accept(m_output);
  }

  @Override
  public void end() {
    m_useOutput.accept(0);
  }
}
