package edu.wpi.first.wpilibj2.command;

import java.util.function.Supplier;
import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;

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
