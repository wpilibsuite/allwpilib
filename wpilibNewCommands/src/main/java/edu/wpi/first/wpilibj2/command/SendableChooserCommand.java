// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;
import java.util.function.Supplier;

public class SendableChooserCommand extends Command {
  private final Supplier<Command> m_commandSupplier;

  public SendableChooserCommand(SendableChooser<Command> commandSupplier) {
    m_commandSupplier = commandSupplier::getSelected;
  }

  @Override
  public void initialize() {
    CommandScheduler.getInstance().schedule(m_commandSupplier.get().asProxy());
  }
}
