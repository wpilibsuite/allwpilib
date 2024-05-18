// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

import static edu.wpi.first.units.Units.Seconds;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;
import frc.robot.Color;
import frc.robot.LEDPattern;
import frc.robot.subsystems.RobotSignals.LEDView;

public class IntakeSubsystem extends SubsystemBase {
  
  private final LEDView robotSignals;
  private final CommandXboxController operatorController;
  public final Trigger gamePieceAcquired = new Trigger(this::hasGamePieceAcquired);

  public IntakeSubsystem(LEDView robotSignals, CommandXboxController operatorController) {

    this.robotSignals = robotSignals;
    this.operatorController = operatorController;
    gamePieceAcquired.whileTrue(gamePieceAcquired());
  }

  public Command gamePieceAcquired() {

    LEDPattern gamePieceAcquiredSignal = LEDPattern.solid(Color.kMagenta).blink(Seconds.of(0.2));
    return
      robotSignals.setSignal(gamePieceAcquiredSignal) // this command locks the robotSignals.Main subsystem only
        .ignoringDisable(true)
        .withName("MainGamePieceAcquiredSignal");
  }

  private boolean hasGamePieceAcquired() {

    return operatorController.getHID().getBButton(); // fake event source for game piece acquired
  }
  
  public void beforeCommands() {}

  public void afterCommands() {}
}
