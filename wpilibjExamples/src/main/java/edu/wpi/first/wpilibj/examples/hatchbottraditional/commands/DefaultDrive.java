package edu.wpi.first.wpilibj.examples.hatchbottraditional.commands;

import java.util.function.DoubleSupplier;

import edu.wpi.first.wpilibj.examples.hatchbottraditional.subsystems.DriveSubsystem;
import edu.wpi.first.wpilibj.experimental.command.RunCommand;

public class DefaultDrive extends RunCommand {

  public DefaultDrive(DriveSubsystem drive, DoubleSupplier fwd, DoubleSupplier rot){
    super(() -> drive.arcadeDrive(fwd.getAsDouble(), rot.getAsDouble()), drive);
  }
}
