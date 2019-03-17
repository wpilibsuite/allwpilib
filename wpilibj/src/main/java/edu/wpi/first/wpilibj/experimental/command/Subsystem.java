package edu.wpi.first.wpilibj.experimental.command;

public interface Subsystem {

  default void periodic() {}

  default Command getDefaultCommand() {
    return null;
  }

  default Command getCurrentCommand() {
    return CommandScheduler.getInstance().requiring(this);
  }

}
