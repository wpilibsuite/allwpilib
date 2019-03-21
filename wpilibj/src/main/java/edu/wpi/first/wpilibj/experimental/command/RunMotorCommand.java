package edu.wpi.first.wpilibj.experimental.command;

import java.util.function.BooleanSupplier;

import edu.wpi.first.wpilibj.SpeedController;

/**
 * A command that runs a motor at a given speed, and turns the motor off when it ends.  Has no end
 * conditions as-is; either subclass it or use {@link Command#withTimeout(double)} or
 * {@link Command#withInterruptCondition(BooleanSupplier)} to give it one.
 */
public class RunMotorCommand extends SendableCommandBase {

  protected final SpeedController m_motor;
  protected final double m_output;

  /**
   * Creates a new RunMotorCommand.
   *
   * @param motor        the motor to run
   * @param output       the speed to run the motor at
   * @param requirements the subsystems required by this command
   */
  public RunMotorCommand(SpeedController motor, double output, Subsystem... requirements) {
    m_motor = motor;
    m_output = output;
    addRequirements(requirements);
  }

  @Override
  public void initialize() {
    m_motor.set(m_output);
  }

  @Override
  public void end() {
    m_motor.disable();
  }
}
