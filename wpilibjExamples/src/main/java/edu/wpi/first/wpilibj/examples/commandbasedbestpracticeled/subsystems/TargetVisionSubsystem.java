// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.subsystems;

/*
 * Example subsystem that acquires a target (simulated fake boolean by pressing "A" button).
 * A signal is displayed to indicate the status of the Target Vision (acquired target or not).
 */

import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.Commands;
import edu.wpi.first.wpilibj2.command.SubsystemBase;
import edu.wpi.first.wpilibj2.command.button.CommandXboxController;
import edu.wpi.first.wpilibj2.command.button.Trigger;
import frc.robot.Color;
import frc.robot.LEDPattern;
import frc.robot.subsystems.RobotSignals.LEDView;

public class TargetVisionSubsystem extends SubsystemBase {

  private final LEDView robotSignals;
  private final CommandXboxController operatorController;
  public final Trigger targetAcquired = new Trigger(this::canSeeTarget);

  public TargetVisionSubsystem(LEDView robotSignals, CommandXboxController operatorController) {

    this.robotSignals = robotSignals;
    this.operatorController = operatorController;
    targetAcquired.whileTrue(targetAcquired());
  }
  
  public Command targetAcquired() {

    LEDPattern targetAcquiredSignal = LEDPattern.solid(Color.kOrange);
    return
                  
      robotSignals.setSignal(targetAcquiredSignal) // this command locks the robotSignals.Top subsystem only
      .andThen(Commands.idle(this).withTimeout(0.)) // command created in this subsystem will lock this subsystem also
      /*composite*/
      .withName("LedVisionTargetInSight")
      .ignoringDisable(true); // ignore disable true must be here on the composite; on the first command doesn't do anything
  }

  private boolean canSeeTarget() {
    
    return operatorController.getHID().getAButton(); // fake event source for target acquired
  }

  /**
   * Disallow default command
   * This prevents accidentally assuming the default command will run in composite commands which it wont
   * unless using "disjointSequence()".
   */
  @Override
  public void setDefaultCommand(Command def) {

      throw new IllegalArgumentException("Default Command not allowed");
  }

  public void beforeCommands() {}

  public void afterCommands() {}
}
