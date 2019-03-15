package edu.wpi.first.wpilibj.experimental.command;

import edu.wpi.first.wpilibj.Sendable;
import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

public interface Subsystem extends Sendable {

  void periodic();

  default Command getDefaultCommand() {
    return null;
  }

  default Command getCurrentCommand() {
    return CommandScheduler.getInstance().requiring(this);
  }

  @Override
  default String getName() {
    return "";
  }

  @Override
  default void setName(String name) {
  }

  @Override
  default String getSubsystem() {
    return getName();
  }

  @Override
  default void setSubsystem(String subsystem) {
  }

  @Override
  default void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("Subsystem");

    builder.addBooleanProperty(".hasDefault", () -> getDefaultCommand() != null, null);
    builder.addStringProperty(".default",
        () -> getDefaultCommand() != null ? getDefaultCommand().getName() : "none", null);
    builder.addBooleanProperty(".hasCommand", () -> getCurrentCommand() != null, null);
    builder.addStringProperty(".command",
        () -> getCurrentCommand() != null ? getCurrentCommand().getName() : "none", null);
  }
}
