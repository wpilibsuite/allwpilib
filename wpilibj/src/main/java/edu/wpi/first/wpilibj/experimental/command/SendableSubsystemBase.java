package edu.wpi.first.wpilibj.experimental.command;

import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * A base for subsystems that handles registration in the constructor, and provides a more intuitive
 * method for setting the default command.
 */
public abstract class SendableSubsystemBase implements Subsystem, Sendable {

  private Command m_defaultCommand;
  protected String m_name = this.getClass().getSimpleName();

  public SendableSubsystemBase() {
    CommandScheduler.getInstance().registerSubsystem(this);
  }

  /**
   * Sets the default command for this subsystem.
   *
   * @param defaultCommand the default command to use
   */
  public void setDefaultCommand(Command defaultCommand) {
    m_defaultCommand = defaultCommand;
  }

  @Override
  public Command getDefaultCommand() {
    return m_defaultCommand;
  }

  @Override
  public String getName() {
    return m_name;
  }

  @Override
  public void setName(String name) {
    m_name = name;
  }

  @Override
  public String getSubsystem() {
    return getName();
  }

  @Override
  public void setSubsystem(String subsystem) {
    setName(subsystem);
  }

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Subsystem");

    builder.addBooleanProperty(".hasDefault", () -> getDefaultCommand() != null, null);
    builder.addStringProperty(".default",
        () -> getDefaultCommand() != null ? getDefaultCommand().getName() : "none", null);
    builder.addBooleanProperty(".hasCommand", () -> getCurrentCommand() != null, null);
    builder.addStringProperty(".command",
        () -> getCurrentCommand() != null ? getCurrentCommand().getName() : "none", null);
  }
}
